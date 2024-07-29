/*
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Lena Ploetzke, Anna Wendler
*
* Contact: Martin J. Kuehn <Martin.Kuehn@DLR.de>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ide_secir/parameters_io.h"
#include "ide_secir/parameters.h"
#include "memilio/config.h"

#ifdef MEMILIO_HAS_JSONCPP

#include "ide_secir/model.h"
#include "ide_secir/infection_state.h"
#include "memilio/io/epi_data.h"
#include "memilio/io/io.h"
#include "memilio/utils/date.h"

#include <string>
#include <cmath>

namespace mio
{
namespace isecir
{

IOResult<void> set_initial_flows(Model& model, ScalarType dt, std::string const& path, Date date,
                                 ScalarType scale_confirmed_cases)
{
    //--- Preparations ---
    // Try to get RKI data from path.
    BOOST_OUTCOME_TRY(auto&& rki_data, mio::read_confirmed_cases_noage(path));
    auto max_date_entry = std::max_element(rki_data.begin(), rki_data.end(), [](auto&& a, auto&& b) {
        return a.date < b.date;
    });
    if (max_date_entry == rki_data.end()) {
        log_error("RKI data file is empty.");
        return failure(StatusCode::InvalidFileFormat, path + ", file is empty.");
    }
    auto max_date = max_date_entry->date;
    if (max_date < date) {
        log_error("Specified date does not exist in RKI data.");
        return failure(StatusCode::OutOfRange, path + ", specified date does not exist in RKI data.");
    }

    // Get (global) support_max to determine how many flows in the past we have to compute.
    ScalarType global_support_max         = model.get_global_support_max(dt);
    Eigen::Index global_support_max_index = Eigen::Index(std::ceil(global_support_max / dt));

    // m_transitions should be empty at the beginning.
    if (model.m_transitions.get_num_time_points() > 0) {
        model.m_transitions = TimeSeries<ScalarType>((int)InfectionTransition::Count);
    }
    if (model.m_populations.get_time(0) != 0) {
        model.m_populations.remove_last_time_point();
        model.m_populations.add_time_point<Eigen::VectorXd>(
            0, TimeSeries<ScalarType>::Vector::Constant((int)InfectionState::Count, 0));
    }

    // The Dead compartment needs to be set to 0 so that RKI data can be added correctly.
    model.m_populations[0][Eigen::Index(InfectionState::Dead)] = 0;

    // The first time we need is -4 * global_support_max.
    Eigen::Index start_shift = 4 * global_support_max_index;
    // The last time needed is dependent on the mean stay time in the Exposed compartment and
    // the mean stay time of asymptomatic individuals in InfectedNoSymptoms.
    ScalarType mean_ExposedToInfectedNoSymptoms =
        model.parameters.get<TransitionDistributions>()[Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms)]
            .get_mean(dt);
    ScalarType mean_InfectedNoSymptomsToInfectedSymptoms =
        model.parameters
            .get<TransitionDistributions>()[Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)]
            .get_mean(dt);
    ScalarType mean_InfectedSymptomsToInfectedSevere =
        model.parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToInfectedSevere]
            .get_mean();
    ScalarType mean_InfectedSevereToInfectedCritical =
        model.parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSevereToInfectedCritical]
            .get_mean();
    ScalarType mean_InfectedCriticalToDead =
        model.parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedCriticalToDead].get_mean();

    std::cout << "Mean values: " << mean_ExposedToInfectedNoSymptoms << ", "
              << mean_InfectedNoSymptomsToInfectedSymptoms << ", " << mean_InfectedSymptomsToInfectedSevere << ", "
              << mean_InfectedSevereToInfectedCritical << ", " << mean_InfectedCriticalToDead << std::endl;

    Eigen::Index last_time_index_needed =
        Eigen::Index(std::ceil((mean_ExposedToInfectedNoSymptoms + mean_InfectedNoSymptomsToInfectedSymptoms) / dt));
    // Create TimeSeries with zeros. The index of time zero is start_shift.
    for (Eigen::Index i = -start_shift; i <= last_time_index_needed; i++) {
        // Add time point.
        model.m_transitions.add_time_point(
            i * dt, TimeSeries<ScalarType>::Vector::Constant((int)InfectionTransition::Count, 0.));
    }

    //--- Calculate the flow InfectedNoSymptomsToInfectedSymptoms using the RKI data and store in the m_transitions object.---
    ScalarType min_offset_needed = std::ceil(
        model.m_transitions.get_time(0) -
        1); // Need -1 if first time point is integer and just the floor value if not, therefore use ceil and -1
    ScalarType max_offset_needed = std::ceil(model.m_transitions.get_last_time());

    bool min_offset_needed_avail = false;
    bool max_offset_needed_avail = false;
    // Go through the entries of rki_data and check if date is needed for calculation. Confirmed cases are scaled.
    // Define dummy variables to store the first and the last index of the TimeSeries where the considered entry of rki_data is potentially needed.
    Eigen::Index idx_needed_first = 0;
    Eigen::Index idx_needed_last  = 0;
    ScalarType time_idx           = 0;
    for (auto&& entry : rki_data) {
        int offset = get_offset_in_days(entry.date, date);
        if ((offset >= min_offset_needed) && (offset <= max_offset_needed)) {
            if (offset == min_offset_needed) {
                min_offset_needed_avail = true;
            }
            // Smallest index for which the entry is needed.
            idx_needed_first =
                Eigen::Index(std::max(std::floor((offset - model.m_transitions.get_time(0) - 1) / dt), 0.));
            // Biggest index for which the entry is needed.
            idx_needed_last = Eigen::Index(std::min(std::ceil((offset - model.m_transitions.get_time(0) + 1) / dt),
                                                    double(model.m_transitions.get_num_time_points() - 1)));

            for (Eigen::Index i = idx_needed_first; i <= idx_needed_last; i++) {
                time_idx = model.m_transitions.get_time(i);
                if (offset == int(std::floor(time_idx))) {
                    model.m_transitions[i][Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)] +=
                        (1 - (time_idx - std::floor(time_idx))) * scale_confirmed_cases * entry.num_confirmed;
                }
                if (offset == int(std::ceil(time_idx))) {
                    model.m_transitions[i][Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)] +=
                        (time_idx - std::floor(time_idx)) * scale_confirmed_cases * entry.num_confirmed;
                }
                if (offset == int(std::floor(time_idx - dt))) {
                    model.m_transitions[i][Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)] -=
                        (1 - (time_idx - dt - std::floor(time_idx - dt))) * scale_confirmed_cases * entry.num_confirmed;
                }
                if (offset == int(std::ceil(time_idx - dt))) {
                    model.m_transitions[i][Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)] -=
                        (time_idx - dt - std::floor(time_idx - dt)) * scale_confirmed_cases * entry.num_confirmed;
                }
            }

            // Compute Dead by shifting RKI data according to mean stay times.

            if (offset == std::floor(-mean_InfectedSymptomsToInfectedSevere - mean_InfectedSevereToInfectedCritical -
                                     mean_InfectedCriticalToDead)) {
                model.m_populations[0][Eigen::Index(InfectionState::Dead)] +=
                    (1 - (-mean_InfectedSymptomsToInfectedSevere - mean_InfectedSevereToInfectedCritical -
                          mean_InfectedCriticalToDead -
                          std::floor(-mean_InfectedSymptomsToInfectedSevere - mean_InfectedSevereToInfectedCritical -
                                     mean_InfectedCriticalToDead))) *
                    entry.num_deaths;
            }
            if (offset == std::ceil(-mean_InfectedSymptomsToInfectedSevere - mean_InfectedSevereToInfectedCritical -
                                    mean_InfectedCriticalToDead)) {
                model.m_populations[0][Eigen::Index(InfectionState::Dead)] +=
                    (-mean_InfectedSymptomsToInfectedSevere - mean_InfectedSevereToInfectedCritical -
                     mean_InfectedCriticalToDead -
                     std::floor(-mean_InfectedSymptomsToInfectedSevere - mean_InfectedSevereToInfectedCritical -
                                mean_InfectedCriticalToDead)) *
                    entry.num_deaths;
            }

            if (offset == 0) {
                model.m_total_confirmed_cases = scale_confirmed_cases * entry.num_confirmed;
            }

            if (offset == max_offset_needed) {
                max_offset_needed_avail = true;
            }
        }
    }
    if (!max_offset_needed_avail || !min_offset_needed_avail) {
        // TODO
        log_warning("Necessary range of dates needed to compute initial values does not exist in RKI data. Missing "
                    "dates were set to 0.");
        // return failure(StatusCode::OutOfRange, path + ", necessary range of dates does not exist in RKI data.");
    }

    //--- Calculate the flows "after" InfectedNoSymptomsToInfectedSymptoms. ---
    // Compute flow InfectedSymptomsToInfectedSevere for -3 * global_support_max, ..., 0.
    for (Eigen::Index i = -3 * global_support_max_index; i <= 0; i++) {
        model.compute_flow(Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere),
                           Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms), dt,
                           i + start_shift);
    }
    // Compute flow InfectedSevereToInfectedCritical for -2 * global_support_max, ..., 0.
    for (Eigen::Index i = -2 * global_support_max_index; i <= 0; i++) {
        model.compute_flow(Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical),
                           Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere), dt, i + start_shift);
    }
    // Compute flows from InfectedSymptoms, InfectedSevere and InfectedCritical to Recovered and
    // flow InfectedCriticalToDead for -global_support_max, ..., 0.
    for (Eigen::Index i = -global_support_max_index; i <= 0; i++) {
        // Compute flow InfectedSymptomsToRecovered.
        model.compute_flow(Eigen::Index(InfectionTransition::InfectedSymptomsToRecovered),
                           Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms), dt,
                           i + start_shift);
        // Compute flow InfectedSevereToRecovered.
        model.compute_flow(Eigen::Index(InfectionTransition::InfectedSevereToRecovered),
                           Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere), dt, i + start_shift);
        // Compute flow InfectedCriticalToRecovered.
        model.compute_flow(Eigen::Index(InfectionTransition::InfectedCriticalToRecovered),
                           Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical), dt, i + start_shift);
        // Compute flow InfectedCriticalToDead.
        model.compute_flow(Eigen::Index(InfectionTransition::InfectedCriticalToDead),
                           Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical), dt, i + start_shift);
    }

    //--- Calculate the remaining flows. ---
    // Compute flow ExposedToInfectedNoSymptoms for -2 * global_support_max, ..., 0.
    // Use mean value of the TransitionDistribution InfectedNoSymptomsToInfectedSymptoms for the calculation.
    Eigen::Index index_shift_mean = Eigen::Index(std::round(mean_InfectedNoSymptomsToInfectedSymptoms / dt));
    for (Eigen::Index i = -2 * global_support_max_index; i <= 0; i++) {
        model.m_transitions[i + start_shift][Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms)] =
            (1 / model.parameters.get<TransitionProbabilities>()[Eigen::Index(
                     InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)]) *
            model.m_transitions[i + start_shift + index_shift_mean]
                               [Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)];
    }

    // Compute flow SusceptibleToExposed for -global_support_max, ..., 0.
    // Use mean values of the TransitionDistribution ExposedToInfectedNoSymptoms and of the TransitionDistribution InfectedNoSymptomsToInfectedSymptoms for the calculation.
    index_shift_mean =
        Eigen::Index(std::round((mean_ExposedToInfectedNoSymptoms + mean_InfectedNoSymptomsToInfectedSymptoms) / dt));
    for (Eigen::Index i = -global_support_max_index; i <= 0; i++) {
        model.m_transitions[i + start_shift][Eigen::Index(InfectionTransition::SusceptibleToExposed)] =
            (1 / model.parameters.get<TransitionProbabilities>()[Eigen::Index(
                     InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)]) *
            model.m_transitions[i + start_shift + index_shift_mean]
                               [Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)];
    }

    // InfectedNoSymptomsToRecovered for -global_support_max, ..., 0.
    // If we previously calculated the transition ExposedToInfectedNoSymptoms, we can calculate this transition using the standard formula.
    for (Eigen::Index i = -global_support_max_index; i <= 0; i++) {
        model.compute_flow(Eigen::Index(InfectionTransition::InfectedNoSymptomsToRecovered),
                           Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms), dt, i + start_shift);
    }

    // At the end of the calculation, delete all time points that are not required for the simulation.
    auto transition_copy(model.m_transitions);
    model.m_transitions = TimeSeries<ScalarType>((int)InfectionTransition::Count);
    for (Eigen::Index i = -global_support_max_index; i <= 0; i++) {
        model.m_transitions.add_time_point(i * dt, transition_copy.get_value(i + start_shift));
    }

    return mio::success();
}

} // namespace isecir
} // namespace mio

#endif // MEMILIO_HAS_JSONCPP
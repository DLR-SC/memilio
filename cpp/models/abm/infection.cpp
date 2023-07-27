/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: David Kerkmann, Sascha Korf, Khoa Nguyen
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

#include "abm/infection.h"
#include <math.h>

namespace mio
{
namespace abm
{

Infection::Infection(VirusVariant virus, AgeGroup age, const GlobalInfectionParameters& params, TimePoint init_date,
                     InfectionState init_state, bool detected)
    : m_virus_variant(virus)
    , m_detected(detected)
{
    m_viral_load.start_date = draw_infection_course(age, params, init_date, init_state);

    auto vl_params = params.get<ViralLoadDistributions>()[{
        virus, age, VaccinationState::Unvaccinated}]; // TODO: change vaccination state

    m_viral_load.peak = vl_params.viral_load_peak.get_distribution_instance()(vl_params.viral_load_peak.params);
    m_viral_load.incline =
        vl_params.viral_load_incline.get_distribution_instance()(vl_params.viral_load_incline.params);
    m_viral_load.decline =
        vl_params.viral_load_decline.get_distribution_instance()(vl_params.viral_load_decline.params);
    m_viral_load.end_date =
        m_viral_load.start_date +
        days(int(m_viral_load.peak / m_viral_load.incline - m_viral_load.peak / m_viral_load.decline));

    auto inf_params  = params.get<InfectivityDistributions>()[{virus, age}];
    m_log_norm_alpha = inf_params.infectivity_alpha.get_distribution_instance()(inf_params.infectivity_alpha.params);
    m_log_norm_beta  = inf_params.infectivity_beta.get_distribution_instance()(inf_params.infectivity_beta.params);

    m_time_is_infected = m_infection_course.back().first - m_infection_course[0].first;
}

ScalarType Infection::get_viral_load(TimePoint t) const
{
    if (t >= m_viral_load.start_date && t <= m_viral_load.end_date) {
        if (t.days() <= m_viral_load.start_date.days() + m_viral_load.peak / m_viral_load.incline) {
            return m_viral_load.incline * (t - m_viral_load.start_date).days();
        }
        else {
            return m_viral_load.peak + m_viral_load.decline * (t.days() - m_viral_load.peak / m_viral_load.incline -
                                                               m_viral_load.start_date.days());
        }
    }
    else {
        return 0.;
    }
}

ScalarType Infection::get_infectivity(TimePoint t) const
{
    auto time_shift = (m_infection_course[1].first - m_infection_course[0].first) - minutes(1872);
    if (m_viral_load.start_date + time_shift >= t)
        return 0;
    ScalarType scaling_factor =
        double((m_viral_load.end_date - m_viral_load.start_date).seconds()) / double(m_time_is_infected.seconds());
    auto scaled_time       = TimePoint(0) + seconds(int(scaling_factor * (t.seconds() - time_shift.seconds()) -
                                                  (scaling_factor - 1) * m_viral_load.start_date.seconds()));
    ScalarType infectivity = 1 / (1 + exp(-(m_log_norm_alpha + m_log_norm_beta * get_viral_load(scaled_time))));
    if (m_infection_course.size() > 3 && m_infection_course[3].second == InfectionState::InfectedSevere) {
        return infectivity;
    }
    else {
        return 0.75 * infectivity;
    }
}

VirusVariant Infection::get_virus_variant() const
{
    return m_virus_variant;
}

InfectionState Infection::get_infection_state(TimePoint t) const
{
    if (t < m_infection_course[0].first)
        return InfectionState::Susceptible;

    return (*std::prev(std::upper_bound(m_infection_course.begin(), m_infection_course.end(), t,
                                        [](const TimePoint& s, std::pair<TimePoint, InfectionState> state) {
                                            return state.first > s;
                                        })))
        .second;
}

TimePoint Infection::get_infection_start() const
{
    return (*std::find_if(m_infection_course.begin(), m_infection_course.end(),
                          [](const std::pair<TimePoint, InfectionState>& inf) {
                              return (inf.second == InfectionState::Exposed);
                          }))
        .first;
}

void Infection::set_detected()
{
    m_detected = true;
}

bool Infection::is_detected() const
{
    return m_detected;
}

TimePoint Infection::draw_infection_course(AgeGroup age, const GlobalInfectionParameters& params, TimePoint init_date,
                                           InfectionState init_state)
{
    TimePoint start_date = draw_infection_course_backward(age, params, init_date, init_state);
    draw_infection_course_forward(age, params, init_date, init_state);
    return start_date;
}

void Infection::draw_infection_course_forward(AgeGroup age, const GlobalInfectionParameters& params,
                                              TimePoint init_date, InfectionState start_state)
{
    auto t = init_date;
    TimeSpan time_period{}; // time period for current infection state
    InfectionState next_state{start_state}; // next state to enter
    m_infection_course.push_back(std::pair<TimePoint, InfectionState>(t, next_state));
    auto uniform_dist = UniformDistribution<double>::get_instance();
    ScalarType v; // random draws
    while ((next_state != InfectionState::Recovered && next_state != InfectionState::Dead)) {
        switch (next_state) {
        case InfectionState::Exposed:
            // roll out how long until infected without symptoms
            time_period = days(params.get<IncubationPeriod>()[{m_virus_variant, age,
                                                               VaccinationState::Unvaccinated}]); // subject to change
            next_state  = InfectionState::InfectedNoSymptoms;
            break;
        case InfectionState::InfectedNoSymptoms:
            // roll out next infection step
            v = uniform_dist();
            if (v < 0.79) { // TODO: subject to change
                time_period = days(params.get<InfectedNoSymptomsToSymptoms>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                next_state  = InfectionState::InfectedSymptoms;
            }
            else {
                time_period = days(params.get<InfectedNoSymptomsToRecovered>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                next_state  = InfectionState::Recovered;
            }

            break;
        case InfectionState::InfectedSymptoms:
            // roll out next infection step
            v = uniform_dist();
            if (v < 0.08) { // TODO: subject to change
                time_period = days(params.get<InfectedSymptomsToSevere>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                next_state  = InfectionState::InfectedSevere;
            }
            else {
                time_period = days(params.get<InfectedSymptomsToRecovered>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                next_state  = InfectionState::Recovered;
            }
            break;
        case InfectionState::InfectedSevere:
            // roll out next infection step
            v = uniform_dist();
            if (v < 0.18) { // TODO: subject to change
                time_period = days(params.get<SevereToCritical>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                next_state  = InfectionState::InfectedCritical;
            }
            else {
                time_period = days(params.get<SevereToRecovered>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                next_state  = InfectionState::Recovered;
            }
            break;
        case InfectionState::InfectedCritical:
            // roll out next infection step
            v = uniform_dist();
            if (v < 0.22) { // TODO: subject to change
                time_period = days(params.get<CriticalToDead>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                next_state  = InfectionState::Dead;
            }
            else {
                time_period = days(params.get<CriticalToRecovered>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                next_state  = InfectionState::Recovered;
            }
            break;
        default:
            break;
        }
        t = t + time_period;
        m_infection_course.push_back({t, next_state});
    }
}

TimePoint Infection::draw_infection_course_backward(AgeGroup age, const GlobalInfectionParameters& params,
                                                    TimePoint init_date, InfectionState init_state)
{
    auto start_date = init_date;
    TimeSpan time_period{}; // time period for current infection state
    InfectionState previous_state{init_state}; // next state to enter
    auto uniform_dist = UniformDistribution<double>::get_instance();
    ScalarType v; // random draws
    while ((previous_state != InfectionState::Exposed)) {
        switch (previous_state) {

        case InfectionState::InfectedNoSymptoms:
            time_period    = days(params.get<IncubationPeriod>()[{
                m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
            previous_state = InfectionState::Exposed;
            break;

        case InfectionState::InfectedSymptoms:
            time_period    = days(params.get<InfectedNoSymptomsToSymptoms>()[{
                m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
            previous_state = InfectionState::InfectedNoSymptoms;
            break;

        case InfectionState::InfectedSevere:
            time_period    = days(params.get<InfectedSymptomsToSevere>()[{
                m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
            previous_state = InfectionState::InfectedSymptoms;
            break;

        case InfectionState::InfectedCritical:
            time_period    = days(params.get<SevereToCritical>()[{
                m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
            previous_state = InfectionState::InfectedSevere;
            break;

        case InfectionState::Recovered:
            // roll out next infection step
            v = uniform_dist();
            if (v < 1 / 4) {
                time_period    = days(params.get<InfectedNoSymptomsToRecovered>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                previous_state = InfectionState::InfectedNoSymptoms;
            }
            if (v < 2 / 4) { // TODO: subject to change
                time_period    = days(params.get<InfectedSymptomsToRecovered>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                previous_state = InfectionState::InfectedSymptoms;
            }
            else if (v < 3 / 4) {
                time_period    = days(params.get<SevereToRecovered>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                previous_state = InfectionState::InfectedSevere;
            }
            else {
                time_period    = days(params.get<CriticalToRecovered>()[{
                    m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
                previous_state = InfectionState::InfectedCritical;
            }
            break;

        case InfectionState::Dead:
            time_period    = days(params.get<CriticalToDead>()[{
                m_virus_variant, age, VaccinationState::Unvaccinated}]); // TODO: subject to change
            previous_state = InfectionState::InfectedCritical;
            break;

        default:
            break;
        }
        start_date = start_date - time_period;
        m_infection_course.insert(m_infection_course.begin(), {start_date, previous_state});
    }
    return start_date;
}

} // namespace abm
} // namespace mio

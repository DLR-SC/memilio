/*
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*
* Authors: David Kerkmann, Sascha Korf
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
#ifndef EPI_ABM_INFECTION_H
#define EPI_ABM_INFECTION_H

#include "abm/time.h"
#include "abm/infection_state.h"
#include "abm/virus_variant.h"
#include "abm/parameters.h"

#include <vector>
#include <cmath>

namespace mio
{
namespace abm
{

/**
 * @brief Models the ViralLoad for an Infection, modelled on a log_10 scale.
 */
struct ViralLoad {
    TimePoint start_date; ///< Start date of the ViralLoad concentration in the Person.
    TimePoint end_date; ///< End date of the ViralLoad concentration in the Person.
    ScalarType peak; ///< Peak amplitude of the ViralLoad.
    ScalarType incline; ///< Incline of the ViralLoad during incline phase in log_10 scale per day (always positive).
    ScalarType decline; ///< Decline of the ViralLoad during decline phase in log_10 scale per day (always negative).
};

template<typename FP=double>
class Infection
{

public:
    /**
     * @brief Create an Infection for a single Person.
     * @param[in] virus Virus type of the Infection.
     * @param[in] age AgeGroup to determine the ViralLoad course.
     * @param[in] init_date Date of initializing the Infection.
     * @param[in] init_state [Default: InfectionState::Exposed] #InfectionState at time of initializing the Infection.
     * @param[in] detected [Default: false] If the Infection is detected.
     */
    Infection(VirusVariant virus, AgeGroup age, const GlobalInfectionParameters<FP>& params, TimePoint init_date,
              InfectionState init_state = InfectionState::Exposed, bool detected = false)
        : m_virus_variant(virus)
        , m_detected(detected)
    {
        m_viral_load.start_date = draw_infection_course(age, params, init_date, init_state);

        auto vl_params    = params.template get<ViralLoadDistributions>()[{
                                                               virus, age, VaccinationState::Unvaccinated}]; // TODO: change vaccination state
        m_viral_load.peak = vl_params.viral_load_peak.get_distribution_instance()(vl_params.viral_load_peak.params);
        m_viral_load.incline =
            vl_params.viral_load_incline.get_distribution_instance()(vl_params.viral_load_incline.params);
        m_viral_load.decline =
            vl_params.viral_load_decline.get_distribution_instance()(vl_params.viral_load_decline.params);
        m_viral_load.end_date =
            m_viral_load.start_date +
            days(int(m_viral_load.peak / m_viral_load.incline - m_viral_load.peak / m_viral_load.decline));

        m_viral_load.end_date =
            m_viral_load.start_date +
            days(int(m_viral_load.peak / m_viral_load.incline - m_viral_load.peak / m_viral_load.decline));

        auto inf_params  = params.template get<InfectivityDistributions>()[{virus, age}];
        m_log_norm_alpha = inf_params.infectivity_alpha.get_distribution_instance()(inf_params.infectivity_alpha.params);
        m_log_norm_beta  = inf_params.infectivity_beta.get_distribution_instance()(inf_params.infectivity_beta.params);
    }


    /**
     * @brief Gets the ViralLoad of the Infection at a given TimePoint.
     * @param[in] t TimePoint of querry.
     */
    ScalarType get_viral_load(TimePoint t) const;

    /**
     * @brief Get infectivity at a given time.
     * Computed depending on current ViralLoad and individual invlogit function of each Person
     * corresponding to https://www.science.org/doi/full/10.1126/science.abi5273
     * @param[in] t TimePoint of the querry.
     * @return Infectivity at given TimePoint.
     */
    ScalarType get_infectivity(TimePoint t) const;

    /**
     * @brief: Get VirusVariant.
     * @return VirusVariant of the Infection.
     */
    VirusVariant get_virus_variant() const;

    /**
     * @brief Get the #InfectionState of the Infection.
     * @param[in] t TimePoint of the querry.
     * @return #InfectionState at the given TimePoint.
     */
    InfectionState get_infection_state(TimePoint t) const;

    /**
     * @brief Set the Infection to detected.
     */
    void set_detected();

    /**
     * @return Get the detected state.
     */
    bool is_detected() const;

private:
    /**
     * @brief Determine ViralLoad course and Infection course based on init_state.
     * Calls draw_infection_course_backward for all #InfectionState%s prior and draw_infection_course_forward for all
     * subsequent #InfectionState%s.
     * @param[in] age AgeGroup of the Person.
     * @param[in] params GlobalInfectionParameters.
     * @param[in] init_date Date of initializing the Infection.
     * @param[in] init_state #InfectionState at time of initializing the Infection.
     * @return The starting date of the Infection.
     */
    TimePoint draw_infection_course(AgeGroup age, const GlobalInfectionParameters<FP>& params, TimePoint init_date,
                                    InfectionState init_state);

    /**
     * @brief Determine ViralLoad course and Infection course prior to the given start_state.
     * @param[in] age AgeGroup of the Person.
     * @param[in] params GlobalInfectionParameters.
     * @param[in] init_date Date of initializing the Infection.
     * @param[in] init_state #InfectionState at time of initializing the Infection.
     */
    void draw_infection_course_forward(AgeGroup age, const GlobalInfectionParameters<FP>& params, TimePoint init_date,
                                       InfectionState init_state);

    /**
     * @brief Determine ViralLoad course and Infection course subsequent to the given start_state.
     * @param[in] age AgeGroup of the person.
     * @param[in] params GlobalInfectionParameters.
     * @param[in] init_date Date of initializing the Infection.
     * @param[in] init_state InfectionState at time of initializing the Infection.
     * @return The starting date of the Infection.
     */
    TimePoint draw_infection_course_backward(AgeGroup age, const GlobalInfectionParameters<FP>& params, TimePoint init_date,
                                             InfectionState init_state);

    std::vector<std::pair<TimePoint, InfectionState>> m_infection_course; ///< Start date of each #InfectionState.
    VirusVariant m_virus_variant; ///< Variant of the Infection.
    ViralLoad m_viral_load; ///< ViralLoad of the Infection.
    ScalarType m_log_norm_alpha,
        m_log_norm_beta; ///< Parameters for the infectivity mapping, which is modelled through an invlogit function.
    bool m_detected; ///< Whether an Infection is detected or not.
};

} // namespace abm
} // namespace mio

#endif

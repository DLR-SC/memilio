/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Anna Wendler, Lena Ploetzke, Martin J. Kuehn
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
#include "ide_secir/model.h"
#include "infection_state.h"
#include "memilio/config.h"
#include "memilio/utils/logging.h"

namespace mio
{
namespace isecir
{

Model::Model(TimeSeries<ScalarType>&& init, ScalarType N_init, ScalarType Dead_before,
             const ParameterSet& Parameterset_init)
    : parameters{Parameterset_init}
    , m_transitions{std::move(init)}
    , m_population{TimeSeries<ScalarType>(Eigen::Index(InfectionState::Count))}
    , m_N{N_init}
    , m_deaths_before{Dead_before}
{
    if (!((int)m_transitions.get_num_elements() == (int)InfectionTransition::Count)) {
        log_error("Initialization failed. Number of elements in init does not match the required number.");
    }
    m_population.add_time_point(0);
    m_population[Eigen::Index(0)][Eigen::Index(InfectionState::Dead)] =
        m_deaths_before + m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedCriticalToDead)];
}

void Model::compute_susceptibles(ScalarType dt)
{
    Eigen::Index num_time_points = m_population.get_num_time_points();
    // using number of susceptibles from previous time step and force of infection from previous time step:
    // compute current number of susceptibles and store susceptibles in m_population
    m_population.get_last_value()[Eigen::Index(InfectionState::Susceptible)] =
        m_population[num_time_points - 2][Eigen::Index(InfectionState::Susceptible)] / (1 + dt * m_forceofinfection);
}

void Model::update_forceofinfection(ScalarType dt, bool initialization)
{
    m_forceofinfection = 0;

    // determine the relevant calculation area = union of the supports of the relevant transition distributions
    ScalarType calc_time = std::max(
        {parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms]
             .get_xright(),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToRecovered]
             .get_xright(),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToInfectedSevere]
             .get_xright(),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToRecovered]
             .get_xright()});

    // corresponding index
    /* need calc_time_index timesteps in sum,
     subtract 1 because in the last summand all TransitionDistributions evaluate to 0 (by definition of xright)*/
    Eigen::Index calc_time_index = (Eigen::Index)std::ceil(calc_time / dt) - 1;

    Eigen::Index num_time_points{};
    ScalarType current_time{};
    ScalarType deaths{};

    if (initialization) {
        // determine m_forceofinfection at time -m_dt which is the penultimate timepoint in m_transitions
        num_time_points = m_transitions.get_num_time_points() - 1;
        current_time    = -dt;
        deaths          = m_deaths_before;
    }
    else {
        // determine m_forceofinfection for current last time in m_transitions.
        num_time_points = m_transitions.get_num_time_points();
        current_time    = m_transitions.get_last_time();
        deaths          = m_population.get_last_value()[Eigen::Index(InfectionState::Dead)];
    }

    for (Eigen::Index i = num_time_points - 1 - calc_time_index; i < num_time_points - 1; i++) {
        ScalarType infection_age = (num_time_points - 1 - i) * dt;
        m_forceofinfection +=
            parameters.get<TransmissionProbabilityOnContact>() *
            parameters.get<ContactPatterns>().get_cont_freq_mat().get_matrix_at(current_time)(0, 0) *
            ((parameters
                      .get<TransitionProbabilities>()[(int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms] *
                  parameters
                      .get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms]
                      .Distribution(infection_age) +
              parameters.get<TransitionProbabilities>()[(int)InfectionTransition::InfectedNoSymptomsToRecovered] *
                  parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToRecovered]
                      .Distribution(infection_age)) *
                 m_transitions[i + 1][Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms)] *
                 parameters.get<RelativeTransmissionNoSymptoms>() +
             (parameters.get<TransitionProbabilities>()[(int)InfectionTransition::InfectedSymptomsToInfectedSevere] *
                  parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToInfectedSevere]
                      .Distribution(infection_age) +
              parameters.get<TransitionProbabilities>()[(int)InfectionTransition::InfectedSymptomsToRecovered] *
                  parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToRecovered]
                      .Distribution(infection_age)) *
                 m_transitions[i + 1][Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)] *
                 parameters.get<RiskOfInfectionFromSymptomatic>());
    }
    m_forceofinfection = 1 / ((ScalarType)m_N - deaths) * m_forceofinfection;
}

void Model::compute_flow(int idx_InfectionTransitions, Eigen::Index idx_IncomingFlow, ScalarType dt)
{
    ScalarType sum = 0;

    /* If we have TransitionDistribution(m_dt*i)=0 for all i>= k (determined by the support of the distribution)
     then we have that the derivative of TransitionDistribution(m_dt*i) is equal to zero for all i>= k+1,
     since we are using a backwards difference scheme to compute the derivative. Hence calc_time_index goes until 
     std::ceil(xright/m_dt) since for std::ceil(xright/m_dt)+1 all terms are already zero. 
     This needs to be adjusted if we are changing the finite difference scheme */
    Eigen::Index calc_time_index =
        (Eigen::Index)std::ceil(parameters.get<TransitionDistributions>()[idx_InfectionTransitions].get_xright() / dt);
    Eigen::Index num_time_points = m_transitions.get_num_time_points();

    for (Eigen::Index i = num_time_points - 1 - calc_time_index; i < num_time_points - 1; i++) {
        // (num_time_points - 1 - i)* m_dt is the time, the individuals has already been infected
        ScalarType infection_age = (num_time_points - 1 - i) * dt;

        // backward difference scheme to approximate first derivative
        sum += (parameters.get<TransitionDistributions>()[idx_InfectionTransitions].Distribution(infection_age) -
                parameters.get<TransitionDistributions>()[idx_InfectionTransitions].Distribution(infection_age - dt)) /
               dt * m_transitions[i + 1][idx_IncomingFlow];
    }

    m_transitions.get_last_value()[Eigen::Index(idx_InfectionTransitions)] =
        (-dt) * parameters.get<TransitionProbabilities>()[idx_InfectionTransitions] * sum;
}

void Model::flows_current_timestep(ScalarType dt)
{
    // calculate flow from S to E with force of infection from previous time step und susceptibles from current time step
    m_transitions.get_last_value()[Eigen::Index(InfectionTransition::SusceptibleToExposed)] =
        dt * m_forceofinfection * m_population.get_last_value()[Eigen::Index(InfectionState::Susceptible)];
    // calculate all other flows with compute_flow
    // flow from E to C
    compute_flow((int)InfectionTransition::ExposedToInfectedNoSymptoms,
                 Eigen::Index(InfectionTransition::SusceptibleToExposed), dt);
    // flow from C to I
    compute_flow((int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms,
                 Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms), dt);
    // flow from C to R
    compute_flow((int)InfectionTransition::InfectedNoSymptomsToRecovered,
                 Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms), dt);
    // flow from I to H
    compute_flow((int)InfectionTransition::InfectedSymptomsToInfectedSevere,
                 Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms), dt);
    // flow from I to R
    compute_flow((int)InfectionTransition::InfectedSymptomsToRecovered,
                 Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms), dt);
    // flow from H to U
    compute_flow((int)InfectionTransition::InfectedSevereToInfectedCritical,
                 Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere), dt);
    // flow from to H to R
    compute_flow((int)InfectionTransition::InfectedSevereToRecovered,
                 Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere), dt);
    // flow from U to D
    compute_flow((int)InfectionTransition::InfectedCriticalToDead,
                 Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical), dt);
    // flow from U to R
    compute_flow((int)InfectionTransition::InfectedCriticalToRecovered,
                 Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical), dt);
}

void Model::compute_deaths()
{
    Eigen::Index num_time_points = m_population.get_num_time_points();

    m_population.get_last_value()[Eigen::Index(InfectionState::Dead)] =
        m_population[num_time_points - 2][Eigen::Index(InfectionState::Dead)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedCriticalToDead)];
}

void Model::compute_recovered()
{
    Eigen::Index num_time_points = m_population.get_num_time_points();

    m_population.get_last_value()[Eigen::Index(InfectionState::Recovered)] =
        m_population[num_time_points - 2][Eigen::Index(InfectionState::Recovered)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedNoSymptomsToRecovered)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedSymptomsToRecovered)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedSevereToRecovered)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedCriticalToRecovered)];
}

void Model::compute_compartment(Eigen::Index idx_InfectionState, Eigen::Index idx_IncomingFlow,
                                int idx_TransitionDistribution1, int idx_TransitionDistribution2, ScalarType dt)
{
    ScalarType sum = 0;

    // determine relevant calculation area and corresponding index
    ScalarType calc_time =
        std::max(parameters.get<TransitionDistributions>()[idx_TransitionDistribution1].get_xright(),
                 parameters.get<TransitionDistributions>()[idx_TransitionDistribution2].get_xright());

    Eigen::Index calc_time_index = (Eigen::Index)std::ceil(calc_time / dt);

    Eigen::Index num_time_points = m_transitions.get_num_time_points();

    for (Eigen::Index i = num_time_points - 1 - calc_time_index; i < num_time_points - 1; i++) {
        sum += (parameters.get<TransitionProbabilities>()[idx_TransitionDistribution1] *
                    parameters.get<TransitionDistributions>()[idx_TransitionDistribution1].Distribution(
                        (num_time_points - 1 - i) * dt) +
                (1 - parameters.get<TransitionProbabilities>()[idx_TransitionDistribution1]) *
                    parameters.get<TransitionDistributions>()[idx_TransitionDistribution2].Distribution(
                        (num_time_points - 1 - i) * dt)) *
               m_transitions[i + 1][idx_IncomingFlow];
    }

    m_population.get_last_value()[idx_InfectionState] = sum;
}

void Model::compartments_current_timestep_ECIHU(ScalarType dt)
{
    // E
    compute_compartment(Eigen::Index(InfectionState::Exposed), Eigen::Index(InfectionTransition::SusceptibleToExposed),
                        (int)InfectionTransition::ExposedToInfectedNoSymptoms, 0,
                        dt); // this is a dummy index as there is no transition from E to R in our model,
    // write any transition here as probability from E to R is 0
    // C
    compute_compartment(Eigen::Index(InfectionState::InfectedNoSymptoms),
                        Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms),
                        (int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms,
                        (int)InfectionTransition::InfectedNoSymptomsToRecovered, dt);
    // I
    compute_compartment(Eigen::Index(InfectionState::InfectedSymptoms),
                        Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms),
                        (int)InfectionTransition::InfectedSymptomsToInfectedSevere,
                        (int)InfectionTransition::InfectedSymptomsToRecovered, dt);
    // H
    compute_compartment(Eigen::Index(InfectionState::InfectedSevere),
                        Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere),
                        (int)InfectionTransition::InfectedSevereToInfectedCritical,
                        (int)InfectionTransition::InfectedSevereToRecovered, dt);
    // U
    compute_compartment(Eigen::Index(InfectionState::InfectedCritical),
                        Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical),
                        (int)InfectionTransition::InfectedCriticalToDead,
                        (int)InfectionTransition::InfectedCriticalToRecovered, dt);
}

void Model::initialize(ScalarType dt)
{
    // compute Susceptibles at time 0  and m_forceofinfection at time -m_dt as initial values for discretization scheme
    // use m_forceofinfection at -m_dt to be consistent with further calculations of S (see compute_susceptibles()),
    // where also the value of m_forceofinfection for the previous timestep is used
    update_forceofinfection(dt, true);
    m_population[Eigen::Index(0)][Eigen::Index(InfectionState::Susceptible)] =
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::SusceptibleToExposed)] / m_forceofinfection;

    //calculate other compartment sizes for t=0
    compartments_current_timestep_ECIHU(dt);

    //R; need an initial value for R, therefore do not calculate via compute_recovered()
    m_population.get_last_value()[Eigen::Index(InfectionState::Recovered)] =
        m_N - m_population.get_last_value()[Eigen::Index(InfectionState::Susceptible)] -
        m_population.get_last_value()[Eigen::Index(InfectionState::Exposed)] -
        m_population.get_last_value()[Eigen::Index(InfectionState::InfectedNoSymptoms)] -
        m_population.get_last_value()[Eigen::Index(InfectionState::InfectedSymptoms)] -
        m_population.get_last_value()[Eigen::Index(InfectionState::InfectedSevere)] -
        m_population.get_last_value()[Eigen::Index(InfectionState::InfectedCritical)] -
        m_population.get_last_value()[Eigen::Index(InfectionState::Dead)];

    // compute m_forceofinfection at time 0 needed for further simulation
    update_forceofinfection(dt);
}

} // namespace isecir
} // namespace mio

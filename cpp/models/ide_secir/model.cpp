/* 
* Copyright (C) 2020-2024 MEmilio
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
#include "ide_secir/parameters.h"
#include "infection_state.h"
#include "memilio/config.h"
#include "memilio/utils/logging.h"
#include "memilio/math/eigen.h"

namespace mio
{
namespace isecir
{

Model::Model(TimeSeries<ScalarType>&& init, ScalarType N_init, ScalarType deaths, ScalarType total_confirmed_cases,
             const ParameterSet& Parameterset_init)
    : parameters{Parameterset_init}
    , m_transitions{std::move(init)}
    , m_populations{TimeSeries<ScalarType>(Eigen::Index(InfectionState::Count))}
    , m_total_confirmed_cases{total_confirmed_cases}
    , m_N{N_init}
{
    if (m_transitions.get_num_time_points() > 0) {
        m_populations.add_time_point<Eigen::VectorXd>(
            m_transitions.get_last_time(), TimeSeries<ScalarType>::Vector::Constant((int)InfectionState::Count, 0.));
    }
    else {
        // Initialize m_populations with zero as the first point of time if no data is provided for the transitions.
        // This can happen for example in the case of initialization with real data.
        m_populations.add_time_point<Eigen::VectorXd>(
            0, TimeSeries<ScalarType>::Vector::Constant((int)InfectionState::Count, 0));
    }
    // Set deaths at simulation start time t0.
    m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Dead)] = deaths;
}

void Model::initialize(ScalarType dt)
{
    if (m_total_confirmed_cases > 1e-12) {
        m_initialization_method = 1;
        other_compartments_current_timestep(dt);

        // The scheme of the ODE model for initialization is applied here.
        m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Recovered)] =
            m_total_confirmed_cases - m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSymptoms)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSevere)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedCritical)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Dead)];

        m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Susceptible)] =
            m_N - m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Exposed)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedNoSymptoms)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSymptoms)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSevere)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedCritical)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Recovered)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Dead)];
    }
    else if (m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Susceptible)] > 1e-12) {
        // Take initialized value for Susceptibles if value can't be calculated via the standard formula.
        m_initialization_method = 2;
        // Calculate other compartment sizes for start time t0.
        other_compartments_current_timestep(dt);

        // R; need an initial value for R, therefore do not calculate via compute_recovered()
        m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Recovered)] =
            m_N - m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Susceptible)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Exposed)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedNoSymptoms)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSymptoms)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSevere)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedCritical)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Dead)];
    }
    else if (m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Recovered)] > 1e-12) {
        // If value for Recovered is initialized and standard method is not applicable (i.e., no value for total infections
        // or Susceptibles given directly), calculate Susceptibles via other compartments.
        // The calculation of other compartments' values is not dependent on Susceptibles at time t0, i.e., S(t0), but only on the transitions of the past.
        m_initialization_method = 3;
        other_compartments_current_timestep(dt);

        m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Susceptible)] =
            m_N - m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Exposed)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedNoSymptoms)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSymptoms)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSevere)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedCritical)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Recovered)] -
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Dead)];
    }
    else {
        // Compute Susceptibles at t0 and m_forceofinfection at time t0-dt as initial values for discretization scheme.
        // Use m_forceofinfection at t0-dt to be consistent with further calculations of S (see compute_susceptibles()),
        // where also the value of m_forceofinfection for the previous timestep is used.
        update_forceofinfection(dt, true);
        if (m_forceofinfection > 1e-12) {
            m_initialization_method = 4;

            /* Attention: With an inappropriate combination of parameters and initial conditions, it can happen that S 
            becomes greater than N when this formula is used. In this case, at least one compartment is initialized 
            with a number less than zero, so that a log_error is thrown.
            However, this initialization method is consistent with the numerical solver of the model equations,
            so it may sometimes make sense to use this method. */
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Susceptible)] =
                m_transitions.get_last_value()[Eigen::Index(InfectionTransition::SusceptibleToExposed)] /
                (dt * m_forceofinfection);

            // Calculate other compartment sizes for t0.
            other_compartments_current_timestep(dt);

            // R; need an initial value for R, therefore do not calculate via compute_recovered().
            m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Recovered)] =
                m_N - m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Susceptible)] -
                m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Exposed)] -
                m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedNoSymptoms)] -
                m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSymptoms)] -
                m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedSevere)] -
                m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::InfectedCritical)] -
                m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Dead)];
        }
        else {
            m_initialization_method = -1;
            log_error("Error occured while initializing compartments: Force of infection is evaluated to 0 and neither "
                      "Susceptibles nor Recovered or total confirmed cases for time 0 were set. One of them should be "
                      "larger 0.");
        }
    }

    // Verify that the initialization produces an appropriate result.
    // Another check would be if the sum of the compartments is equal to N, but in all initialization methods one of the compartments is initialized via N - the others.
    // This also means that if a compartment is greater than N, we will always have one or more compartments less than zero.
    // Check if all compartments are non negative.
    for (int i = 0; i < (int)InfectionState::Count; i++) {
        if (m_populations[0][i] < 0) {
            m_initialization_method = -2;
            log_error("Initialization failed. One or more initial values for populations are less than zero. It may "
                      "help to use a different method for initialization.");
        }
    }

    // Compute m_forceofinfection at time t0 needed for further simulation.
    update_forceofinfection(dt);
}

void Model::compute_susceptibles(ScalarType dt)
{
    Eigen::Index num_time_points = m_populations.get_num_time_points();
    // Using number of Susceptibles from previous time step and force of infection from previous time step:
    // compute current number of Susceptibles and store Susceptibles in m_populations
    m_populations.get_last_value()[Eigen::Index(InfectionState::Susceptible)] =
        m_populations[num_time_points - 2][Eigen::Index(InfectionState::Susceptible)] / (1 + dt * m_forceofinfection);
}

void Model::compute_flow(Eigen::Index idx_InfectionTransitions, Eigen::Index idx_IncomingFlow, ScalarType dt,
                         Eigen::Index current_time_index)
{
    ScalarType sum = 0;
    /* In order to satisfy TransitionDistribution(dt*i) = 0 for all i >= k, k is determined by the maximum support of the distribution.
    Since we are using a backwards difference scheme to compute the derivative, we have that the
    derivative of TransitionDistribution(dt*i) = 0 for all i >= k+1.

    Hence calc_time_index goes until std::ceil(support_max/dt) since for std::ceil(support_max/dt)+1 all terms are already zero. 
    This needs to be adjusted if we are changing the finite difference scheme */

    Eigen::Index calc_time_index = (Eigen::Index)std::ceil(
        parameters.get<TransitionDistributions>()[idx_InfectionTransitions].get_support_max(dt, m_tol) / dt);

    for (Eigen::Index i = current_time_index - calc_time_index; i < current_time_index; i++) {
        // (current_time_index - i) * dt is the time the individuals have already spent in this state.
        ScalarType state_age = (current_time_index - i) * dt;

        // Backward difference scheme to approximate first derivative.
        sum += (parameters.get<TransitionDistributions>()[idx_InfectionTransitions].eval(state_age) -
                parameters.get<TransitionDistributions>()[idx_InfectionTransitions].eval(state_age - dt)) /
               dt * m_transitions[i + 1][idx_IncomingFlow];
    }

    m_transitions.get_value(current_time_index)[Eigen::Index(idx_InfectionTransitions)] =
        (-dt) * parameters.get<TransitionProbabilities>()[idx_InfectionTransitions] * sum;
}

void Model::compute_flow(Eigen::Index idx_InfectionTransitions, Eigen::Index idx_IncomingFlow, ScalarType dt)
{
    Eigen::Index current_time_index = m_transitions.get_num_time_points() - 1;
    compute_flow(idx_InfectionTransitions, idx_IncomingFlow, dt, current_time_index);
}

void Model::flows_current_timestep(ScalarType dt)
{
    // Calculate flow from Susceptibles to Exposed with force of infection from previous time step and Susceptibles from current time step.
    m_transitions.get_last_value()[Eigen::Index(InfectionTransition::SusceptibleToExposed)] =
        dt * m_forceofinfection * m_populations.get_last_value()[Eigen::Index(InfectionState::Susceptible)];
    // Calculate all other flows with compute_flow.
    // Flow from Exposed to InfectedNoSymptoms
    compute_flow(Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms),
                 Eigen::Index(InfectionTransition::SusceptibleToExposed), dt);
    // Flow from InfectedNoSymptoms to InfectedSymptoms
    compute_flow(Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms),
                 Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms), dt);
    // Flow from InfectedNoSymptoms to Recovered
    compute_flow(Eigen::Index(InfectionTransition::InfectedNoSymptomsToRecovered),
                 Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms), dt);
    // Flow from InfectedSymptoms to InfectedSevere
    compute_flow(Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere),
                 Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms), dt);
    // Flow from InfectedSymptoms to Recovered
    compute_flow(Eigen::Index(InfectionTransition::InfectedSymptomsToRecovered),
                 Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms), dt);
    // Flow from InfectedSevere to InfectedCritical
    compute_flow(Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical),
                 Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere), dt);
    // Flow from InfectedSevere to Recovered
    compute_flow(Eigen::Index(InfectionTransition::InfectedSevereToRecovered),
                 Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere), dt);
    // Flow from InfectedCritical to Dead
    compute_flow(Eigen::Index(InfectionTransition::InfectedCriticalToDead),
                 Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical), dt);
    // Flow from InfectedCritical to Recovered
    compute_flow(Eigen::Index(InfectionTransition::InfectedCriticalToRecovered),
                 Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical), dt);
}

void Model::compute_deaths()
{
    Eigen::Index num_time_points = m_populations.get_num_time_points();

    m_populations.get_last_value()[Eigen::Index(InfectionState::Dead)] =
        m_populations[num_time_points - 2][Eigen::Index(InfectionState::Dead)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedCriticalToDead)];
}

void Model::update_forceofinfection(ScalarType dt, bool initialization)
{
    m_forceofinfection = 0;

    // Determine the relevant calculation area = union of the supports of the relevant transition distributions.
    ScalarType calc_time = std::max(
        {parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToRecovered]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToInfectedSevere]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToRecovered]
             .get_support_max(dt, m_tol)});

    // Corresponding index.
    // Need calc_time_index timesteps in sum,
    // subtract 1 because in the last summand all TransitionDistributions evaluate to 0 (by definition of support_max).
    Eigen::Index calc_time_index = (Eigen::Index)std::ceil(calc_time / dt) - 1;

    Eigen::Index num_time_points;
    ScalarType current_time;
    ScalarType deaths;

    if (initialization) {
        // Determine m_forceofinfection at time t0-dt which is the penultimate timepoint in m_transitions.
        num_time_points = m_transitions.get_num_time_points() - 1;
        // Get time of penultimate timepoint in m_transitions.
        current_time = m_transitions.get_time(num_time_points - 1);
        // Determine the number of individuals in Dead compartment at time t0-dt.
        deaths = m_populations[Eigen::Index(0)][Eigen::Index(InfectionState::Dead)] -
                 m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedCriticalToDead)];
    }
    else {
        // Determine m_forceofinfection for current last time in m_transitions.
        num_time_points = m_transitions.get_num_time_points();
        current_time    = m_transitions.get_last_time();
        deaths          = m_populations.get_last_value()[Eigen::Index(InfectionState::Dead)];
    }

    for (Eigen::Index i = num_time_points - 1 - calc_time_index; i < num_time_points - 1; i++) {

        ScalarType state_age = (num_time_points - 1 - i) * dt;
        ScalarType season_val =
            1 +
            parameters.get<Seasonality>() *
                sin(3.141592653589793 * (std::fmod((parameters.get<StartDay>() + current_time), 365.0) / 182.5 + 0.5));
        m_forceofinfection +=
            season_val * parameters.get<TransmissionProbabilityOnContact>().eval(state_age) *
            parameters.get<ContactPatterns>().get_cont_freq_mat().get_matrix_at(current_time)(0, 0) *
            ((parameters
                      .get<TransitionProbabilities>()[(int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms] *
                  parameters
                      .get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms]
                      .eval(state_age) +
              parameters.get<TransitionProbabilities>()[(int)InfectionTransition::InfectedNoSymptomsToRecovered] *
                  parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToRecovered]
                      .eval(state_age)) *
                 m_transitions[i + 1][Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms)] *
                 parameters.get<RelativeTransmissionNoSymptoms>().eval(state_age) +
             (parameters.get<TransitionProbabilities>()[(int)InfectionTransition::InfectedSymptomsToInfectedSevere] *
                  parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToInfectedSevere]
                      .eval(state_age) +
              parameters.get<TransitionProbabilities>()[(int)InfectionTransition::InfectedSymptomsToRecovered] *
                  parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToRecovered].eval(
                      state_age)) *
                 m_transitions[i + 1][Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms)] *
                 parameters.get<RiskOfInfectionFromSymptomatic>().eval(state_age));
    }
    m_forceofinfection = 1 / (m_N - deaths) * m_forceofinfection;
}

void Model::compute_compartment(Eigen::Index idx_InfectionState, Eigen::Index idx_IncomingFlow,
                                Eigen::Index idx_TransitionDistribution1, Eigen::Index idx_TransitionDistribution2,
                                ScalarType dt)
{
    ScalarType sum = 0;

    // Determine relevant calculation area and corresponding index.
    ScalarType calc_time =
        std::max(parameters.get<TransitionDistributions>()[idx_TransitionDistribution1].get_support_max(dt, m_tol),
                 parameters.get<TransitionDistributions>()[idx_TransitionDistribution2].get_support_max(dt, m_tol));

    Eigen::Index calc_time_index = (Eigen::Index)std::ceil(calc_time / dt) - 1;

    Eigen::Index num_time_points = m_transitions.get_num_time_points();

    for (Eigen::Index i = num_time_points - 1 - calc_time_index; i < num_time_points - 1; i++) {

        ScalarType state_age = (num_time_points - 1 - i) * dt;

        sum += (parameters.get<TransitionProbabilities>()[idx_TransitionDistribution1] *
                    parameters.get<TransitionDistributions>()[idx_TransitionDistribution1].eval(state_age) +
                (1 - parameters.get<TransitionProbabilities>()[idx_TransitionDistribution1]) *
                    parameters.get<TransitionDistributions>()[idx_TransitionDistribution2].eval(state_age)) *
               m_transitions[i + 1][idx_IncomingFlow];
    }

    m_populations.get_last_value()[idx_InfectionState] = sum;
}

void Model::other_compartments_current_timestep(ScalarType dt)
{
    // Exposed
    // idx_TransitionDistribution2 is a dummy index as there is no transition from Exposed to Recovered in our model,
    // write any transition here as probability of going from Exposed to Recovered is 0.
    compute_compartment(Eigen::Index(InfectionState::Exposed), Eigen::Index(InfectionTransition::SusceptibleToExposed),
                        Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms), 0, dt);
    // InfectedNoSymptoms
    compute_compartment(Eigen::Index(InfectionState::InfectedNoSymptoms),
                        Eigen::Index(InfectionTransition::ExposedToInfectedNoSymptoms),
                        Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms),
                        Eigen::Index(InfectionTransition::InfectedNoSymptomsToRecovered), dt);
    // InfectedSymptoms
    compute_compartment(Eigen::Index(InfectionState::InfectedSymptoms),
                        Eigen::Index(InfectionTransition::InfectedNoSymptomsToInfectedSymptoms),
                        Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere),
                        Eigen::Index(InfectionTransition::InfectedSymptomsToRecovered), dt);
    // InfectedSevere
    compute_compartment(Eigen::Index(InfectionState::InfectedSevere),
                        Eigen::Index(InfectionTransition::InfectedSymptomsToInfectedSevere),
                        Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical),
                        Eigen::Index(InfectionTransition::InfectedSevereToRecovered), dt);
    // InfectedCritical
    compute_compartment(Eigen::Index(InfectionState::InfectedCritical),
                        Eigen::Index(InfectionTransition::InfectedSevereToInfectedCritical),
                        Eigen::Index(InfectionTransition::InfectedCriticalToDead),
                        Eigen::Index(InfectionTransition::InfectedCriticalToRecovered), dt);
}

void Model::compute_recovered()
{
    Eigen::Index num_time_points = m_populations.get_num_time_points();

    m_populations.get_last_value()[Eigen::Index(InfectionState::Recovered)] =
        m_populations[num_time_points - 2][Eigen::Index(InfectionState::Recovered)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedNoSymptomsToRecovered)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedSymptomsToRecovered)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedSevereToRecovered)] +
        m_transitions.get_last_value()[Eigen::Index(InfectionTransition::InfectedCriticalToRecovered)];
}

ScalarType Model::get_global_support_max(ScalarType dt) const
{
    return std::max(
        {parameters.get<TransitionDistributions>()[(int)InfectionTransition::ExposedToInfectedNoSymptoms]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToInfectedSymptoms]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedNoSymptomsToRecovered]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToInfectedSevere]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSymptomsToRecovered]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSevereToInfectedCritical]
             .get_support_max(dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedSevereToRecovered].get_support_max(
             dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedCriticalToDead].get_support_max(
             dt, m_tol),
         parameters.get<TransitionDistributions>()[(int)InfectionTransition::InfectedCriticalToRecovered]
             .get_support_max(dt, m_tol)});
}

} // namespace isecir
} // namespace mio

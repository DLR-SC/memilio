/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Anna Wendler, Lena Ploetzke
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
#include <iostream>

namespace mio
{
namespace isecir
{

Model::Model(TimeSeries<double>&& init, double dt_init, int N_init, Pa Parameterset_init)
    : parameters{Parameterset_init}
    , m_transitions{std::move(init)}
    , m_SECIR{TimeSeries<double>(8)}
    , m_dt{dt_init}
    , m_N{N_init}
{
}

// define helper functions later needed for simulation
void Model::update_susceptibles()
{
    // using number of susceptibles from previous time step and force of infection from previous time step: compute current number of susceptibles
    m_susceptibles = m_susceptibles / (1 + m_dt * m_forceofinfection);
}
void Model::update_forceofinfection()
{
    // determine force of infection for the current last timepoint in m_transitions
    m_forceofinfection = 0;
    int calc_time      = 0;
    // determine the relevant calculation area; just in the support of one of the transition distributions
    for (int i = 1; i < 5; i++) {
        std::cout << "xright: " << parameters.get<TransitionDistributions>()[i].get_xright() << "\n";
        // if (parameters.get<TransitionDistributions>()[i].get_xright()) {
        //     std::cout << "i" << i << "\n";
        //     //calc_time = parameters.get<TransitionDistributions>()[i].get_xright();
        // }
    }
    std::cout << calc_time << "\n";
    // //determine corresponding indices
    // //Achtung:Parameter müssten zum teil noch abhängig von tau und t sein, das ist in parameters aber noch nicht
    // calc_time = (int)std::ceil(calc_time / m_dt); //need calc_time timesteps in sum
    // for (int i = m_transitions.get_num_time_points() - calc_time; i < m_transitions.get_num_time_points() - 1; i++) {
    //     m_forceofinfection +=
    //         parameters.get<TransmissionProbabilityOnContact>() *
    //         parameters.get<ContactPatterns>().get_cont_freq_mat().get_matrix_at(m_transitions.get_last_time())(0, 0) *
    //         ((parameters.get<TransitionProbabilities>()[0] *
    //               parameters.get<TransitionDistributions>()[1].Distribution(
    //                   m_transitions.get_time(m_transitions.get_num_time_points() - 1 - i)) +
    //           (1 - parameters.get<TransitionProbabilities>()[0]) *
    //               parameters.get<TransitionDistributions>()[2].Distribution(
    //                   m_transitions.get_time(m_transitions.get_num_time_points() - 1 - i))) *
    //              m_transitions[i + 1][Eigen::Index(InfectionTransitions::ExposedToInfectedNoSymptoms)] *
    //              parameters.get<RelativeTransmissionNoSymptoms>() +
    //          (parameters.get<TransitionProbabilities>()[1] *
    //               parameters.get<TransitionDistributions>()[3].Distribution(
    //                   m_transitions.get_time(m_transitions.get_num_time_points() - 1 - i)) +
    //           (1 - parameters.get<TransitionProbabilities>()[1]) *
    //               parameters.get<TransitionDistributions>()[4].Distribution(
    //                   m_transitions.get_time(m_transitions.get_num_time_points() - 1 - i))) *
    //              m_transitions[i + 1][Eigen::Index(InfectionTransitions::InfectedNoSymptomsToInfectedSymptoms)] *
    //              parameters.get<RiskOfInfectionFromSymptomatic>());
    // }

    //Achtung:hier muss noch D(t_n) rein
    m_forceofinfection = m_dt / ((double)m_N) * m_forceofinfection;
}

// define function that defines general scheme to compute flow
void Model::compute_flow(int idx_InfectionTransitions, double TransitionProbability, int idx_TransitionDistribution)
{
    // allowed just for idx_InfectionTransitions>=1
    double sum = 0;
    for (int i =
             m_transitions.get_num_time_points() -
             (int)std::ceil(parameters.get<TransitionDistributions>()[idx_TransitionDistribution].get_xright() / m_dt);
         i < m_transitions.get_num_time_points() - 1; i++) {
        //forward difference scheme to get always tau>0
        sum += (parameters.get<TransitionDistributions>()[idx_TransitionDistribution].Distribution(
                    m_transitions.get_time(m_transitions.get_num_time_points() - i + 1)) -
                parameters.get<TransitionDistributions>()[idx_TransitionDistribution].Distribution(
                    m_transitions.get_time(m_transitions.get_num_time_points() - i))) /
               m_dt * m_transitions[i][Eigen::Index(idx_InfectionTransitions - 1)];
    }

    m_transitions[m_transitions.get_num_time_points()][Eigen::Index(idx_InfectionTransitions)] =
        (-1) * TransitionProbability * m_dt * sum;
}

void Model::compute_totaldeaths()
{
    double sum = 0;
    for (int i = 0; i < m_transitions.get_num_time_points() - 1; i++) {
        sum += m_transitions[i][Eigen::Index(5)];
    }

    m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(7)] = m_dt * sum;
}

// function that actually computes flows for remaining transitions
// TimeSeries<double> get_flows(int t_max)
// {}
// sigma_S^E, sigma_E^C and sigma_C^I are known from update_forceofinfection
// compute remaining flows here
//think we dont need this function

// define function that defines general scheme to compute compartments from flows

// fucntion that actually computes size of compartments from flows (all but S and R)
void Model::get_size_of_compartments(int idx_IncomingFlow, double TransitionProbability, int idx_TransitionDistribution,
                                     int idx_TransitionDistributionToRecov, int idx_InfectionState)
{
    double sum = 0;

    for (int i =
             m_transitions.get_num_time_points() -
             (int)std::ceil(parameters.get<TransitionDistributions>()[idx_TransitionDistribution].get_xright() / m_dt);
         i < m_transitions.get_num_time_points() - 1; i++) {
        sum +=
            (TransitionProbability * parameters.get<TransitionDistributions>()[idx_TransitionDistribution].Distribution(
                                         m_transitions.get_time(m_transitions.get_num_time_points() - i)) +
             (1 - TransitionProbability) *
                 parameters.get<TransitionDistributions>()[idx_TransitionDistributionToRecov].Distribution(
                     m_transitions.get_time(m_transitions.get_num_time_points() - i))) *
            m_transitions[i][Eigen::Index(idx_IncomingFlow)];
    }

    m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(idx_InfectionState)] = m_dt * sum;
}

// do simulation
void Model::simulate(int t_max)
{
    std::cout << t_max << "\n";
    // compute S(0) and phi(0) as initial values for discretization scheme
    update_forceofinfection();
    // m_susceptibles = m_transitions[m_transitions.get_num_time_points() - 1]
    //                               [Eigen::Index(InfectionTransitions::SusceptibleToExposed)] /
    //                  m_forceofinfection;

    // // for every time step:
    // while (m_transitions.get_last_time() < t_max) {
    //     m_transitions.add_time_point(m_transitions.get_last_time() + m_dt);
    //     Eigen::Index idx = m_transitions.get_num_time_points() - 1;
    //     // compute_S:
    //     // compute S(t_{idx}) and store it in m_susceptibles
    //     update_susceptibles();
    //     // compute_phi:
    //     // compute sigma_E^C(t_{n+1}) and store it
    //     compute_flow(1, 1, 0);
    //     // compute sigma_C^I(t_{n+1}) and store it
    //     compute_flow(2, parameters.get<TransitionProbabilities>()[0], 1);

    //     // with this: compute phi(t_{n+1})
    //     update_forceofinfection();
    //     //calculate sigma_S^E
    //     m_transitions[idx][Eigen::Index(InfectionTransitions::SusceptibleToExposed)] =
    //         m_forceofinfection * m_susceptibles;

    //     // compute remaining flows
    //     //sigma_I^H(t_{n+1})
    //     compute_flow(3, parameters.get<TransitionProbabilities>()[1], 3);
    //     //sigma_H^U(t_{n+1})
    //     compute_flow(4, parameters.get<TransitionProbabilities>()[2], 5);
    //     //sigma_U^D(t_{n+1})
    //     compute_flow(5, parameters.get<TransitionProbabilities>()[3], 7);

    //     // compute D
    //     compute_totaldeaths();
    // }

    // at the end of simulation:
    // compute remaining compartments from flows
    // E, use dummy transitiontorecov
    // get_size_of_compartments(1, parameters.get<TransitionProbabilities>()[0], 0, 0, 1);
    // // C
    // get_size_of_compartments(2, parameters.get<TransitionProbabilities>()[1], 1, 2, 2);
    // // I
    // get_size_of_compartments(3, parameters.get<TransitionProbabilities>()[2], 3, 4, 3);
    // // H
    // get_size_of_compartments(4, parameters.get<TransitionProbabilities>()[3], 5, 6, 4);
    // // U
    // get_size_of_compartments(5, parameters.get<TransitionProbabilities>()[4], 7, 8, 5);
    // // R
    // m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(6)] = m_N -
    //                                                           m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(0)] -
    //                                                           m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(1)] -
    //                                                           m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(2)] -
    //                                                           m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(3)] -
    //                                                           m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(4)] -
    //                                                           m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(5)] -
    //                                                           m_SECIR[m_SECIR.get_num_time_points()][Eigen::Index(7)];
}

} // namespace isecir
} // namespace mio

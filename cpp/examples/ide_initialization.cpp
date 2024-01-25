/*
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Anna Wendler
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
#include "ide_secir/infection_state.h"
#include "ide_secir/parameters.h"
#include "ide_secir/simulation.h"
#include "ide_secir/initialflows.h"
#include "memilio/config.h"
#include "memilio/math/eigen.h"
#include "memilio/utils/time_series.h"
// #include "memilio/epidemiology/uncertain_matrix.h"
#include <iostream>

int main()
{
    using Vec = mio::TimeSeries<ScalarType>::Vector;

    // ScalarType tmax        = 10;
    ScalarType N           = 10000;
    ScalarType Dead_before = 12;
    ScalarType dt          = 1;

    int num_transitions = (int)mio::isecir::InfectionTransition::Count;

    // create TimeSeries with num_transitions elements where transitions needed for simulation will be stored
    mio::TimeSeries<ScalarType> init(num_transitions);

    // add time points for initialization of transitions
    Vec vec_init(num_transitions);
    // Add dummy time point so that model initialization works
    // TODO: check if it is possible to initialize with an empty time series for init_transitions
    // TODO: check if there is an easier/cleaner way to initialize
    init.add_time_point(0, vec_init);

    // // create TimeSeries with num_transitions elements where transitions needed for simulation will be stored
    // mio::TimeSeries<ScalarType> init(num_transitions);

    // // add time points for initialization of transitions
    // Vec vec_init(num_transitions);
    // vec_init[(int)mio::isecir::InfectionTransition::SusceptibleToExposed]                 = 25.0;
    // vec_init[(int)mio::isecir::InfectionTransition::ExposedToInfectedNoSymptoms]          = 15.0;
    // vec_init[(int)mio::isecir::InfectionTransition::InfectedNoSymptomsToInfectedSymptoms] = 8.0;
    // vec_init[(int)mio::isecir::InfectionTransition::InfectedNoSymptomsToRecovered]        = 4.0;
    // vec_init[(int)mio::isecir::InfectionTransition::InfectedSymptomsToInfectedSevere]     = 1.0;
    // vec_init[(int)mio::isecir::InfectionTransition::InfectedSymptomsToRecovered]          = 4.0;
    // vec_init[(int)mio::isecir::InfectionTransition::InfectedSevereToInfectedCritical]     = 1.0;
    // vec_init[(int)mio::isecir::InfectionTransition::InfectedSevereToRecovered]            = 1.0;
    // vec_init[(int)mio::isecir::InfectionTransition::InfectedCriticalToDead]               = 1.0;
    // vec_init[(int)mio::isecir::InfectionTransition::InfectedCriticalToRecovered]          = 1.0;
    // // add initial time point to time series
    // init.add_time_point(-10, vec_init);
    // // add further time points until time 0
    // while (init.get_last_time() < 0) {
    //     vec_init *= 1.01;
    //     init.add_time_point(init.get_last_time() + dt, vec_init);
    // }

    // Initialize model.
    mio::isecir::Model model(std::move(init), N, Dead_before);

    // // Set working parameters
    // mio::SmootherCosine smoothcos(2.0);
    // mio::StateAgeFunctionWrapper delaydistribution(smoothcos);
    // std::vector<mio::StateAgeFunctionWrapper> vec_delaydistrib(num_transitions, delaydistribution);
    // vec_delaydistrib[(int)mio::isecir::InfectionTransition::SusceptibleToExposed].set_parameter(3.0);
    // vec_delaydistrib[(int)mio::isecir::InfectionTransition::InfectedNoSymptomsToInfectedSymptoms].set_parameter(4.0);
    // model.parameters.set<mio::isecir::TransitionDistributions>(vec_delaydistrib);

    // std::vector<ScalarType> vec_prob((int)mio::isecir::InfectionTransition::Count, 0.5);
    // vec_prob[Eigen::Index(mio::isecir::InfectionTransition::SusceptibleToExposed)]        = 1;
    // vec_prob[Eigen::Index(mio::isecir::InfectionTransition::ExposedToInfectedNoSymptoms)] = 1;
    // model.parameters.set<mio::isecir::TransitionProbabilities>(vec_prob);

    // mio::ContactMatrixGroup contact_matrix               = mio::ContactMatrixGroup(1, 1);
    // contact_matrix[0]                                    = mio::ContactMatrix(Eigen::MatrixXd::Constant(1, 1, 10.));
    // model.parameters.get<mio::isecir::ContactPatterns>() = mio::UncertainContactMatrix(contact_matrix);

    // mio::ExponentialDecay expdecay(0.5);
    // mio::StateAgeFunctionWrapper prob(expdecay);
    // model.parameters.set<mio::isecir::TransmissionProbabilityOnContact>(prob);
    // model.parameters.set<mio::isecir::RelativeTransmissionNoSymptoms>(prob);
    // model.parameters.set<mio::isecir::RiskOfInfectionFromSymptomatic>(prob);

    // model.check_constraints(dt);
    ScalarType rki_dummy{1.};
    mio::isecir::set_initial_flows(model, dt, rki_dummy);

    // Carry out simulation.
    mio::isecir::Simulation sim(model, 0, dt);

    sim.print_transitions();

    // sim.print_compartments();

    std::cout << "Done. \n";
}

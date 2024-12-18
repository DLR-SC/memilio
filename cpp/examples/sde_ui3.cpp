/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Nils Wassmuth, Rene Schmieding, Martin J. Kuehn
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

#include "memilio/utils/logging.h"
#include "sde_ui3/model.h"
#include "sde_ui3/simulation.h"

int main()
{
    mio::set_log_level(mio::LogLevel::debug);

    double t0   = 0.;
    double tmax = 5.;
    double dt   = 0.1;

    double total_population = 10000;

    mio::log_info("Simulating SIR; t={} ... {} with dt = {}.", t0, tmax, dt);

    mio::sui3::Model model;

    model.populations[{mio::Index<mio::sui3::InfectionState>(mio::sui3::InfectionState::Infected_V1I0)}]  = 100;
    model.populations[{mio::Index<mio::sui3::InfectionState>(mio::sui3::InfectionState::Uninfected_I0)}] = total_population - model.populations[{mio::Index<mio::sui3::InfectionState>(mio::sui3::InfectionState::Infected_V1I0)}];

    model.parameters.set<mio::sui3::TimeInfectedV1>(10);
    model.parameters.set<mio::sui3::TimeInfectedV2>(10);
    model.parameters.set<mio::sui3::TimeInfectedV3>(10);
    model.parameters.set<mio::sui3::TransmissionProbabilityOnContactV1>(0.2);
    model.parameters.set<mio::sui3::TransmissionProbabilityOnContactV2>(0.2);
    model.parameters.set<mio::sui3::TransmissionProbabilityOnContactV3>(0.2);
    model.parameters.set<mio::sui3::ProbMutation>(0.01);
    model.parameters.get<mio::sui3::ContactPatterns>().get_baseline()(0, 0) = 1;
    //model.parameters.get<mio::sui3::ContactPatterns>().add_damping(0.6, mio::SimulationTime(12.5));

    model.check_constraints();

    auto sir = mio::sui3::simulate(t0, tmax, dt, model);

    sir.print_table();
}

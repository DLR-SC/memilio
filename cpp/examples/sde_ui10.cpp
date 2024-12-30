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
#include "sde_ui10/model.h"
#include "sde_ui10/simulation.h"

int main()
{
    mio::set_log_level(mio::LogLevel::debug);

    double t0   = 0.;
    double tmax = 500.;
    double dt   = 0.1;

    double total_population = 10000;

    mio::log_info("Simulating SIR; t={} ... {} with dt = {}.", t0, tmax, dt);

    mio::sui3::Model model;

    model.populations[{mio::Index<mio::sui10::InfectionState>(mio::sui10::InfectionState::Infected_V1I0)}]  = 100;
    model.populations[{mio::Index<mio::sui10::InfectionState>(mio::sui10::InfectionState::Uninfected_I0)}] = 
        total_population - model.populations[{mio::Index<mio::sui10::InfectionState>(mio::sui10::InfectionState::Infected_V1I0)}];


    model.parameters.set<mio::sui10::TimeInfectedV1>(10);
    model.parameters.set<mio::sui10::TimeInfectedV2>(10);
    model.parameters.set<mio::sui10::TimeInfectedV3>(10);
    model.parameters.set<mio::sui10::TimeInfectedV4>(10);
    model.parameters.set<mio::sui10::TimeInfectedV5>(10);
    model.parameters.set<mio::sui10::TimeInfectedV6>(10);
    model.parameters.set<mio::sui10::TimeInfectedV7>(10);
    model.parameters.set<mio::sui10::TimeInfectedV8>(10);
    model.parameters.set<mio::sui10::TimeInfectedV9>(10);
    model.parameters.set<mio::sui10::TimeInfectedV10>(10);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV1>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV2>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV3>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV4>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV5>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV6>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV7>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV8>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV9>(0.2);
    model.parameters.set<mio::sui10::TransmissionProbabilityOnContactV10>(0.2);
    model.parameters.set<mio::sui10::ProbMutation>(0.01);
    model.parameters.get<mio::sui10::ContactPatterns>().get_baseline()(0, 0) = 1;
    //model.parameters.get<mio::sui3::ContactPatterns>().add_damping(0.6, mio::SimulationTime(12.5));

    model.check_constraints();

    auto sui3 = mio::sui3::simulate_flows(t0, tmax, dt, model);

    //sui3[0].print_table({"Susceptible", "Infected", "Recovered"});
    //sui3[1].print_table({"S-I", "I-R", "R-S"});

    auto integrator = std::make_shared<mio::EulerIntegratorCore<double>>();
    mio::FlowSimStoc<double, mio::sui10::Model> sim(model, t0, dt);
    sim.set_integrator(integrator);
    sim.advance(tmax);
    std::ofstream textfile;
    textfile.open("Test.txt");
    sim.get_result().print_table({}, 16, 5 , textfile);
    //sim.get_flows().print_table();
}

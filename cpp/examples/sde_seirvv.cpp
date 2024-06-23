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
#include <fstream>
#include <vector>
#include <iostream>

#include "memilio/utils/logging.h"
#include "sde_seirvv/model.h"
#include "sde_seirvv/simulation.h"
#include "memilio/utils/random_number_generator.h"

int main()
{
    mio::set_log_level(mio::LogLevel::debug);

    double t0   = 0.;
    double tmid = 100.;
    double tmax = 400.;
    double dt   = 0.1;

    mio::log_info("Simulating SEIRVV; t={} ... {} with dt = {}.", t0, tmax, dt);

    mio::sseirvv::Model model;

    /// Model Parameters are taken from 
    /// It is assumed that both variants have the same transmission probability 
    /// on contact and the same time exposed. The time infected is scaled by
    /// 1.35 for the second variant.
    
    double total_population = 180000;

    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV1)}]  = 0;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV2)}]  = 0;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV1)}]  = 7200;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV2)}]  = 0;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV1)}] = 0;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV2)}] = 0;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV1V2)}]  = 0;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV1V2)}]  = 0;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV1V2)}] = 0;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::Susceptible)}] =
        total_population -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV1)}] -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV2)}] -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV1)}] -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV2)}] -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV1)}] -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV2)}] -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV1V2)}] -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV1V2)}] -
        model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV1V2)}];

    model.parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV1>(0.076);
    model.parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV2>(0.076);
    model.parameters.set<mio::sseirvv::TimeExposedV1>(5.33);
    model.parameters.set<mio::sseirvv::TimeExposedV2>(5.33);      
    model.parameters.set<mio::sseirvv::TimeInfectedV1>(17.2);
    model.parameters.set<mio::sseirvv::TimeInfectedV2>(17.2 * 1.35);

    model.check_constraints();

    /// The second variant enters with 100 individuals at some point tmid. The model does not fix
    /// the increase of totalpopulation caused by this injection
    auto ssirs = mio::sseirvv::simulate(t0, tmid, dt, model);
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::Susceptible)}] = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[0];
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV1)}] = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[1];
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV1)}]  = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[2];
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV1)}] = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[3];    
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV2)}]  = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[4];
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV2)}]  = 100;
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV2)}] = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[6];
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::ExposedV1V2)}]  = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[7];
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::InfectedV1V2)}]  = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[8];
    model.populations[{mio::Index<mio::sseirvv::InfectionState>(mio::sseirvv::InfectionState::RecoveredV1V2)}] = ssirs.get_value(static_cast<size_t>(ssirs.get_num_time_points()) - 1)[9];
    auto ssirs2 = mio::sseirvv::simulate(tmid, tmax, dt, model);

    ssirs.print_table({"Susceptible", "ExposedV1", "InfectedV1", "RecoveredV1", "ExposedV2", "InfectedV2", "RecoveredV2", "ExposedV1V2", "InfectedV1V2", "RecoveredV1V2"});
}

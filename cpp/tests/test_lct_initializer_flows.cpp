/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Lena Ploetzke
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

#include "lct_secir/infection_state.h"
#include "lct_secir/initializer_flows.h"
#include "memilio/config.h"
#include "memilio/utils/time_series.h"
#include "memilio/epidemiology/uncertain_matrix.h"
#include "memilio/utils/compiler_diagnostics.h"
#include "memilio/math/eigen.h"

#include <vector>
#include <gtest/gtest.h>

// Test compares a calculation of an initial vector using data for flows with a previous result.
TEST(TestInitializer, compareWithPrevious)
{
    ScalarType dt = 0.5;

    using Model    = mio::lsecir::Model<2, 3, 2, 3, 2>;
    using LctState = Model::LctState;

    // Previous result.
    Eigen::VectorXd compare(LctState::Count);
    compare << 82810889.00545, 850.70432, 970.04980, 315.32890, 391.51799, 391.39351, 565.45854, 580.79267, 85.97421,
        86.02738, 80.26791, 189.53449, 167.57963, 329757.36512, 9710;

    // Initialize a model.
    Model model(std::move(Eigen::VectorXd::Zero(LctState::Count)));

    // Define parameters.
    model.parameters.get<mio::lsecir::TimeExposed>()                      = 3.1;
    model.parameters.get<mio::lsecir::TimeInfectedNoSymptoms>()           = 3.1;
    model.parameters.get<mio::lsecir::TimeInfectedSymptoms>()             = 6.1;
    model.parameters.get<mio::lsecir::TimeInfectedSevere>()               = 11.1;
    model.parameters.get<mio::lsecir::TimeInfectedCritical>()             = 17.1;
    model.parameters.get<mio::lsecir::TransmissionProbabilityOnContact>() = 0.01;
    mio::ContactMatrixGroup contact_matrix                                = mio::ContactMatrixGroup(1, 1);
    model.parameters.get<mio::lsecir::ContactPatterns>()                  = mio::UncertainContactMatrix(contact_matrix);

    model.parameters.get<mio::lsecir::RelativeTransmissionNoSymptoms>() = 1;
    model.parameters.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() = 1;
    model.parameters.get<mio::lsecir::Seasonality>()                    = 0;
    model.parameters.get<mio::lsecir::StartDay>()                       = 0;
    model.parameters.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 0.1;
    model.parameters.get<mio::lsecir::SeverePerInfectedSymptoms>()      = 0.1;
    model.parameters.get<mio::lsecir::CriticalPerSevere>()              = 0.1;
    model.parameters.get<mio::lsecir::DeathsPerCritical>()              = 0.1;

    ScalarType total_confirmed_cases = 341223;
    ScalarType deaths                = 9710;
    ScalarType total_population      = 83155031.0;

    // Add time points for initialization of transitions.
    mio::TimeSeries<ScalarType> init((int)mio::lsecir::InfectionTransition::Count);
    mio::TimeSeries<ScalarType>::Vector vec_init((int)mio::lsecir::InfectionTransition::Count);
    vec_init[(int)mio::lsecir::InfectionTransition::SusceptibleToExposed]                 = 25.0;
    vec_init[(int)mio::lsecir::InfectionTransition::ExposedToInfectedNoSymptoms]          = 15.0;
    vec_init[(int)mio::lsecir::InfectionTransition::InfectedNoSymptomsToInfectedSymptoms] = 8.0;
    vec_init[(int)mio::lsecir::InfectionTransition::InfectedNoSymptomsToRecovered]        = 4.0;
    vec_init[(int)mio::lsecir::InfectionTransition::InfectedSymptomsToInfectedSevere]     = 1.0;
    vec_init[(int)mio::lsecir::InfectionTransition::InfectedSymptomsToRecovered]          = 4.0;
    vec_init[(int)mio::lsecir::InfectionTransition::InfectedSevereToInfectedCritical]     = 1.0;
    vec_init[(int)mio::lsecir::InfectionTransition::InfectedSevereToRecovered]            = 1.0;
    vec_init[(int)mio::lsecir::InfectionTransition::InfectedCriticalToDead]               = 1.0;
    vec_init[(int)mio::lsecir::InfectionTransition::InfectedCriticalToRecovered]          = 1.0;
    // Add initial time point to time series.
    init.add_time_point(-130, vec_init);
    // Add further time points until time 0.
    while (init.get_last_time() < 0) {
        vec_init *= 1.01;
        init.add_time_point(init.get_last_time() + dt, vec_init);
    }

    // Calculate initial vector and compare with previous reult.
    mio::lsecir::Initializer<Model> initializer(std::move(init), model);
    initializer.set_tol_for_support_max(1e-6);
    initializer.compute_initialization_vector(total_population, deaths, total_confirmed_cases);

    for (int i = 0; i < (int)LctState::Count; i++) {
        EXPECT_NEAR(model.get_initial_values()[i], compare[i], 1e-4) << "at subcompartment number " << i;
    }
}

// Check if the constraints of the initializer are validated as expected.
TEST(TestInitializer, testConstraints)
{
    // Deactivate temporarily log output for next tests.
    mio::set_log_level(mio::LogLevel::off);

    ScalarType dt                    = 0.5;
    ScalarType total_confirmed_cases = 341223;
    ScalarType deaths                = 9710;
    ScalarType total_population      = 83155031.0;

    using Model    = mio::lsecir::Model<2, 3, 2, 3, 2>;
    using LctState = Model::LctState;

    // Initialize a model.
    Model model(std::move(Eigen::VectorXd::Zero(LctState::Count)));

    // Check wrong size of initial flows.
    mio::TimeSeries<ScalarType> init_wrong_size((int)mio::lsecir::InfectionTransition::Count - 1);
    Eigen::VectorXd vec_wrong_size = Eigen::VectorXd::Ones((int)mio::lsecir::InfectionTransition::Count - 1);
    init_wrong_size.add_time_point(-50, vec_wrong_size);
    while (init_wrong_size.get_last_time() < 0) {
        init_wrong_size.add_time_point(init_wrong_size.get_last_time() + dt, vec_wrong_size);
    }

    mio::lsecir::Initializer<Model> initializer_init_wrong_size(std::move(init_wrong_size), model);
    bool status =
        initializer_init_wrong_size.compute_initialization_vector(total_population, deaths, total_confirmed_cases);

    EXPECT_TRUE(status);

    // Check if last time of initial flows is not zero.
    mio::TimeSeries<ScalarType> init_wrong((int)mio::lsecir::InfectionTransition::Count);
    Eigen::VectorXd vec_init = Eigen::VectorXd::Ones((int)mio::lsecir::InfectionTransition::Count);
    init_wrong.add_time_point(-50, vec_init);
    while (init_wrong.get_last_time() < -5) {
        init_wrong.add_time_point(init_wrong.get_last_time() + dt, vec_init);
    }

    mio::TimeSeries<ScalarType> init_copy(init_wrong);
    mio::lsecir::Initializer<Model> initializer_init_wrong_last_time(std::move(init_copy), model);

    status =
        initializer_init_wrong_last_time.compute_initialization_vector(total_population, deaths, total_confirmed_cases);
    EXPECT_TRUE(status);

    // Check if time steps of initial flows are not equidistant.
    init_wrong.add_time_point(init_wrong.get_last_time() + 2. * dt, vec_init);
    while (init_wrong.get_last_time() < 0) {
        init_wrong.add_time_point(init_wrong.get_last_time() + dt, vec_init);
    }

    mio::lsecir::Initializer<Model> initializer_init_wrong_equidistant(std::move(init_wrong), model);

    status = initializer_init_wrong_equidistant.compute_initialization_vector(total_population, deaths,
                                                                              total_confirmed_cases);
    EXPECT_TRUE(status);

    // Check large step size.
    mio::TimeSeries<ScalarType> init_wrong_step((int)mio::lsecir::InfectionTransition::Count);
    init_wrong_step.add_time_point(-50, vec_init);
    init_wrong_step.add_time_point(init_wrong_step.get_last_time() + 10 * dt, vec_init);
    while (init_wrong_step.get_last_time() < 0) {
        init_wrong_step.add_time_point(init_wrong_step.get_last_time() + 10 * dt, vec_init);
    }

    mio::lsecir::Initializer<Model> initializer_init_wrong_step(std::move(init_wrong_step), model);

    status = initializer_init_wrong_step.compute_initialization_vector(total_population, deaths, total_confirmed_cases);
    EXPECT_TRUE(status);

    // Check with too short time period of initial data.
    mio::TimeSeries<ScalarType> init_short((int)mio::lsecir::InfectionTransition::Count);
    init_short.add_time_point(-1., vec_init);
    while (init_short.get_last_time() < 0) {
        init_short.add_time_point(init_short.get_last_time() + dt, vec_init);
    }

    mio::lsecir::Initializer<Model> initializer_init_short(std::move(init_short), model);
    status = initializer_init_short.compute_initialization_vector(total_population, deaths, total_confirmed_cases);
    EXPECT_TRUE(status);

    // Check with correct flows.
    mio::TimeSeries<ScalarType> init_right((int)mio::lsecir::InfectionTransition::Count);
    init_right.add_time_point(-50, vec_init);
    while (init_right.get_last_time() < 0) {
        init_right.add_time_point(init_right.get_last_time() + dt, vec_init);
    }

    mio::lsecir::Initializer<Model> initializer_right(std::move(init_right), model);
    initializer_right.set_tol_for_support_max(1e-6);

    status = initializer_right.compute_initialization_vector(total_population, deaths, total_confirmed_cases);
    EXPECT_FALSE(status);

    // Reactive log output.
    mio::set_log_level(mio::LogLevel::warn);
}
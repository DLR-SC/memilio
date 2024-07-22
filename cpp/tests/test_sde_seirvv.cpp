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
#include "abm_helpers.h"
#include "sde_seirvv/model.h"
#include "sde_seirvv/simulation.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

const mio::sseirvv::Model& sseirvv_testing_model()
{
    static mio::sseirvv::Model model;
    model.step_size = 1. / 29;
    model.populations.array().setConstant(1);
    { // set parameters s.t. coeffStoI is 1
        model.parameters.set<mio::sseirvv::TimeExposedV1>(1);
        model.parameters.set<mio::sseirvv::TimeExposedV2>(1./4);
        model.parameters.set<mio::sseirvv::TimeInfectedV1>(1);
        model.parameters.set<mio::sseirvv::TimeInfectedV2>(1./9);
        model.parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV1>(1);
        model.parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV2>(1);
        model.parameters.get<mio::sseirvv::ContactPatterns>().get_baseline()(0, 0) = 10;
    }
    return model;
}

TEST(TestSdeSeirvv, get_flows)
{
    // make two get_flows computations with mocked rng
    ScopedMockDistribution<
        testing::StrictMock<MockDistribution<mio::DistributionAdapter<std::normal_distribution<double>>>>>
        normal_dist_mock;

    EXPECT_CALL(normal_dist_mock.get_mock(), invoke)
        .Times(testing::Exactly(18))
        .WillOnce(testing::Return(1.))
        .WillOnce(testing::Return(0.))
        .WillOnce(testing::Return(0.))
        .WillOnce(testing::Return(1.))
        .WillOnce(testing::Return(1.))
        .WillOnce(testing::Return(0.))
        .WillOnce(testing::Return(0.))
        .WillOnce(testing::Return(1.))
        .WillOnce(testing::Return(0.)) //end first mock rng
        .WillOnce(testing::Return(0.))
        .WillOnce(testing::Return(1.))
        .WillOnce(testing::Return(1.))
        .WillOnce(testing::Return(0.))
        .WillOnce(testing::Return(0.))
        .WillOnce(testing::Return(1.))
        .WillOnce(testing::Return(1.))
        .WillOnce(testing::Return(0.))
        .WillOnce(testing::Return(1.));

    // Non-constant y for a more meaningful test
    Eigen::VectorXd y = Eigen::VectorXd(10);
    y << 3,1,2,4,1,1,1,4,4,4;
    Eigen::VectorXd flows   = Eigen::VectorXd::Constant(9, 1);

    // results contain two parts : deterministic + stochastic

    sseirvv_testing_model().get_flows(y, y, 0, flows);
    auto expected_result = Eigen::VectorXd(9);
    expected_result << 6 + sqrt(6) * sqrt(29), 15 + 0, 1 + 0, 4 + sqrt(4) * sqrt(29), 2 + sqrt(2) * sqrt(29), 
        9 + 0, 20 + 0, 16 + sqrt(16) * sqrt(29), 36 + 0;
    EXPECT_EQ(flows, expected_result);

    sseirvv_testing_model().get_flows(y, y, 0, flows);
    expected_result = Eigen::VectorXd(9);
    expected_result << 6 + 0, 15 + sqrt(15) * sqrt(29), 1 + sqrt(1) * sqrt(29), 4 + 0, 2 + 0, 9 + sqrt(9) * sqrt(29), 
        20 + sqrt(20) * sqrt(29), 16 + 0, 36 + sqrt(36) * sqrt(29);
    EXPECT_EQ(flows, expected_result);
}

TEST(TestSdeSeirvv, Simulation)
{
    // make a single integration step via a simulation
    // this should overwrite the model step size
    ScopedMockDistribution<
        testing::StrictMock<MockDistribution<mio::DistributionAdapter<std::normal_distribution<double>>>>>
        normal_dist_mock;

    EXPECT_CALL(normal_dist_mock.get_mock(), invoke)
        .Times(testing::Exactly(18))
        // 9 calls for each advance, as each call get_derivatives exactly once
        .WillRepeatedly(testing::Return(.0));

    auto sim = mio::sseirvv::Simulation(sseirvv_testing_model(), 0, 1);
    sim.advance(1);

    EXPECT_EQ(sim.get_model().step_size, 1.0); // set by simulation

    EXPECT_EQ(sim.get_result().get_num_time_points(), 2); // stores initial value and single step

    auto expected_result = Eigen::VectorXd(10);
    expected_result << 0,0.5,1,1,0.5,1,2,1,1,2;
    EXPECT_EQ(sim.get_result().get_last_value(), expected_result);

    sim.advance(1.5);
    EXPECT_EQ(sim.get_model().step_size, 0.5); // set by simulation
}

TEST(TestSdeSeirvv, FlowSimulation)
{
    // make a single integration step via a flow simulation
    // this should overwrite the model step size
    ScopedMockDistribution<
        testing::StrictMock<MockDistribution<mio::DistributionAdapter<std::normal_distribution<double>>>>>
        normal_dist_mock;

    EXPECT_CALL(normal_dist_mock.get_mock(), invoke)
        .Times(testing::Exactly(18))
        // 3 calls for each advance, as each call get_derivatives exactly once
        .WillRepeatedly(testing::Return(.5));

    auto sim = mio::sseirvv::FlowSimulation(sseirvv_testing_model(), 0, 1);
    sim.advance(1);

    EXPECT_EQ(sim.get_model().step_size, 1.0); // set by simulation

    EXPECT_EQ(sim.get_result().get_num_time_points(), 2); // stores initial value and single step

    auto expected_result = Eigen::VectorXd(10);
    expected_result << 0,0.5,1,1,0.5,1,2,1,1,2;
    EXPECT_EQ(sim.get_result().get_last_value(), expected_result);

    auto expected_flows = Eigen::VectorXd(9);
    expected_flows << 0.5,0.5,1,1,1,1,1,1,1;
    EXPECT_EQ(sim.get_flows().get_last_value(), expected_flows);

    sim.advance(1.5);
    EXPECT_EQ(sim.get_model().step_size, 0.5); // set by simulation
}

TEST(TestSdeSeirvv, check_constraints_parameters)
{
    mio::sseirvv::Model::ParameterSet parameters;
    parameters.set<mio::sseirvv::TimeInfectedV1>(6);
    parameters.set<mio::sseirvv::TimeInfectedV2>(6);
    parameters.set<mio::sseirvv::TimeExposedV1>(6);
    parameters.set<mio::sseirvv::TimeExposedV2>(6);
    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV1>(0.04);
    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV2>(0.04);
    parameters.get<mio::sseirvv::ContactPatterns>().get_baseline()(0, 0) = 10;

    // model.check_constraints() combines the functions from population and parameters.
    // We only want to test the functions for the parameters defined in parameters.h
    EXPECT_EQ(parameters.check_constraints(), 0);

    mio::set_log_level(mio::LogLevel::off);

    parameters.set<mio::sseirvv::TimeInfectedV1>(0);
    EXPECT_EQ(parameters.check_constraints(), 1);

    parameters.set<mio::sseirvv::TimeInfectedV1>(6);
    parameters.set<mio::sseirvv::TimeInfectedV2>(0);
    EXPECT_EQ(parameters.check_constraints(), 1);

    parameters.set<mio::sseirvv::TimeInfectedV2>(6);
    parameters.set<mio::sseirvv::TimeExposedV1>(0); 
    EXPECT_EQ(parameters.check_constraints(), 1);

    parameters.set<mio::sseirvv::TimeExposedV1>(6);
    parameters.set<mio::sseirvv::TimeExposedV2>(0);
    EXPECT_EQ(parameters.check_constraints(), 1);

    parameters.set<mio::sseirvv::TimeExposedV2>(6);
    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV1>(10.);
    EXPECT_EQ(parameters.check_constraints(), 1);

    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV1>(0.04);
    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV2>(10.);
    EXPECT_EQ(parameters.check_constraints(), 1);
    mio::set_log_level(mio::LogLevel::warn);
}

TEST(TestSdeSeirvv, apply_constraints_parameters)
{
    const ScalarType tol_times = 1e-1;
    mio::sseirvv::Model::ParameterSet parameters;
    parameters.set<mio::sseirvv::TimeInfectedV1>(6);
    parameters.set<mio::sseirvv::TimeInfectedV2>(6);
    parameters.set<mio::sseirvv::TimeExposedV1>(6);
    parameters.set<mio::sseirvv::TimeExposedV2>(6);
    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV1>(0.04);
    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV2>(0.04);
    parameters.get<mio::sseirvv::ContactPatterns>().get_baseline()(0, 0) = 10;

    EXPECT_EQ(parameters.apply_constraints(), 0);

    mio::set_log_level(mio::LogLevel::off);

    parameters.set<mio::sseirvv::TimeInfectedV1>(-2.5);
    EXPECT_EQ(parameters.apply_constraints(), 1);
    EXPECT_EQ(parameters.get<mio::sseirvv::TimeInfectedV1>(), tol_times);

    parameters.set<mio::sseirvv::TimeInfectedV2>(-2.5);
    EXPECT_EQ(parameters.apply_constraints(), 1);
    EXPECT_EQ(parameters.get<mio::sseirvv::TimeInfectedV2>(), tol_times);

    parameters.set<mio::sseirvv::TimeExposedV1>(-2.5);
    EXPECT_EQ(parameters.apply_constraints(), 1);
    EXPECT_EQ(parameters.get<mio::sseirvv::TimeExposedV1>(), tol_times);

    parameters.set<mio::sseirvv::TimeExposedV2>(-2.5);
    EXPECT_EQ(parameters.apply_constraints(), 1);
    EXPECT_EQ(parameters.get<mio::sseirvv::TimeExposedV2>(), tol_times);

    parameters.set<mio::sseirvv::TimeExposedV2>(-2.5);
    EXPECT_EQ(parameters.apply_constraints(), 1);
    EXPECT_EQ(parameters.get<mio::sseirvv::TimeExposedV2>(), tol_times);

    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV1>(10.);
    EXPECT_EQ(parameters.apply_constraints(), 1);
    EXPECT_NEAR(parameters.get<mio::sseirvv::TransmissionProbabilityOnContactV1>(), 0.0, 1e-14);

    parameters.set<mio::sseirvv::TransmissionProbabilityOnContactV2>(10.);
    EXPECT_EQ(parameters.apply_constraints(), 1);
    EXPECT_NEAR(parameters.get<mio::sseirvv::TransmissionProbabilityOnContactV2>(), 0.0, 1e-14);
    mio::set_log_level(mio::LogLevel::warn);
}

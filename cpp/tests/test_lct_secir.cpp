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

#include "lct_secir/model.h"
#include "lct_secir/infection_state.h"
#include "lct_secir/parameters.h"
#include "ode_secir/model.h"
#include "memilio/config.h"
#include "memilio/utils/time_series.h"
#include "memilio/utils/logging.h"
#include "memilio/epidemiology/uncertain_matrix.h"
#include "memilio/math/eigen.h"
#include "memilio/compartments/simulation.h"
#include "load_test_data.h"

#include <gtest/gtest.h>
#include "boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp"

// Test confirms that default construction of an LCT model works.
TEST(TestLCTSecir, simulateDefault)
{
    using Model     = mio::lsecir::Model<1, 1, 1, 1, 1>;
    using LctState  = Model::LctState;
    ScalarType t0   = 0;
    ScalarType tmax = 1;
    ScalarType dt   = 0.1;

    Eigen::VectorXd init = Eigen::VectorXd::Constant(Eigen::Index(Model::LctState::InfectionState::Count), 15);
    init[0]              = 200;
    init[3]              = 50;
    init[5]              = 30;

    Model model;
    for (size_t i = 0; i < LctState::Count; i++) {
        model.populations[mio::Index<LctState>(i)] = init[i];
    }

    mio::TimeSeries<ScalarType> result = mio::simulate<ScalarType, Model>(t0, tmax, dt, model);

    EXPECT_NEAR(result.get_last_time(), tmax, 1e-10);
    ScalarType sum_pop = init.sum();
    for (Eigen::Index i = 0; i < result.get_num_time_points(); i++) {
        ASSERT_NEAR(sum_pop, result[i].sum(), 1e-5);
    }
}

/* Test compares the result for an LCT SECIR model with one single subcompartment for each infection state 
    with the result of the equivalent ODE SECIR model. */
TEST(TestLCTSecir, compareWithOdeSecir)
{
    using Model     = mio::lsecir::Model<1, 1, 1, 1, 1>;
    using LctState  = Model::LctState;
    ScalarType t0   = 0;
    ScalarType tmax = 5;
    ScalarType dt   = 0.1;

    // Initialization vector for both models.
    Eigen::VectorXd init = Eigen::VectorXd::Constant(Eigen::Index(Model::LctState::InfectionState::Count), 15);
    init[0]              = 200;
    init[3]              = 50;
    init[5]              = 30;

    // Define LCT model.
    Model model_lct;
    //Set initial values
    for (size_t i = 0; i < LctState::Count; i++) {
        model_lct.populations[mio::Index<LctState>(i)] = init[i];
    }

    // Set Parameters.
    model_lct.parameters.get<mio::lsecir::TimeExposed>()            = 3.2;
    model_lct.parameters.get<mio::lsecir::TimeInfectedNoSymptoms>() = 2;
    model_lct.parameters.get<mio::lsecir::TimeInfectedSymptoms>()   = 5.8;
    model_lct.parameters.get<mio::lsecir::TimeInfectedSevere>()     = 9.5;
    model_lct.parameters.get<mio::lsecir::TimeInfectedCritical>()   = 7.1;

    model_lct.parameters.get<mio::lsecir::TransmissionProbabilityOnContact>() = 0.05;

    mio::ContactMatrixGroup& contact_matrix_lct = model_lct.parameters.get<mio::lsecir::ContactPatterns>();
    contact_matrix_lct[0]                       = mio::ContactMatrix(Eigen::MatrixXd::Constant(1, 1, 10));
    contact_matrix_lct[0].add_damping(0.7, mio::SimulationTime(2.));

    model_lct.parameters.get<mio::lsecir::RelativeTransmissionNoSymptoms>() = 0.7;
    model_lct.parameters.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() = 0.25;
    model_lct.parameters.get<mio::lsecir::StartDay>()                       = 50;
    model_lct.parameters.get<mio::lsecir::Seasonality>()                    = 0.1;
    model_lct.parameters.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 0.09;
    model_lct.parameters.get<mio::lsecir::SeverePerInfectedSymptoms>()      = 0.2;
    model_lct.parameters.get<mio::lsecir::CriticalPerSevere>()              = 0.25;
    model_lct.parameters.get<mio::lsecir::DeathsPerCritical>()              = 0.3;

    // Simulate.
    mio::TimeSeries<ScalarType> result_lct = mio::simulate<ScalarType, Model>(
        t0, tmax, dt, model_lct,
        std::make_shared<mio::ControlledStepperWrapper<ScalarType, boost::numeric::odeint::runge_kutta_cash_karp54>>());

    // Initialize ODE model with one age group.
    mio::osecir::Model model_ode(1);
    // Set initial distribution of the population.
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::Exposed}] =
        init[Eigen::Index(Model::LctState::InfectionState::Exposed)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedNoSymptoms}] =
        init[Eigen::Index(Model::LctState::InfectionState::InfectedNoSymptoms)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedNoSymptomsConfirmed}] = 0;
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedSymptoms}] =
        init[Eigen::Index(Model::LctState::InfectionState::InfectedSymptoms)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedSymptomsConfirmed}] = 0;
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedSevere}] =
        init[Eigen::Index(Model::LctState::InfectionState::InfectedSevere)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedCritical}] =
        init[Eigen::Index(Model::LctState::InfectionState::InfectedCritical)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::Recovered}] =
        init[Eigen::Index(Model::LctState::InfectionState::Recovered)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::Dead}] =
        init[Eigen::Index(Model::LctState::InfectionState::Dead)];
    model_ode.populations.set_difference_from_total({mio::AgeGroup(0), mio::osecir::InfectionState::Susceptible},
                                                    init.sum());

    // Set parameters according to the parameters of the LCT model.
    // No restrictions by additional parameters.
    model_ode.parameters.get<mio::osecir::TestAndTraceCapacity<double>>() = std::numeric_limits<double>::max();
    model_ode.parameters.get<mio::osecir::ICUCapacity<double>>()          = std::numeric_limits<double>::max();

    model_ode.parameters.set<mio::osecir::StartDay>(50);
    model_ode.parameters.set<mio::osecir::Seasonality<double>>(0.1);
    model_ode.parameters.get<mio::osecir::TimeExposed<double>>()[(mio::AgeGroup)0]            = 3.2;
    model_ode.parameters.get<mio::osecir::TimeInfectedNoSymptoms<double>>()[(mio::AgeGroup)0] = 2.0;
    model_ode.parameters.get<mio::osecir::TimeInfectedSymptoms<double>>()[(mio::AgeGroup)0]   = 5.8;
    model_ode.parameters.get<mio::osecir::TimeInfectedSevere<double>>()[(mio::AgeGroup)0]     = 9.5;
    model_ode.parameters.get<mio::osecir::TimeInfectedCritical<double>>()[(mio::AgeGroup)0]   = 7.1;

    mio::ContactMatrixGroup& contact_matrix_ode = model_ode.parameters.get<mio::osecir::ContactPatterns<double>>();
    contact_matrix_ode[0]                       = mio::ContactMatrix(Eigen::MatrixXd::Constant(1, 1, 10));
    contact_matrix_ode[0].add_damping(0.7, mio::SimulationTime(2.));

    model_ode.parameters.get<mio::osecir::TransmissionProbabilityOnContact<double>>()[(mio::AgeGroup)0] = 0.05;
    model_ode.parameters.get<mio::osecir::RelativeTransmissionNoSymptoms<double>>()[(mio::AgeGroup)0]   = 0.7;
    model_ode.parameters.get<mio::osecir::RecoveredPerInfectedNoSymptoms<double>>()[(mio::AgeGroup)0]   = 0.09;
    model_ode.parameters.get<mio::osecir::RiskOfInfectionFromSymptomatic<double>>()[(mio::AgeGroup)0]   = 0.25;
    model_ode.parameters.get<mio::osecir::SeverePerInfectedSymptoms<double>>()[(mio::AgeGroup)0]        = 0.2;
    model_ode.parameters.get<mio::osecir::CriticalPerSevere<double>>()[(mio::AgeGroup)0]                = 0.25;
    model_ode.parameters.get<mio::osecir::DeathsPerCritical<double>>()[(mio::AgeGroup)0]                = 0.3;

    // Simulate.
    mio::TimeSeries<double> result_ode = mio::osecir::simulate<double>(
        t0, tmax, dt, model_ode,
        std::make_shared<mio::ControlledStepperWrapper<double, boost::numeric::odeint::runge_kutta_cash_karp54>>());

    // Simulation results should be equal.
    ASSERT_EQ(result_lct.get_num_time_points(), result_ode.get_num_time_points());
    for (int i = 0; i < 4; ++i) {
        ASSERT_NEAR(result_lct.get_time(i), result_ode.get_time(i), 1e-5);

        ASSERT_NEAR(result_lct[i][Eigen::Index(Model::LctState::InfectionState::Susceptible)],
                    result_ode[i][Eigen::Index(mio::osecir::InfectionState::Susceptible)], 1e-5);
        ASSERT_NEAR(result_lct[i][Eigen::Index(Model::LctState::InfectionState::Exposed)],
                    result_ode[i][Eigen::Index(mio::osecir::InfectionState::Exposed)], 1e-5);
        ASSERT_NEAR(result_lct[i][Eigen::Index(Model::LctState::InfectionState::InfectedNoSymptoms)],
                    result_ode[i][Eigen::Index(mio::osecir::InfectionState::InfectedNoSymptoms)], 1e-5);
        ASSERT_NEAR(0, result_ode[i][Eigen::Index(mio::osecir::InfectionState::InfectedNoSymptomsConfirmed)], 1e-5);
        ASSERT_NEAR(result_lct[i][Eigen::Index(Model::LctState::InfectionState::InfectedSymptoms)],
                    result_ode[i][Eigen::Index(mio::osecir::InfectionState::InfectedSymptoms)], 1e-5);
        ASSERT_NEAR(0, result_ode[i][Eigen::Index(mio::osecir::InfectionState::InfectedSymptomsConfirmed)], 1e-5);
        ASSERT_NEAR(result_lct[i][Eigen::Index(Model::LctState::InfectionState::InfectedCritical)],
                    result_ode[i][Eigen::Index(mio::osecir::InfectionState::InfectedCritical)], 1e-5);
        ASSERT_NEAR(result_lct[i][Eigen::Index(Model::LctState::InfectionState::InfectedSevere)],
                    result_ode[i][Eigen::Index(mio::osecir::InfectionState::InfectedSevere)], 1e-5);
        ASSERT_NEAR(result_lct[i][Eigen::Index(Model::LctState::InfectionState::Recovered)],
                    result_ode[i][Eigen::Index(mio::osecir::InfectionState::Recovered)], 1e-5);
        ASSERT_NEAR(result_lct[i][Eigen::Index(Model::LctState::InfectionState::Dead)],
                    result_ode[i][Eigen::Index(mio::osecir::InfectionState::Dead)], 1e-5);
    }
}

// Test if the function get_derivatives() is working using a hand calculated result.
TEST(TestLCTSecir, testEvalRightHandSide)
{
    // Define model.
    // Chose more than one subcompartment for all compartments except S, R, D so that the function is correct for all selections.
    using Model          = mio::lsecir::Model<2, 3, 2, 2, 2>;
    using LctState       = Model::LctState;
    using InfectionState = LctState::InfectionState;

    Model model;

    // Define initial population distribution in infection states, one entry per subcompartment.
    std::vector<std::vector<ScalarType>> initial_populations = {{750},    {30, 20}, {20, 10, 10}, {30, 20},
                                                                {40, 10}, {10, 20}, {20},         {10}};

    // Transfer the initial values in initial_populations to the model.
    model.populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::Susceptible>())] =
        initial_populations[(size_t)InfectionState::Susceptible][0];
    for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::Exposed>(); i++) {
        model.populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::Exposed>() + i)] =
            initial_populations[(size_t)InfectionState::Exposed][i];
    }
    for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::InfectedNoSymptoms>(); i++) {
        model.populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::InfectedNoSymptoms>() + i)] =
            initial_populations[(size_t)InfectionState::InfectedNoSymptoms][i];
    }
    for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::InfectedSymptoms>(); i++) {
        model.populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::InfectedSymptoms>() + i)] =
            initial_populations[(size_t)InfectionState::InfectedSymptoms][i];
    }
    for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::InfectedSevere>(); i++) {
        model.populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::InfectedSevere>() + i)] =
            initial_populations[(size_t)InfectionState::InfectedSevere][i];
    }
    for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::InfectedCritical>(); i++) {
        model.populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::InfectedCritical>() + i)] =
            initial_populations[(size_t)InfectionState::InfectedCritical][i];
    }
    model.populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::Recovered>())] =
        initial_populations[(size_t)InfectionState::Recovered][0];
    model.populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::Dead>())] =
        initial_populations[(size_t)InfectionState::Dead][0];

    // Set parameters.
    model.parameters.set<mio::lsecir::TimeExposed>(3.2);
    model.parameters.get<mio::lsecir::TimeInfectedNoSymptoms>() = 2;
    model.parameters.get<mio::lsecir::TimeInfectedSymptoms>()   = 5.8;
    model.parameters.get<mio::lsecir::TimeInfectedSevere>()     = 9.5;
    model.parameters.get<mio::lsecir::TimeInfectedCritical>()   = 7.1;

    model.parameters.get<mio::lsecir::TransmissionProbabilityOnContact>() = 0.05;

    mio::ContactMatrixGroup& contact_matrix = model.parameters.get<mio::lsecir::ContactPatterns>();
    contact_matrix[0]                       = mio::ContactMatrix(Eigen::MatrixXd::Constant(1, 1, 10));

    model.parameters.get<mio::lsecir::RelativeTransmissionNoSymptoms>() = 0.7;
    model.parameters.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() = 0.25;
    model.parameters.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 0.09;
    model.parameters.get<mio::lsecir::Seasonality>()                    = 0.;
    model.parameters.get<mio::lsecir::StartDay>()                       = 0;
    model.parameters.get<mio::lsecir::SeverePerInfectedSymptoms>()      = 0.2;
    model.parameters.get<mio::lsecir::CriticalPerSevere>()              = 0.25;
    model.parameters.get<mio::lsecir::DeathsPerCritical>()              = 0.3;

    // Compare the result of get_derivatives() with a hand calculated result.
    size_t num_subcompartments = LctState::Count;
    Eigen::VectorXd dydt(num_subcompartments);
    model.get_derivatives(model.get_initial_values(), model.get_initial_values(), 0, dydt);

    Eigen::VectorXd compare(num_subcompartments);
    compare << -15.3409, -3.4091, 6.25, -17.5, 15, 0, 3.3052, 3.4483, -7.0417, 6.3158, -2.2906, -2.8169, 12.3899,
        1.6901;

    for (size_t i = 0; i < num_subcompartments; i++) {
        ASSERT_NEAR(compare[i], dydt[i], 1e-3);
    }
}

// Model setup to compare result with a previous output.
class ModelTestLCTSecir : public testing::Test
{
public:
    using Model          = mio::lsecir::Model<2, 3, 1, 1, 5>;
    using LctState       = Model::LctState;
    using InfectionState = LctState::InfectionState;

protected:
    virtual void SetUp()
    {
        // Define initial distribution of the population in the subcompartments.
        Eigen::VectorXd init(LctState::Count);
        std::vector<std::vector<ScalarType>> initial_populations = {{750}, {30, 20},          {20, 10, 10}, {50},
                                                                    {50},  {10, 10, 5, 3, 2}, {20},         {10}};
        model                                                    = new Model();
        // Transfer the initial values in initial_populations to the model.
        model->populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::Susceptible>())] =
            initial_populations[(size_t)InfectionState::Susceptible][0];
        for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::Exposed>(); i++) {
            model->populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::Exposed>() + i)] =
                initial_populations[(size_t)InfectionState::Exposed][i];
        }
        for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::InfectedNoSymptoms>(); i++) {
            model->populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::InfectedNoSymptoms>() +
                                                    i)] =
                initial_populations[(size_t)InfectionState::InfectedNoSymptoms][i];
        }
        for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::InfectedSymptoms>(); i++) {
            model
                ->populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::InfectedSymptoms>() + i)] =
                initial_populations[(size_t)InfectionState::InfectedSymptoms][i];
        }
        for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::InfectedSevere>(); i++) {
            model->populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::InfectedSevere>() + i)] =
                initial_populations[(size_t)InfectionState::InfectedSevere][i];
        }
        for (size_t i = 0; i < LctState::get_num_subcompartments<InfectionState::InfectedCritical>(); i++) {
            model
                ->populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::InfectedCritical>() + i)] =
                initial_populations[(size_t)InfectionState::InfectedCritical][i];
        }
        model->populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::Recovered>())] =
            initial_populations[(size_t)InfectionState::Recovered][0];
        model->populations[mio::Index<LctState>(LctState::get_first_index<InfectionState::Dead>())] =
            initial_populations[(size_t)InfectionState::Dead][0];

        // Set parameters.
        model->parameters.get<mio::lsecir::TimeExposed>()                      = 3.2;
        model->parameters.get<mio::lsecir::TimeInfectedNoSymptoms>()           = 2;
        model->parameters.get<mio::lsecir::TimeInfectedSymptoms>()             = 5.8;
        model->parameters.get<mio::lsecir::TimeInfectedSevere>()               = 9.5;
        model->parameters.get<mio::lsecir::TimeInfectedCritical>()             = 7.1;
        model->parameters.get<mio::lsecir::TransmissionProbabilityOnContact>() = 0.05;

        mio::ContactMatrixGroup& contact_matrix = model->parameters.get<mio::lsecir::ContactPatterns>();
        contact_matrix[0]                       = mio::ContactMatrix(Eigen::MatrixXd::Constant(1, 1, 10));
        contact_matrix[0].add_damping(0.7, mio::SimulationTime(2.));

        model->parameters.get<mio::lsecir::RelativeTransmissionNoSymptoms>() = 0.7;
        model->parameters.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() = 0.25;
        model->parameters.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 0.09;
        model->parameters.get<mio::lsecir::SeverePerInfectedSymptoms>()      = 0.2;
        model->parameters.get<mio::lsecir::CriticalPerSevere>()              = 0.25;
        model->parameters.get<mio::lsecir::DeathsPerCritical>()              = 0.3;
    }

    virtual void TearDown()
    {
        delete model;
    }

public:
    Model* model = nullptr;
};

// Test compares a simulation with the result of a previous run stored in a .csv file.
TEST_F(ModelTestLCTSecir, compareWithPreviousRun)
{
    ScalarType tmax                    = 3;
    mio::TimeSeries<ScalarType> result = mio::simulate<ScalarType, ModelTestLCTSecir::Model>(
        0, tmax, 0.5, *model,
        std::make_shared<mio::ControlledStepperWrapper<ScalarType, boost::numeric::odeint::runge_kutta_cash_karp54>>());

    // Compare subcompartments.
    auto compare = load_test_data_csv<ScalarType>("lct-secir-subcompartments-compare.csv");

    ASSERT_EQ(compare.size(), static_cast<size_t>(result.get_num_time_points()));
    for (size_t i = 0; i < compare.size(); i++) {
        ASSERT_EQ(compare[i].size(), static_cast<size_t>(result.get_num_elements()) + 1) << "at row " << i;
        ASSERT_NEAR(result.get_time(i), compare[i][0], 1e-7) << "at row " << i;
        for (size_t j = 1; j < compare[i].size(); j++) {
            ASSERT_NEAR(result.get_value(i)[j - 1], compare[i][j], 1e-7) << " at row " << i;
        }
    }

    // Compare InfectionState compartments.
    mio::TimeSeries<ScalarType> population = LctState::calculate_compartments(result);
    auto compare_population                = load_test_data_csv<ScalarType>("lct-secir-compartments-compare.csv");

    ASSERT_EQ(compare_population.size(), static_cast<size_t>(population.get_num_time_points()));
    for (size_t i = 0; i < compare_population.size(); i++) {
        ASSERT_EQ(compare_population[i].size(), static_cast<size_t>(population.get_num_elements()) + 1)
            << "at row " << i;
        ASSERT_NEAR(population.get_time(i), compare_population[i][0], 1e-7) << "at row " << i;
        for (size_t j = 1; j < compare_population[i].size(); j++) {
            ASSERT_NEAR(population.get_value(i)[j - 1], compare_population[i][j], 1e-7) << " at row " << i;
        }
    }
}

// Check constraints of Parameters class.
TEST(TestLCTSecir, testConstraintsParameters)
{
    // Deactivate temporarily log output for next tests.
    mio::set_log_level(mio::LogLevel::off);

    // Check for exceptions of parameters.
    mio::lsecir::Parameters parameters_lct;
    parameters_lct.get<mio::lsecir::TimeExposed>()                      = 0;
    parameters_lct.get<mio::lsecir::TimeInfectedNoSymptoms>()           = 3.1;
    parameters_lct.get<mio::lsecir::TimeInfectedSymptoms>()             = 6.1;
    parameters_lct.get<mio::lsecir::TimeInfectedSevere>()               = 11.1;
    parameters_lct.get<mio::lsecir::TimeInfectedCritical>()             = 17.1;
    parameters_lct.get<mio::lsecir::TransmissionProbabilityOnContact>() = 0.01;
    mio::ContactMatrixGroup contact_matrix                              = mio::ContactMatrixGroup(1, 1);
    parameters_lct.get<mio::lsecir::ContactPatterns>()                  = mio::UncertainContactMatrix(contact_matrix);

    parameters_lct.get<mio::lsecir::RelativeTransmissionNoSymptoms>() = 1;
    parameters_lct.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() = 1;
    parameters_lct.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 0.1;
    parameters_lct.get<mio::lsecir::SeverePerInfectedSymptoms>()      = 0.1;
    parameters_lct.get<mio::lsecir::CriticalPerSevere>()              = 0.1;
    parameters_lct.get<mio::lsecir::DeathsPerCritical>()              = 0.1;

    // Check improper TimeExposed.
    bool constraint_check = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::TimeExposed>() = 3.1;

    // Check TimeInfectedNoSymptoms.
    parameters_lct.get<mio::lsecir::TimeInfectedNoSymptoms>() = 0.1;
    constraint_check                                          = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::TimeInfectedNoSymptoms>() = 3.1;

    // Check TimeInfectedSymptoms.
    parameters_lct.get<mio::lsecir::TimeInfectedSymptoms>() = -0.1;
    constraint_check                                        = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::TimeInfectedSymptoms>() = 6.1;

    // Check TimeInfectedSevere.
    parameters_lct.get<mio::lsecir::TimeInfectedSevere>() = 0.5;
    constraint_check                                      = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::TimeInfectedSevere>() = 11.1;

    // Check TimeInfectedCritical.
    parameters_lct.get<mio::lsecir::TimeInfectedCritical>() = 0.;
    constraint_check                                        = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::TimeInfectedCritical>() = 17.1;

    // Check TransmissionProbabilityOnContact.
    parameters_lct.get<mio::lsecir::TransmissionProbabilityOnContact>() = -1;
    constraint_check                                                    = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::TransmissionProbabilityOnContact>() = 0.01;

    // Check RelativeTransmissionNoSymptoms.
    parameters_lct.get<mio::lsecir::RelativeTransmissionNoSymptoms>() = 1.5;
    constraint_check                                                  = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::RelativeTransmissionNoSymptoms>() = 1;

    // Check RiskOfInfectionFromSymptomatic.
    parameters_lct.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() = 1.5;
    constraint_check                                                  = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() = 1;

    // Check RecoveredPerInfectedNoSymptoms.
    parameters_lct.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 1.5;
    constraint_check                                                  = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 0.1;

    // Check SeverePerInfectedSymptoms.
    parameters_lct.get<mio::lsecir::SeverePerInfectedSymptoms>() = -1;
    constraint_check                                             = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::SeverePerInfectedSymptoms>() = 0.1;

    // Check CriticalPerSevere.
    parameters_lct.get<mio::lsecir::CriticalPerSevere>() = -1;
    constraint_check                                     = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::CriticalPerSevere>() = 0.1;

    // Check DeathsPerCritical.
    parameters_lct.get<mio::lsecir::DeathsPerCritical>() = -1;
    constraint_check                                     = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::DeathsPerCritical>() = 0.1;

    // Check Seasonality.
    parameters_lct.get<mio::lsecir::Seasonality>() = 1;
    constraint_check                               = parameters_lct.check_constraints();
    EXPECT_TRUE(constraint_check);
    parameters_lct.get<mio::lsecir::Seasonality>() = 0.1;

    // Check with correct parameters.
    constraint_check = parameters_lct.check_constraints();
    EXPECT_FALSE(constraint_check);

    // Reactive log output.
    mio::set_log_level(mio::LogLevel::warn);
}

// Test calculate_compartments function of the LctInfectionState class with a TimeSeries that has an incorrect
// number of elements.
TEST(TestLctInfectionState, testCalculatePopWrongSize)
{
    // Deactivate temporarily log output because an error is expected.
    mio::set_log_level(mio::LogLevel::off);

    using Model    = mio::lsecir::Model<2, 3, 1, 1, 5>;
    using LctState = Model::LctState;

    // TimeSeries has to have LctState::Count elements.
    size_t wrong_size = LctState::Count - 2;
    // Define TimeSeries with wrong_size elements.
    mio::TimeSeries<ScalarType> wrong_num_elements(wrong_size);
    Eigen::VectorXd vec_wrong_size = Eigen::VectorXd::Ones(wrong_size);
    wrong_num_elements.add_time_point(-10, vec_wrong_size);
    wrong_num_elements.add_time_point(-9, vec_wrong_size);
    // Call the calculate_compartments function with the TimeSeries with a wrong number of elements.
    mio::TimeSeries<ScalarType> population = LctState::calculate_compartments(wrong_num_elements);
    // A TimeSeries of the right size with values -1 is expected.
    EXPECT_EQ(1, population.get_num_time_points());
    for (int i = 0; i < population.get_num_elements(); i++) {
        EXPECT_EQ(-1, population.get_last_value()[i]);
    }
    // Reactive log output.
    mio::set_log_level(mio::LogLevel::warn);
}

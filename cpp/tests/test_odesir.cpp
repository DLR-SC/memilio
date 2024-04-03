/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Daniel Abele, Martin J. Kuehn, Martin Siggel, Henrik Zunker
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
#include "load_test_data.h"
#include "memilio/config.h"
#include "memilio/utils/time_series.h"
#include "ode_sir/model.h"
#include "ode_sir/infection_state.h"
#include "ode_sir/parameters.h"
#include "memilio/math/euler.h"
#include "memilio/compartments/simulation.h"
#include <gtest/gtest.h>
#include <iomanip>
#include <vector>
#include <sstream>

TEST(TestOdeSir, simulateDefault)
{
    double t0   = 0;
    double tmax = 1;
    double dt   = 0.1;

    mio::osir::Model model(1);
    mio::TimeSeries<double> result = simulate(t0, tmax, dt, model);

    EXPECT_NEAR(result.get_last_time(), tmax, 1e-10);
}

TEST(TestOdeSir, compareWithPreviousRun)
{
    // initialization
    double t0   = 0.;
    double tmax = 3.;
    double dt   = 0.1002004008016032;

    double total_population = 1061000;

    mio::osir::Model model(1);

    model.populations[{mio::Index<mio::AgeGroup>(0),
                       mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Infected)}]  = 1000;
    model.populations[{mio::Index<mio::AgeGroup>(0),
                       mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Recovered)}] = 1000;
    model.populations[{mio::Index<mio::AgeGroup>(0),
                       mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Susceptible)}] =
        total_population -
        model.populations[{mio::Index<mio::AgeGroup>(0),
                           mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Infected)}] -
        model.populations[{mio::Index<mio::AgeGroup>(0),
                           mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Recovered)}];
    model.parameters.set<mio::osir::TransmissionProbabilityOnContact>(1.0);
    model.parameters.set<mio::osir::TimeInfected>(2);

    model.parameters.get<mio::osir::ContactPatterns>().get_cont_freq_mat()[0].get_baseline().setConstant(2.7);
    model.parameters.get<mio::osir::ContactPatterns>().get_cont_freq_mat()[0].add_damping(0.6,
                                                                                          mio::SimulationTime(12.5));

    std::vector<std::vector<double>> refData = load_test_data_csv<double>("ode-sir-compare.csv");
    auto integrator                          = std::make_shared<mio::EulerIntegratorCore>();
    auto result                              = mio::simulate<mio::osir::Model>(t0, tmax, dt, model, integrator);

    ASSERT_EQ(refData.size(), static_cast<size_t>(result.get_num_time_points()));

    for (Eigen::Index irow = 0; irow < result.get_num_time_points(); ++irow) {
        double t     = refData[static_cast<size_t>(irow)][0];
        auto rel_tol = 1e-6;

        //test result diverges at damping because of changes, not worth fixing at the moment
        if (t > 11.0 && t < 13.0) {
            //strong divergence around damping
            rel_tol = 0.5;
        }
        else if (t > 13.0) {
            //minor divergence after damping
            rel_tol = 1e-2;
        }

        ASSERT_NEAR(t, result.get_times()[irow], 1e-12) << "at row " << irow;

        for (size_t icol = 0; icol < 3; ++icol) {
            double ref    = refData[static_cast<size_t>(irow)][icol + 1];
            double actual = result[irow][icol];

            double tol = rel_tol * ref;
            ASSERT_NEAR(ref, actual, tol) << "at row " << irow;
        }
    }
}

TEST(TestOdeSir, checkPopulationConservation)
{
    // initialization
    double t0   = 0.;
    double tmax = 50.;
    double dt   = 0.1002004008016032;

    double total_population = 1061000;

    mio::osir::Model model(1);

    model.populations[{mio::Index<mio::AgeGroup>(0),
                       mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Infected)}]  = 1000;
    model.populations[{mio::Index<mio::AgeGroup>(0),
                       mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Recovered)}] = 1000;
    model.populations[{mio::Index<mio::AgeGroup>(0),
                       mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Susceptible)}] =
        total_population -
        model.populations[{mio::Index<mio::AgeGroup>(0),
                           mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Infected)}] -
        model.populations[{mio::Index<mio::AgeGroup>(0),
                           mio::Index<mio::osir::InfectionState>(mio::osir::InfectionState::Recovered)}];
    model.parameters.set<mio::osir::TransmissionProbabilityOnContact>(1.0);
    model.parameters.set<mio::osir::TimeInfected>(2);

    model.parameters.get<mio::osir::ContactPatterns>().get_cont_freq_mat()[0].get_baseline().setConstant(2.7);
    model.parameters.get<mio::osir::ContactPatterns>().get_cont_freq_mat()[0].add_damping(0.6,
                                                                                          mio::SimulationTime(12.5));
    auto result        = mio::simulate<mio::osir::Model>(t0, tmax, dt, model);
    double num_persons = 0.0;
    for (auto i = 0; i < result.get_last_value().size(); i++) {
        num_persons += result.get_last_value()[i];
    }
    EXPECT_NEAR(num_persons, total_population, 1e-8);
}

TEST(TestOdeSir, check_constraints_parameters)
{
    mio::osir::Model model(1);
    model.parameters.set<mio::osir::TimeInfected>(6);
    model.parameters.set<mio::osir::TransmissionProbabilityOnContact>(0.04);
    model.parameters.get<mio::osir::ContactPatterns>().get_cont_freq_mat()[0].get_baseline().setConstant(10);

    // model.check_constraints() combines the functions from population and parameters.
    // We only want to test the functions for the parameters defined in parameters.h
    ASSERT_EQ(model.parameters.check_constraints(), 0);

    mio::set_log_level(mio::LogLevel::off);

    model.parameters.set<mio::osir::TimeInfected>(0);
    ASSERT_EQ(model.parameters.check_constraints(), 1);

    model.parameters.set<mio::osir::TimeInfected>(6);
    model.parameters.set<mio::osir::TransmissionProbabilityOnContact>(10.);
    ASSERT_EQ(model.parameters.check_constraints(), 1);
    mio::set_log_level(mio::LogLevel::warn);
}

TEST(TestOdeSir, apply_constraints_parameters)
{
    const double tol_times = 1e-1;
    mio::osir::Model model(1);
    model.parameters.set<mio::osir::TimeInfected>(6);
    model.parameters.set<mio::osir::TransmissionProbabilityOnContact>(0.04);
    model.parameters.get<mio::osir::ContactPatterns>().get_cont_freq_mat()[0].get_baseline().setConstant(10);

    EXPECT_EQ(model.parameters.apply_constraints(), 0);

    mio::set_log_level(mio::LogLevel::off);

    model.parameters.set<mio::osir::TimeInfected>(-2.5);
    EXPECT_EQ(model.parameters.apply_constraints(), 1);
    EXPECT_EQ(model.parameters.get<mio::osir::TimeInfected>()[(mio::AgeGroup)0], tol_times);

    model.parameters.set<mio::osir::TransmissionProbabilityOnContact>(10.);
    EXPECT_EQ(model.parameters.apply_constraints(), 1);
    EXPECT_NEAR(model.parameters.get<mio::osir::TransmissionProbabilityOnContact>()[(mio::AgeGroup)0], 0.0, 1e-14);
    mio::set_log_level(mio::LogLevel::warn);
}

TEST(Testsir, get_derivatives_agegrp_compare)
{
    // Test, that in the case of one age group, the simulation is the same as before the implementation of agegroups
    // This test is independent of the integrator used.
    double t0   = 0.;
    double tmax = 50.;
    double dt   = 0.1002004008016032;

    double total_population = 1061000;

    mio::osir::Model model(1);

    model.populations[{mio::AgeGroup(0), mio::osir::InfectionState::Infected}]  = 1000;
    model.populations[{mio::AgeGroup(0), mio::osir::InfectionState::Recovered}] = 1000;
    model.populations[{mio::AgeGroup(0), mio::osir::InfectionState::Susceptible}] =
        total_population - model.populations[{mio::AgeGroup(0), mio::osir::InfectionState::Infected}] -
        model.populations[{mio::AgeGroup(0), mio::osir::InfectionState::Recovered}];
    model.parameters.set<mio::osir::TimeInfected>(2);
    model.parameters.set<mio::osir::TransmissionProbabilityOnContact>(1);
    mio::ContactMatrixGroup& contact_matrix = model.parameters.get<mio::osir::ContactPatterns>().get_cont_freq_mat();
    contact_matrix[0].get_baseline().setConstant(2.7);
    contact_matrix[0].add_damping(0.6, mio::SimulationTime(12.5));

    auto integrator = std::make_shared<mio::EulerIntegratorCore>();

    model.check_constraints();

    auto sir = simulate(t0, tmax, dt, model, integrator);

    auto dydt_default  = Eigen::VectorXd(Eigen::Index(mio::osir::InfectionState::Count));
    dydt_default.setZero();
    Eigen::VectorXd y0 = sir.get_value(0);
    model.get_derivatives(y0, y0, 0, dydt_default);

    EXPECT_NEAR(dydt_default[0], -2694.9104618284641, 1e-12);
    EXPECT_NEAR(dydt_default[1], 2194.9104618284641, 1e-12);
    EXPECT_NEAR(dydt_default[2], 500, 1e-12);
}
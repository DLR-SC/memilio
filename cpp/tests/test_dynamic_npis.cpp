/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Daniel Abele
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
#include "memilio/epidemiology/dynamic_npis.h"
#include "memilio/mobility/mobility.h"
#include "secir/secir.h"
#include "matchers.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(DynamicNPIs, init)
{
    mio::DynamicNPIs npis;
    EXPECT_EQ(npis.get_thresholds().size(), 0);
    EXPECT_EQ(npis.get_max_exceeded_threshold(0.0), npis.get_thresholds().end());
}

TEST(DynamicNPIs, set_threshold)
{
    mio::DynamicNPIs npis;
    npis.set_threshold(0.5, {mio::DampingSampling(123.0, mio::DampingLevel(0), mio::DampingType(0),
                                                  mio::SimulationTime(0.0), {}, Eigen::VectorXd(1))});
    npis.set_threshold(1.0, {mio::DampingSampling(543.0, mio::DampingLevel(0), mio::DampingType(0),
                                                  mio::SimulationTime(0.0), {}, Eigen::VectorXd(1))});

    EXPECT_EQ(npis.get_thresholds()[0].first, 1.0);
    EXPECT_EQ(npis.get_thresholds()[0].second[0].get_value().value(), 543.0);
    EXPECT_EQ(npis.get_thresholds()[1].first, 0.5);
    EXPECT_EQ(npis.get_thresholds()[1].second[0].get_value().value(), 123.0);
}

TEST(DynamicNPIs, get_threshold)
{
    mio::DynamicNPIs npis;
    npis.set_threshold(0.5, {mio::DampingSampling(0.5, mio::DampingLevel(0), mio::DampingType(0),
                                                  mio::SimulationTime(0.0), {}, Eigen::VectorXd(1))});
    npis.set_threshold(1.0, {mio::DampingSampling(0.5, mio::DampingLevel(0), mio::DampingType(0),
                                                  mio::SimulationTime(0.0), {}, Eigen::VectorXd(1))});

    EXPECT_EQ(npis.get_max_exceeded_threshold(2.0), npis.get_thresholds().begin());
    EXPECT_EQ(npis.get_max_exceeded_threshold(0.75), npis.get_thresholds().begin() + 1);
    EXPECT_EQ(npis.get_max_exceeded_threshold(1.0), npis.get_thresholds().begin() + 1);
    EXPECT_EQ(npis.get_max_exceeded_threshold(0.25), npis.get_thresholds().end());
    EXPECT_EQ(npis.get_max_exceeded_threshold(0.5), npis.get_thresholds().end());
}

TEST(DynamicNPIs, get_damping_indices)
{
    using Damping                 = mio::Damping<mio::RectMatrixShape>;
    using DampingMatrixExpression = mio::DampingMatrixExpression<mio::Dampings<Damping>>;
    DampingMatrixExpression dampexpr(1, 1);
    dampexpr.add_damping(0.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.3));
    dampexpr.add_damping(0.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.4));
    dampexpr.add_damping(0.0, mio::DampingLevel(1), mio::DampingType(1), mio::SimulationTime(0.6));
    dampexpr.add_damping(0.0, mio::DampingLevel(0), mio::DampingType(1), mio::SimulationTime(0.5));
    dampexpr.add_damping(0.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.7));
    dampexpr.add_damping(0.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.9));

    auto indices =
        mio::get_damping_indices(dampexpr, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.3),
                                 mio::SimulationTime(0.9)); //time period is open interval!

    EXPECT_THAT(indices, testing::ElementsAre(1, 4));
}

TEST(DynamicNPIs, get_active_damping)
{
    using Damping                 = mio::Damping<mio::RectMatrixShape>;
    using DampingMatrixExpression = mio::DampingMatrixExpression<mio::Dampings<Damping>>;
    DampingMatrixExpression dampexpr(1, 1);
    dampexpr.add_damping(0.3, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.3));
    dampexpr.add_damping(0.4, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.4));
    dampexpr.add_damping(0.6, mio::DampingLevel(1), mio::DampingType(1), mio::SimulationTime(0.6));
    dampexpr.add_damping(0.5, mio::DampingLevel(0), mio::DampingType(1), mio::SimulationTime(0.5));
    dampexpr.add_damping(0.7, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.7));
    dampexpr.add_damping(0.9, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.9));

    auto a = mio::get_active_damping(dampexpr, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.6));
    EXPECT_EQ(print_wrap(a), print_wrap(Eigen::MatrixXd::Constant(1, 1, 0.4)));

    auto b = mio::get_active_damping(dampexpr, mio::DampingLevel(1), mio::DampingType(1), mio::SimulationTime(0.6));
    EXPECT_EQ(print_wrap(b), print_wrap(Eigen::MatrixXd::Constant(1, 1, 0.6)));

    auto c = mio::get_active_damping(dampexpr, mio::DampingLevel(1), mio::DampingType(1), mio::SimulationTime(0.4));
    EXPECT_EQ(print_wrap(c), print_wrap(Eigen::MatrixXd::Constant(1, 1, 0.0)));
}

TEST(DynamicNPIs, get_active_damping_empty)
{
    using Damping                 = mio::Damping<mio::RectMatrixShape>;
    using DampingMatrixExpression = mio::DampingMatrixExpression<mio::Dampings<Damping>>;
    DampingMatrixExpression dampexpr(1, 1);

    auto d = mio::get_active_damping(dampexpr, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.6));
    EXPECT_EQ(print_wrap(d), print_wrap(Eigen::MatrixXd::Constant(1, 1, 0.0)));
}

TEST(DynamicNPIs, implement_empty)
{
    using Damping                 = mio::Damping<mio::ColumnVectorShape>;
    using DampingMatrixExpression = mio::DampingMatrixExpression<mio::Dampings<Damping>>;
    mio::DampingMatrixExpressionGroup<DampingMatrixExpression> dampexprs(2, 2);
    auto make_mask = [](auto& g) {
        return g;
    };

    auto dynamic_npis = std::vector<mio::DampingSampling>({mio::DampingSampling(
        0.8, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0), {0, 1}, Eigen::VectorXd::Ones(2))});
    mio::implement_dynamic_npis(dampexprs, dynamic_npis, mio::SimulationTime(0.45), mio::SimulationTime(0.6),
                                make_mask);

    EXPECT_EQ(dampexprs[0].get_dampings().size(), 2);
    EXPECT_DOUBLE_EQ(dampexprs[0].get_dampings()[0].get_time().get(), 0.45);
    EXPECT_THAT(dampexprs[0].get_dampings()[0].get_coeffs(), MatrixNear(Eigen::VectorXd::Constant(2, 0.8)));
    EXPECT_DOUBLE_EQ(dampexprs[0].get_dampings()[1].get_time().get(), 0.6);
    EXPECT_THAT(dampexprs[0].get_dampings()[1].get_coeffs(), MatrixNear(Eigen::VectorXd::Zero(2)));
    EXPECT_EQ(dampexprs[1].get_dampings().size(), 2);
    EXPECT_DOUBLE_EQ(dampexprs[1].get_dampings()[0].get_time().get(), 0.45);
    EXPECT_THAT(dampexprs[1].get_dampings()[0].get_coeffs(), MatrixNear(Eigen::VectorXd::Constant(2, 0.8)));
    EXPECT_DOUBLE_EQ(dampexprs[1].get_dampings()[1].get_time().get(), 0.6);
    EXPECT_THAT(dampexprs[1].get_dampings()[1].get_coeffs(), MatrixNear(Eigen::VectorXd::Zero(2)));
}

TEST(DynamicNPIs, implement)
{
    using Damping                 = mio::Damping<mio::RectMatrixShape>;
    using DampingMatrixExpression = mio::DampingMatrixExpression<mio::Dampings<Damping>>;
    mio::DampingMatrixExpressionGroup<DampingMatrixExpression> dampexprs(2, 3, 1);
    auto make_mask = [](auto& g) {
        return (Eigen::MatrixXd(3, 1) << g(0, 0), g(1, 0), g(2, 0)).finished();
    };

    dampexprs[0].add_damping(0.3, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.3));
    dampexprs[0].add_damping(0.4, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.4));
    dampexprs[0].add_damping(0.5, mio::DampingLevel(0), mio::DampingType(1), mio::SimulationTime(0.5));
    dampexprs[0].add_damping(0.7, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.7));

    dampexprs[1].add_damping(0.5, mio::DampingLevel(1), mio::DampingType(1), mio::SimulationTime(0.5));
    dampexprs[1].add_damping(0.9, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.9));

    {
        auto dynamic_npis = std::vector<mio::DampingSampling>(
            {mio::DampingSampling(0.8, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0), {0, 1},
                                  Eigen::MatrixXd::Ones(3, 1))});
        mio::implement_dynamic_npis(dampexprs, dynamic_npis, mio::SimulationTime(0.45), mio::SimulationTime(0.6),
                                    make_mask);
    }

    EXPECT_THAT(
        dampexprs[0].get_dampings(),
        testing::ElementsAre(
            Damping(0.3, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.3), 3, 1), //before npi
            Damping(0.4, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.4), 3, 1), //before npi
            Damping(0.8, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.45), 3, 1), //npi begins
            Damping(0.5, mio::DampingLevel(0), mio::DampingType(1), mio::SimulationTime(0.5), 3, 1), //other type
            Damping(0.4, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.6), 3, 1), //npi ends
            Damping(0.7, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.7), 3, 1))); //after npi

    EXPECT_THAT(
        dampexprs[1].get_dampings(),
        testing::ElementsAre(
            Damping(0.8, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.45), 3, 1), //npi begins
            Damping(0.5, mio::DampingLevel(1), mio::DampingType(1), mio::SimulationTime(0.5), 3, 1), //other type/level
            Damping(0.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.6), 3, 1), //npi ends
            Damping(0.9, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.9), 3, 1))); //after npi

    {
        auto dynamic_npis = std::vector<mio::DampingSampling>({mio::DampingSampling(
            0.6, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0), {0}, Eigen::MatrixXd::Ones(3, 1))});
        mio::implement_dynamic_npis(dampexprs, dynamic_npis, mio::SimulationTime(0.3), mio::SimulationTime(0.9),
                                    make_mask);
    }

    EXPECT_THAT(
        dampexprs[0].get_dampings(),
        testing::ElementsAre(
            Damping(0.6, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.3), 3, 1), //new npi begins
            Damping(0.8, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.45), 3,
                    1), //old npi begins, is kept because it's bigger
            Damping(0.5, mio::DampingLevel(0), mio::DampingType(1), mio::SimulationTime(0.5), 3, 1), //other type
            Damping(0.6, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.6), 3,
                    1), //old npi ends, down to value of new npi
            Damping(
                0.7, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.7), 3,
                1))); //bigger than new npi, new npi ends at t = 0.9, but is already overwritten here by a higher value

    //second matrix not changed by the new npi
    EXPECT_THAT(
        dampexprs[1].get_dampings(),
        testing::ElementsAre(Damping(0.8, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.45), 3, 1),
                             Damping(0.5, mio::DampingLevel(1), mio::DampingType(1), mio::SimulationTime(0.5), 3, 1),
                             Damping(0.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.6), 3, 1),
                             Damping(0.9, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0.9), 3, 1)));
}

namespace mio_test
{

enum class Compartments
{
    A = 0,
    B,
    Count,
};

struct DummyModel {
    using Compartments = mio_test::Compartments;
};

struct DummySim {
    using Model = DummyModel;
    template <class V>
    DummySim(V&& y0)
        : result(0.0, std::forward<V>(y0))
    {
        ON_CALL(*this, advance).WillByDefault([this](auto t) {
            result.reserve(result.get_num_time_points() + 1);
            result.add_time_point(t, result.get_last_value());
        });
        ON_CALL(*this, get_result).WillByDefault(testing::ReturnRef(result));
    }

    MOCK_METHOD(void, advance, (double), ());
    MOCK_METHOD(mio::TimeSeries<double>&, get_result, ());

    mio::TimeSeries<double> result;
    DummyModel model;
};

//overload required for dynamic NPIs
template <class DummySim>
double get_infections_relative(const DummySim&, double, const Eigen::Ref<const Eigen::VectorXd>& y)
{
    return y[0] / y.sum();
}

//overload required for because the mock is not a compartment model simulation
template <class DummySim>
void calculate_migration_returns(Eigen::Ref<mio::TimeSeries<double>::Vector>, const DummySim&,
                                 Eigen::Ref<const mio::TimeSeries<double>::Vector>, double, double)
{
}

} // namespace mio_test

TEST(DynamicNPIs, migration)
{
    mio::SimulationNode<mio_test::DummySim> node_from((Eigen::VectorXd(2) << 0.0, 1.0).finished());
    mio::SimulationNode<mio_test::DummySim> node_to((Eigen::VectorXd(2) << 0.0, 1.0).finished());

    auto last_state_safe = (Eigen::VectorXd(2) << 0.01, 0.99).finished();
    auto last_state_crit = (Eigen::VectorXd(2) << 0.02, 0.98).finished();

    mio::DynamicNPIs npis;
    npis.set_threshold(
        0.015 * 100'000,
        {mio::DampingSampling{
            1.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0), {0}, Eigen::VectorXd::Ones(2)}});
    npis.set_duration(mio::SimulationTime(5.0));
    npis.set_base_value(100'000);
    npis.set_interval(mio::SimulationTime(3.0));

    mio::MigrationCoefficientGroup coeffs(1, 2);
    mio::MigrationParameters parameters(coeffs);
    parameters.set_dynamic_npis_infected(npis);

    mio::MigrationEdge edge(parameters);

    ASSERT_EQ(edge.get_parameters().get_coefficients()[0].get_dampings().size(), 0); //initial

    edge.apply_migration(0.5, 0.5, node_from, node_to);

    ASSERT_EQ(edge.get_parameters().get_coefficients()[0].get_dampings().size(), 0); //not check at the beginning

    EXPECT_CALL(node_from.get_simulation(), advance).Times(1).WillOnce([&](auto t) {
        node_from.get_simulation().result.add_time_point(t, last_state_safe);
    });
    node_from.evolve(3.0, 2.5);
    node_to.evolve(3.0, 2.5);
    edge.apply_migration(3.0, 2.5, node_from, node_to);

    ASSERT_EQ(edge.get_parameters().get_coefficients()[0].get_dampings().size(), 0); //threshold not exceeded

    EXPECT_CALL(node_from.get_simulation(), advance).Times(1).WillOnce([&](auto t) {
        node_from.get_simulation().result.add_time_point(t, last_state_crit);
    });
    node_from.evolve(4.5, 1.5);
    node_to.evolve(4.5, 1.5);
    edge.apply_migration(4.5, 1.5, node_from, node_to);

    ASSERT_EQ(edge.get_parameters().get_coefficients()[0].get_dampings().size(),
              0); //threshold exceeded, but only check every 3 days

    EXPECT_CALL(node_from.get_simulation(), advance).Times(1).WillOnce([&](auto t) {
        node_from.get_simulation().result.add_time_point(t, last_state_crit);
    });
    node_from.evolve(6.0, 1.5);
    node_to.evolve(6.0, 1.5);
    edge.apply_migration(6.0, 1.5, node_from, node_to);

    ASSERT_EQ(edge.get_parameters().get_coefficients()[0].get_dampings().size(), 2); //NPIs implemented
}

namespace mio_test
{

class MockSimulation
{
public:
    MockSimulation(mio::SecirModel m, double t0, double /*dt*/)
        : m_model(m)
        , m_result(t0, m.get_initial_values())
    {
    }
    auto& get_result() const
    {
        return m_result;
    }
    auto& get_result()
    {
        return m_result;
    }
    auto& get_model()
    {
        return m_model;
    }
    const auto& get_model() const
    {
        return m_model;
    }
    auto advance(double t)
    {
        //simple simulation that is constant over time
        m_result.reserve(m_result.get_num_time_points() + 1);
        return m_result.add_time_point(t, m_result.get_last_value());
    }

    mio::SecirModel m_model;
    mio::TimeSeries<double> m_result;
};

} // namespace mio_test

TEST(DynamicNPIs, secir_threshold_safe)
{
    mio::SecirModel model(1);
    model.populations[{mio::AgeGroup(0), mio::InfectionState::InfectedSymptoms}] = 1.0;
    model.populations.set_difference_from_total({mio::AgeGroup(0), mio::InfectionState::Susceptible}, 100.0);

    mio::DynamicNPIs npis;
    npis.set_threshold(
        0.05 * 23'000,
        {mio::DampingSampling{
            1.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0), {0}, Eigen::VectorXd::Ones(1)}});
    npis.set_duration(mio::SimulationTime(5.0));
    npis.set_base_value(23'000);
    model.parameters.get<mio::DynamicNPIsInfectedSymptoms>() = npis;

    ASSERT_EQ(model.parameters.get<mio::ContactPatterns>().get_cont_freq_mat()[0].get_dampings().size(), 0);

    mio::SecirSimulation<mio_test::MockSimulation> sim(model);
    sim.advance(3.0);

    ASSERT_EQ(sim.get_model().parameters.get<mio::ContactPatterns>().get_cont_freq_mat()[0].get_dampings().size(), 0);
}

TEST(DynamicNPIs, secir_threshold_exceeded)
{
    mio::SecirModel model(1);
    model.populations[{mio::AgeGroup(0), mio::InfectionState::InfectedSymptoms}] = 10;
    model.populations.set_difference_from_total({mio::AgeGroup(0), mio::InfectionState::Susceptible}, 100);

    mio::DynamicNPIs npis;
    npis.set_threshold(
        0.05 * 50'000,
        {mio::DampingSampling{
            1.0, mio::DampingLevel(0), mio::DampingType(0), mio::SimulationTime(0), {0}, Eigen::VectorXd::Ones(1)}});
    npis.set_duration(mio::SimulationTime(5.0));
    npis.set_base_value(50'000);
    model.parameters.get<mio::DynamicNPIsInfectedSymptoms>() = npis;

    ASSERT_EQ(model.parameters.get<mio::ContactPatterns>().get_cont_freq_mat()[0].get_dampings().size(), 0);

    mio::SecirSimulation<mio_test::MockSimulation> sim(model);
    sim.advance(3.0);

    ASSERT_EQ(sim.get_model().parameters.get<mio::ContactPatterns>().get_cont_freq_mat()[0].get_dampings().size(), 2);
}

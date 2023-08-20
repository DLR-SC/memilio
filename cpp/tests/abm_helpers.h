/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Daniel Abele, Elisabeth Kluth, David Kerkmann, Sascha Korf, Martin J. Kuehn, Khoa Nguyen
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
#ifndef ABM_HELPERS_H
#define ABM_HELPERS_H

#include "abm/abm.h"
#include "abm/virus_variant.h"
#include "memilio/io/io.h"
#include "memilio/io/json_serializer.h"
#include "memilio/math/eigen_util.h"
#include "memilio/epidemiology/age_group.h"
#include "matchers.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <memory>

// Assign the name to general age group.
const auto NUM_AGE_GROUPS     = 6;
const auto AGE_GROUP_0_TO_4   = mio::AgeGroup(NUM_AGE_GROUPS - 6);
const auto AGE_GROUP_5_TO_14  = mio::AgeGroup(NUM_AGE_GROUPS - 5);
const auto AGE_GROUP_15_TO_34 = mio::AgeGroup(NUM_AGE_GROUPS - 4);
const auto AGE_GROUP_35_TO_59 = mio::AgeGroup(NUM_AGE_GROUPS - 3);
const auto AGE_GROUP_60_TO_79 = mio::AgeGroup(NUM_AGE_GROUPS - 2);
const auto AGE_GROUP_80_PLUS  = mio::AgeGroup(NUM_AGE_GROUPS - 1);

/**
 * mock of the generator function of DistributionAdapter<DistT>.
 * can't be used directly as a generator function because it is not copyable.
 * see MockDistributionRef
 */
template <class DistT>
struct MockDistribution {
    using Distribution = DistT;
    // using invoke() instead of operator() because operators cant be mocked in the GMock framework */
    MOCK_METHOD(typename Distribution::ResultType, invoke, (const typename Distribution::ParamType&), ());
};

/**
 * reference wrapper of a MockDistribution object.
 * Mocks are not copyable but the generator function of a distribution must be copyable.
 * This wrapper is copyable and all copies redirect invocations to a shared underlying mock
 * so it can be used as a generator function.
 */
template <class MockDistribution>
struct MockDistributionRef {
    using Distribution = typename MockDistribution::Distribution;
    typename Distribution::ResultType operator()(const typename Distribution::ParamType& p)
    {
        return mock->invoke(p);
    }
    std::shared_ptr<MockDistribution> mock = std::make_shared<MockDistribution>();
};

/**
 * Replaces the generator function in the static instance of DistributionAdapter with a mock.
 * On construction sets the generator and on destruction restores the previous generator.
 */
template <class MockDistribution>
struct ScopedMockDistribution {
    using Distribution = typename MockDistribution::Distribution;
    /**
     * constructor replaces the generator function with a mock
     */
    ScopedMockDistribution()
    {
        old = Distribution::get_instance().get_generator();
        Distribution::get_instance().set_generator(mock_ref);
    }
    ~ScopedMockDistribution()
    {
        Distribution::get_instance().set_generator(old);
    }
    MockDistribution& get_mock()
    {
        return *mock_ref.mock;
    }

    MockDistributionRef<MockDistribution> mock_ref;
    typename Distribution::GeneratorFunction old;
};

/**
 * @brief Create a Person without a World object. Intended for simple use in tests.
*/
mio::abm::Person make_test_person(mio::abm::Location& location, mio::AgeGroup age = AGE_GROUP_15_TO_34,
                                  mio::abm::InfectionState infection_state = mio::abm::InfectionState::Susceptible,
                                  mio::abm::TimePoint t                    = mio::abm::TimePoint(0),
                                  mio::abm::Parameters params              = mio::abm::Parameters(NUM_AGE_GROUPS));

/**
 * @brief Add a Person to the World. Intended for simple use in tests.
*/
mio::abm::Person& add_test_person(mio::abm::World& world, mio::abm::LocationId loc_id,
                                  mio::AgeGroup age                        = AGE_GROUP_15_TO_34,
                                  mio::abm::InfectionState infection_state = mio::abm::InfectionState::Susceptible,
                                  mio::abm::TimePoint t                    = mio::abm::TimePoint(0));

#endif //ABM_HELPERS_H

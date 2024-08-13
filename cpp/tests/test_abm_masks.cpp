/* 
* Copyright (C) 2020-2024 MEmilio
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
#include "abm/person.h"
#include "abm_helpers.h"
#include "memilio/utils/random_number_generator.h"

TEST(TestMasks, init)
{
    auto mask = mio::abm::Mask(mio::abm::MaskType::Count);
    ASSERT_EQ(mask.get_time_used().seconds(), 0.);
}

TEST(TestMasks, getType)
{
    auto mask = mio::abm::Mask(mio::abm::MaskType::Community);
    auto type = mask.get_type();
    ASSERT_EQ(type, mio::abm::MaskType::Community);
}

TEST(TestMasks, increaseTimeUsed)
{
    auto mask = mio::abm::Mask(mio::abm::MaskType::Community);
    auto dt   = mio::abm::hours(2);
    mask.increase_time_used(dt);
    ASSERT_EQ(mask.get_time_used(), mio::abm::hours(2));
}

TEST(TestMasks, changeMask)
{
    auto mask = mio::abm::Mask(mio::abm::MaskType::Community);
    mask.increase_time_used(mio::abm::hours(2));
    ASSERT_EQ(mask.get_type(), mio::abm::MaskType::Community);
    ASSERT_EQ(mask.get_time_used(), mio::abm::hours(2));

    mask.change_mask(mio::abm::MaskType::Surgical);
    ASSERT_EQ(mask.get_type(), mio::abm::MaskType::Surgical);
    ASSERT_EQ(mask.get_time_used(), mio::abm::hours(0));
}

TEST(TestMasks, maskProtection)
{
    auto rng = mio::RandomNumberGenerator();
    mio::abm::Parameters params(num_age_groups);

    // set time for state transition to two days so that the newly infected person is still exposed
    ScopedMockDistribution<testing::StrictMock<MockDistribution<mio::LogNormalDistribution<double>>>> mock_logNorm_dist;
    EXPECT_CALL(mock_logNorm_dist.get_mock(), invoke).WillRepeatedly(testing::Return(2));

    //setup location with some chance of exposure
    auto t = mio::abm::TimePoint(0);
    mio::abm::Location infection_location(mio::abm::LocationType::School, 0, num_age_groups);
    auto susc_person1 =
        mio::abm::Person(rng, infection_location.get_type(), infection_location.get_id(), age_group_15_to_34);
    auto susc_person2 =
        mio::abm::Person(rng, infection_location.get_type(), infection_location.get_id(), age_group_15_to_34);
    auto infected1 = make_test_person(infection_location, age_group_15_to_34,
                                      mio::abm::InfectionState::InfectedSymptoms, t, params); // infected 7 days prior

    //cache precomputed results
    auto dt = mio::abm::days(1);
    // susc_person1 wears a mask, default protection is 1
    susc_person1.set_wear_mask(true);
    // susc_person2 does not wear a mask
    susc_person2.set_wear_mask(false);

    //mock so person 2 will get infected
    ScopedMockDistribution<testing::StrictMock<MockDistribution<mio::ExponentialDistribution<double>>>>
        mock_exponential_dist;

    auto p1_rng = mio::abm::PersonalRandomNumberGenerator(rng, susc_person1);
    interact_testing(p1_rng, susc_person1, infection_location, {susc_person1, susc_person2, infected1}, t, dt, params);
    EXPECT_CALL(mock_exponential_dist.get_mock(), invoke).WillOnce(testing::Return(0.5));
    auto p2_rng = mio::abm::PersonalRandomNumberGenerator(rng, susc_person2);
    interact_testing(p2_rng, susc_person2, infection_location, {susc_person1, susc_person2, infected1}, t, dt, params);

    // The person susc_person1 should have full protection against an infection, susc_person2 not
    ASSERT_EQ(susc_person1.get_infection_state(t + dt), mio::abm::InfectionState::Susceptible);
    ASSERT_EQ(susc_person2.get_infection_state(t + dt), mio::abm::InfectionState::Exposed);
}

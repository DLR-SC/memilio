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
#include "abm/location_type.h"
#include "abm/person.h"

#include "abm_helpers.h"
#include "memilio/utils/random_number_generator.h"
#include <gtest/gtest.h>

TEST(TestPerson, init)
{
    auto rng = mio::RandomNumberGenerator();

    mio::abm::Location location(mio::abm::LocationType::Work, 0, num_age_groups);
    auto t      = mio::abm::TimePoint(0);
    auto person = mio::abm::Person(rng, location, age_group_60_to_79);

    ASSERT_EQ(person.get_infection_state(t), mio::abm::InfectionState::Susceptible);
    ASSERT_EQ(person.get_location(), location);
    ASSERT_EQ(person.get_person_id(), mio::abm::INVALID_PERSON_ID);
}

TEST(TestPerson, copyPerson)
{
    auto rng             = mio::RandomNumberGenerator();
    auto location        = mio::abm::Location(mio::abm::LocationType::Work, 0, num_age_groups);
    auto t               = mio::abm::TimePoint(0);
    auto person          = mio::abm::Person(rng, location, age_group_60_to_79);
    auto copied_location = location.copy_location_without_persons(num_age_groups);
    auto copied_person   = person.copy_person(copied_location);

    ASSERT_EQ(copied_person.get_infection_state(t), mio::abm::InfectionState::Susceptible);
    ASSERT_EQ(copied_person.get_location(), copied_location);
    ASSERT_EQ(copied_person.get_person_id(), mio::abm::INVALID_PERSON_ID);
}

TEST(TestPerson, migrate)
{
    auto rng = mio::RandomNumberGenerator();

    auto t = mio::abm::TimePoint(0);
    mio::abm::Location home(mio::abm::LocationType::Home, 0, num_age_groups);
    mio::abm::Location loc1(mio::abm::LocationType::PublicTransport, 0, 6, 1);
    mio::abm::Location loc2(mio::abm::LocationType::School, 0, num_age_groups);
    mio::abm::Location loc3(mio::abm::LocationType::PublicTransport, 0, 6, 2);
    auto person = make_test_person(home, age_group_0_to_4, mio::abm::InfectionState::Recovered);
    person.migrate_to(loc1, {0});

    ASSERT_EQ(person.get_location(), loc1);
    ASSERT_EQ(loc1.get_subpopulation(t, mio::abm::InfectionState::Recovered), 1);
    ASSERT_EQ(home.get_subpopulation(t, mio::abm::InfectionState::Recovered), 0);
    ASSERT_EQ(loc1.get_cells()[0].m_persons.size(), 1u);
    ASSERT_EQ(person.get_last_transport_mode(), mio::abm::TransportMode::Unknown);

    person.migrate_to(loc2, mio::abm::TransportMode::Walking);

    ASSERT_EQ(person.get_location(), loc2);
    ASSERT_EQ(loc2.get_subpopulation(t, mio::abm::InfectionState::Recovered), 1);
    ASSERT_EQ(loc1.get_subpopulation(t, mio::abm::InfectionState::Recovered), 0);
    ASSERT_EQ(loc1.get_cells()[0].m_persons.size(), 0u);
    ASSERT_EQ(person.get_last_transport_mode(), mio::abm::TransportMode::Walking);

    person.migrate_to(loc3, mio::abm::TransportMode::Bike, {0, 1});

    ASSERT_EQ(loc3.get_cells()[0].m_persons.size(), 1u);
    ASSERT_EQ(loc3.get_cells()[1].m_persons.size(), 1u);
    ASSERT_EQ(person.get_cells().size(), 2);
    ASSERT_EQ(person.get_cells()[0], 0u);
    ASSERT_EQ(person.get_cells()[1], 1u);
    ASSERT_EQ(person.get_last_transport_mode(), mio::abm::TransportMode::Bike);
}

TEST(TestPerson, setGetAssignedLocation)
{
    auto rng = mio::RandomNumberGenerator();
    mio::abm::Location location(mio::abm::LocationType::Work, 2, num_age_groups);
    auto person = mio::abm::Person(rng, location, age_group_35_to_59);
    person.set_assigned_location(location);
    ASSERT_EQ((int)person.get_assigned_location_index(mio::abm::LocationType::Work), 2);

    person.set_assigned_location({4, mio::abm::LocationType::Work});
    ASSERT_EQ((int)person.get_assigned_location_index(mio::abm::LocationType::Work), 4);
}

TEST(TestPerson, quarantine)
{
    using testing::Return;
    auto rng         = mio::RandomNumberGenerator();
    auto test_params = mio::abm::TestParameters{1.01, 1.01}; //100% safe test

    auto infection_parameters = mio::abm::Parameters(num_age_groups);
    mio::abm::Location home(mio::abm::LocationType::Home, 0, num_age_groups);
    mio::abm::Location work(mio::abm::LocationType::Work, 0, num_age_groups);

    //setup rng mock so the person has a state transition to Recovered
    ScopedMockDistribution<testing::StrictMock<MockDistribution<mio::UniformDistribution<double>>>> mock_uniform_dist;
    EXPECT_CALL(mock_uniform_dist.get_mock(), invoke)
        .Times(testing::AtLeast(4))
        .WillOnce(testing::Return(0.6)) // workgroup
        .WillOnce(testing::Return(0.6)) // schoolgroup
        .WillOnce(testing::Return(0.6)) // goto_work_hour
        .WillOnce(testing::Return(0.6)) // goto_school_hour
        .WillRepeatedly(testing::Return(1.0)); // ViralLoad draws

    auto t_morning = mio::abm::TimePoint(0) + mio::abm::hours(7);
    auto dt        = mio::abm::hours(1);
    infection_parameters
        .get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype, age_group_35_to_59}] =
        0.5 * dt.days();
    infection_parameters.get<mio::abm::AgeGroupGotoSchool>().set_multiple({age_group_5_to_14}, true);
    infection_parameters.get<mio::abm::AgeGroupGotoWork>().set_multiple({age_group_15_to_34, age_group_35_to_59}, true);

    auto person     = make_test_person(home, age_group_35_to_59, mio::abm::InfectionState::InfectedSymptoms, t_morning,
                                       infection_parameters);
    auto rng_person = mio::abm::Person::RandomNumberGenerator(rng, person);

    person.get_tested(rng_person, t_morning, test_params);

    ASSERT_EQ(person.get_infection_state(t_morning), mio::abm::InfectionState::InfectedSymptoms);
    ASSERT_EQ(mio::abm::go_to_work(rng_person, person, t_morning, dt, infection_parameters),
              mio::abm::LocationType::Home);
    ASSERT_EQ(person.get_infection_state(t_morning + dt), mio::abm::InfectionState::Recovered);
    person.remove_quarantine();
    ASSERT_EQ(mio::abm::go_to_work(rng_person, person, t_morning, dt, infection_parameters),
              mio::abm::LocationType::Work);
}

TEST(TestPerson, get_tested)
{
    using testing::Return;
    auto rng                    = mio::RandomNumberGenerator();
    mio::abm::Parameters params = mio::abm::Parameters(num_age_groups);

    mio::abm::TimePoint t(0);
    mio::abm::Location loc(mio::abm::LocationType::Home, 0, num_age_groups);
    auto infected       = make_test_person(loc, age_group_15_to_34, mio::abm::InfectionState::InfectedSymptoms);
    auto rng_infected   = mio::abm::Person::RandomNumberGenerator(rng, infected);
    auto susceptible    = mio::abm::Person(rng, loc, age_group_15_to_34);
    auto rng_suscetible = mio::abm::Person::RandomNumberGenerator(rng, susceptible);

    auto pcr_test     = mio::abm::PCRTest();
    auto antigen_test = mio::abm::AntigenTest();

    // Test pcr test
    ScopedMockDistribution<testing::StrictMock<MockDistribution<mio::UniformDistribution<double>>>>
        mock_uniform_dist_pcr;
    EXPECT_CALL(mock_uniform_dist_pcr.get_mock(), invoke)
        .Times(6)
        .WillOnce(Return(0.4))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.95))
        .WillOnce(Return(0.6))
        .WillOnce(Return(0.999))
        .WillOnce(Return(0.8));
    ASSERT_EQ(infected.get_tested(rng_infected, t, pcr_test.get_default()), true);
    ASSERT_EQ(infected.is_in_quarantine(t, params), true);
    infected.remove_quarantine();
    ASSERT_EQ(infected.get_tested(rng_infected, t, pcr_test.get_default()), false);
    ASSERT_EQ(infected.is_in_quarantine(t, params), false);
    ASSERT_EQ(susceptible.get_tested(rng_suscetible, t, pcr_test.get_default()), false);
    ASSERT_EQ(susceptible.is_in_quarantine(t, params), false);
    ASSERT_EQ(susceptible.get_tested(rng_suscetible, t, pcr_test.get_default()), true);
    ASSERT_EQ(susceptible.is_in_quarantine(t, params), true);
    ASSERT_EQ(susceptible.get_time_of_last_test(), mio::abm::TimePoint(0));

    // Test antigen test
    ScopedMockDistribution<testing::StrictMock<MockDistribution<mio::UniformDistribution<double>>>>
        mock_uniform_dist_antigen;
    EXPECT_CALL(mock_uniform_dist_antigen.get_mock(), invoke)
        .Times(6)
        .WillOnce(Return(0.4))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.95))
        .WillOnce(Return(0.6))
        .WillOnce(Return(0.999))
        .WillOnce(Return(0.8));
    ASSERT_EQ(infected.get_tested(rng_infected, t, antigen_test.get_default()), true);
    ASSERT_EQ(infected.get_tested(rng_infected, t, antigen_test.get_default()), false);
    ASSERT_EQ(susceptible.get_tested(rng_suscetible, t, antigen_test.get_default()), false);
    ASSERT_EQ(susceptible.get_tested(rng_suscetible, t, antigen_test.get_default()), true);
    ASSERT_EQ(susceptible.get_time_of_last_test(), mio::abm::TimePoint(0));
}

TEST(TestPerson, getCells)
{
    mio::abm::Location home(mio::abm::LocationType::Home, 0, 6, 1);
    mio::abm::Location location(mio::abm::LocationType::PublicTransport, 0, 6, 2);
    auto person = make_test_person(home, age_group_15_to_34, mio::abm::InfectionState::InfectedNoSymptoms);
    home.add_person(person);
    person.migrate_to(location, {0, 1});
    ASSERT_EQ(person.get_cells().size(), 2);
}

TEST(TestPerson, interact)
{
    auto rng = mio::RandomNumberGenerator();

    // Location.interact is tested seperately in the location
    auto infection_parameters = mio::abm::Parameters(num_age_groups);
    mio::abm::Location loc(mio::abm::LocationType::Home, 0, num_age_groups);
    mio::abm::TimePoint t(0);
    auto person     = mio::abm::Person(rng, loc, age_group_15_to_34);
    auto rng_person = mio::abm::Person::RandomNumberGenerator(rng, person);
    auto dt         = mio::abm::seconds(8640); //0.1 days
    person.interact(rng_person, t, dt, infection_parameters);
    EXPECT_EQ(person.get_time_at_location(), dt);
}

TEST(TestPerson, applyMaskIntervention)
{
    auto rng = mio::RandomNumberGenerator();

    mio::abm::Location home(mio::abm::LocationType::Home, 0, num_age_groups);
    mio::abm::Location target(mio::abm::LocationType::Work, 0, num_age_groups);
    auto person = make_test_person(home);
    person.get_mask().change_mask(mio::abm::MaskType::Community);
    auto rng_person = mio::abm::Person::RandomNumberGenerator(rng, person);

    target.set_mask_requirement(false);
    person.set_compliance(mio::abm::InterventionType::Mask, 0.);
    person.is_compliant(rng_person, mio::abm::InterventionType::Mask, &target);
    ASSERT_FALSE(person.is_wearing_mask());

    person.set_compliance(mio::abm::InterventionType::Mask, 1);
    person.is_compliant(rng_person, mio::abm::InterventionType::Mask, &target);
    ASSERT_TRUE(person.is_wearing_mask());

    target.set_mask_requirement(true);
    target.set_required_mask(mio::abm::MaskType::Surgical);
    person.set_compliance(mio::abm::InterventionType::Mask, 1);
    person.is_compliant(rng_person, mio::abm::InterventionType::Mask, &target);
    ASSERT_EQ(person.get_mask().get_type(), mio::abm::MaskType::Surgical);
    ASSERT_TRUE(person.is_wearing_mask());
}

TEST(TestPerson, setWearMask)
{
    mio::abm::Location location(mio::abm::LocationType::School, 0, num_age_groups);
    auto person = make_test_person(location);

    person.set_wearing_mask(false);
    ASSERT_FALSE(person.is_wearing_mask());

    person.set_wearing_mask(true);
    ASSERT_TRUE(person.is_wearing_mask());
}

TEST(TestPerson, getMaskProtectiveFactor)
{
    mio::abm::Location location(mio::abm::LocationType::School, 0, 6);
    auto person_community = make_test_person(location);
    person_community.get_mask().change_mask(mio::abm::MaskType::Community);
    person_community.set_wearing_mask(true);
    auto person_surgical = make_test_person(location);
    person_surgical.get_mask().change_mask(mio::abm::MaskType::Surgical);
    person_surgical.set_wearing_mask(true);
    auto person_ffp2 = make_test_person(location);
    person_ffp2.get_mask().change_mask(mio::abm::MaskType::FFP2);
    person_ffp2.set_wearing_mask(true);
    auto person_without = make_test_person(location);
    person_without.set_wearing_mask(false);

    mio::abm::Parameters params                                             = mio::abm::Parameters(num_age_groups);
    params.get<mio::abm::MaskProtection>()[{mio::abm::MaskType::Community}] = 0.5;
    params.get<mio::abm::MaskProtection>()[{mio::abm::MaskType::Surgical}]  = 0.8;
    params.get<mio::abm::MaskProtection>()[{mio::abm::MaskType::FFP2}]      = 0.9;

    ASSERT_EQ(person_community.get_mask_protective_factor(params), 0.5);
    ASSERT_EQ(person_surgical.get_mask_protective_factor(params), 0.8);
    ASSERT_EQ(person_ffp2.get_mask_protective_factor(params), 0.9);
    ASSERT_EQ(person_without.get_mask_protective_factor(params), 0.);
}

TEST(TestPerson, getLatestProtection)
{
    auto rng                    = mio::RandomNumberGenerator();
    auto location               = mio::abm::Location(mio::abm::LocationType::School, 0, num_age_groups);
    auto person                 = mio::abm::Person(rng, location, age_group_15_to_34);
    auto prng                   = mio::abm::Person::RandomNumberGenerator(rng, person);
    mio::abm::Parameters params = mio::abm::Parameters(num_age_groups);

    auto t = mio::abm::TimePoint(0);
    person.add_new_vaccination(mio::abm::ExposureType::GenericVaccine, t);
    auto latest_protection = person.get_latest_protection();
    ASSERT_EQ(latest_protection.first, mio::abm::ExposureType::GenericVaccine);
    ASSERT_EQ(latest_protection.second.days(), t.days());

    t = mio::abm::TimePoint(40 * 24 * 60 * 60);
    person.add_new_infection(mio::abm::Infection(prng, static_cast<mio::abm::VirusVariant>(0), age_group_15_to_34,
                                                 params, t, mio::abm::InfectionState::Exposed));
    latest_protection = person.get_latest_protection();
    ASSERT_EQ(latest_protection.first, mio::abm::ExposureType::NaturalInfection);
    ASSERT_EQ(latest_protection.second.days(), t.days());
}

TEST(Person, rng)
{
    auto rng = mio::RandomNumberGenerator();
    mio::abm::Location loc(mio::abm::LocationType::Home, 0);
    auto p = mio::abm::Person(rng, loc, age_group_35_to_59, 13);

    ASSERT_EQ(p.get_rng_counter(), mio::Counter<uint32_t>(0));

    auto p_rng = mio::abm::Person::RandomNumberGenerator(rng, p);
    ASSERT_EQ(p_rng.get_counter(), mio::rng_totalsequence_counter<uint64_t>(13, mio::Counter<uint32_t>{0}));

    p_rng();
    ASSERT_EQ(p.get_rng_counter(), mio::Counter<uint32_t>(1));
    ASSERT_EQ(p_rng.get_counter(), mio::rng_totalsequence_counter<uint64_t>(13, mio::Counter<uint32_t>{1}));
}

TEST(TestPerson, applyTestIntervention)
{
    using testing::Return;
    auto rng = mio::RandomNumberGenerator();
    ScopedMockDistribution<testing::StrictMock<MockDistribution<mio::UniformDistribution<double>>>>
        mock_uniform_dist_antigen;
    EXPECT_CALL(mock_uniform_dist_antigen.get_mock(), invoke)
        .Times(6)
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8));

    mio::abm::Location home(mio::abm::LocationType::Home, 0, num_age_groups);
    mio::abm::Location target(mio::abm::LocationType::Work, 0, num_age_groups);
    auto person     = make_test_person(home);
    auto rng_person = mio::abm::Person::RandomNumberGenerator(rng, person);

    person.set_compliance(mio::abm::InterventionType::Testing, 1);
    ASSERT_TRUE(person.is_compliant(rng_person, mio::abm::InterventionType::Testing));

    person.set_compliance(mio::abm::InterventionType::Testing, 0.4);
    ASSERT_FALSE(person.is_compliant(rng_person, mio::abm::InterventionType::Testing));
}

TEST(TestPerson, applyIsolationIntervention)
{
    using testing::Return;
    auto rng = mio::RandomNumberGenerator();
    ScopedMockDistribution<testing::StrictMock<MockDistribution<mio::UniformDistribution<double>>>> mock_uniform_dist;
    EXPECT_CALL(mock_uniform_dist.get_mock(), invoke)
        .Times(6)
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8))
        .WillOnce(Return(0.8));

    mio::abm::Location home(mio::abm::LocationType::Home, 0, num_age_groups);
    mio::abm::Location target(mio::abm::LocationType::Work, 0, num_age_groups);
    auto person     = make_test_person(home);
    auto rng_person = mio::abm::Person::RandomNumberGenerator(rng, person);

    person.set_compliance(mio::abm::InterventionType::Isolation, 1);
    ASSERT_TRUE(person.is_compliant(rng_person, mio::abm::InterventionType::Isolation));

    person.set_compliance(mio::abm::InterventionType::Isolation, 0.4);
    ASSERT_FALSE(person.is_compliant(rng_person, mio::abm::InterventionType::Isolation));
}

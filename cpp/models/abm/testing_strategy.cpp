/*
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Elisabeth Kluth, David Kerkmann, Sascha Korf, Martin J. Kuehn, Khoa Nguyen
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

#include "abm/testing_strategy.h"
#include "memilio/utils/random_number_generator.h"

namespace mio
{
namespace abm
{

TestingCriteria::TestingCriteria(const std::vector<AgeGroup>& ages, const std::vector<InfectionState>& infection_states)
{
    for (auto age : ages) {
        m_ages.insert(static_cast<size_t>(age));
    }
    for (auto infection_state : infection_states) {
        m_infection_states.set(static_cast<size_t>(infection_state), true);
    }
}

bool TestingCriteria::operator==(const TestingCriteria& other) const
{
    return m_ages == other.m_ages && m_infection_states == other.m_infection_states;
}

void TestingCriteria::add_age_group(const AgeGroup age_group)
{
    m_ages.insert(static_cast<size_t>(age_group));
}

void TestingCriteria::remove_age_group(const AgeGroup age_group)
{
    m_ages.erase(static_cast<size_t>(age_group));
}

void TestingCriteria::add_infection_state(const InfectionState infection_state)
{
    m_infection_states.set(static_cast<size_t>(infection_state), true);
}

void TestingCriteria::remove_infection_state(const InfectionState infection_state)
{
    m_infection_states.set(static_cast<size_t>(infection_state), false);
}

bool TestingCriteria::evaluate(const Person& p, TimePoint t) const
{
    return (m_ages.empty() || m_ages.count(static_cast<size_t>(p.get_age()))) &&
           (m_infection_states.none() || m_infection_states[static_cast<size_t>(p.get_infection_state(t))]);
}

TestingScheme::TestingScheme(const TestingCriteria& testing_criteria, TimeSpan minimal_time_since_last_test,
                             TimePoint start_date, TimePoint end_date, const GenericTest& test_type, double probability)
    : m_testing_criteria(testing_criteria)
    , m_minimal_time_since_last_test(minimal_time_since_last_test)
    , m_start_date(start_date)
    , m_end_date(end_date)
    , m_test_type(test_type)
    , m_probability(probability)
{
}

bool TestingScheme::operator==(const TestingScheme& other) const
{
    return this->m_testing_criteria == other.m_testing_criteria &&
           this->m_minimal_time_since_last_test == other.m_minimal_time_since_last_test &&
           this->m_start_date == other.m_start_date && this->m_end_date == other.m_end_date &&
           this->m_test_type.get_default().sensitivity == other.m_test_type.get_default().sensitivity &&
           this->m_test_type.get_default().specificity == other.m_test_type.get_default().specificity &&
           this->m_probability == other.m_probability;
    //To be adjusted and also TestType should be static.
}

bool TestingScheme::is_active() const
{
    return m_is_active;
}

void TestingScheme::update_activity_status(TimePoint t)
{
    m_is_active = (m_start_date <= t && t <= m_end_date);
}

bool TestingScheme::run_scheme(Person::RandomNumberGenerator& rng, Person& person, TimePoint t) const
{
    if (person.get_time_since_negative_test() > m_minimal_time_since_last_test) {
        double random = UniformDistribution<double>::get_instance()(rng);
        if (random < m_probability) {
            if (m_testing_criteria.evaluate(person, t)) {
                return !person.get_tested(rng, t, m_test_type.get_default());
            }
        }
    }
    return true;
}

TestingStrategy::TestingStrategy(
    const std::unordered_map<LocationId, std::vector<TestingScheme>>& location_to_schemes_map)
    : m_location_to_schemes_map(location_to_schemes_map)
{
}

void TestingStrategy::add_testing_scheme(const LocationId& loc_id, const TestingScheme& scheme)
{
    auto& schemes_vector = m_location_to_schemes_map[loc_id];
    if (std::find(schemes_vector.begin(), schemes_vector.end(), scheme) == schemes_vector.end()) {
        schemes_vector.emplace_back(scheme);
    }
}

void TestingStrategy::add_testing_scheme(const LocationType& loc_type, const TestingScheme& scheme)
{
    auto loc_id = LocationId{INVALID_LOCATION_INDEX, loc_type};
    add_testing_scheme(loc_id, scheme);
}

void TestingStrategy::remove_testing_scheme(const LocationId& loc_id, const TestingScheme& scheme)
{
    auto& schemes_vector = m_location_to_schemes_map[loc_id];
    auto last            = std::remove(schemes_vector.begin(), schemes_vector.end(), scheme);
    schemes_vector.erase(last, schemes_vector.end());
}

void TestingStrategy::remove_testing_scheme(const LocationType& loc_type, const TestingScheme& scheme)
{
    auto loc_id = LocationId{INVALID_LOCATION_INDEX, loc_type};
    remove_testing_scheme(loc_id, scheme);
}

void TestingStrategy::update_activity_status(TimePoint t)
{
    for (auto& [_, testing_schemes] : m_location_to_schemes_map) {
        for (auto& scheme : testing_schemes) {
            scheme.update_activity_status(t);
        }
    }
}

bool TestingStrategy::run_strategy(Person::RandomNumberGenerator& rng, Person& person, const Location& location,
                                   TimePoint t)
{
    // Person who is in quarantine but not yet home should go home. Otherwise they can't because they test positive.
    if (location.get_type() == mio::abm::LocationType::Home && person.is_in_quarantine()) {
        return true;
    }

    // Combine two vectors of schemes at corresponding location and location stype
    std::vector<TestingScheme>* schemes_vector[] = {
        &m_location_to_schemes_map[LocationId{location.get_index(), location.get_type()}],
        &m_location_to_schemes_map[LocationId{INVALID_LOCATION_INDEX, location.get_type()}]};

    for (auto vec_ptr : schemes_vector) {
        if (!std::all_of(vec_ptr->begin(), vec_ptr->end(), [&rng, &person, t](TestingScheme& ts) {
                return !ts.is_active() || ts.run_scheme(rng, person, t);
            })) {
            return false;
        }
    }
    return true;
}

} // namespace abm
} // namespace mio

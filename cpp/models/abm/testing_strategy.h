/*
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
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
#ifndef EPI_ABM_TESTING_SCHEME_H
#define EPI_ABM_TESTING_SCHEME_H

#include "abm/parameters.h"
#include "abm/person.h"
#include "abm/location.h"
#include "abm/time.h"
#include <bitset>
#include <map>

namespace mio
{
namespace abm
{

/**
 * @brief TestingCriteria for TestingScheme.
 */
class TestingCriteria
{
public:
    /**
     * @brief Create a TestingCriteria.
     * @param[in] ages Vector of AgeGroup%s that are either allowed or required to be tested.
     * @param[in] locations Vector of #Location%s or #LocationType%s that are either allowed or required to be tested.
     * @param[in] infection_states Vector of #InfectionState%s that are either allowed or required to be tested.
     * An empty vector of ages/#LocationType%s/#InfectionStates% means that no condition on the corresponding property
     * is set!
     */
    TestingCriteria() = default;

    TestingCriteria(const std::vector<AgeGroup>& ages, const std::vector<InfectionState>& infection_states)

    {
        for (auto age : ages) {
            m_ages.set((size_t)age, true);
        }
        for (auto infection_state : infection_states) {
            m_infection_states.set((size_t)infection_state, true);
        }
    }

    /**
     * @brief Compares two TestingCriteria for functional equality.
     */
    bool operator==(TestingCriteria other) const
    {
        auto to_compare_ages             = this->m_ages;
        auto to_compare_infection_states = this->m_infection_states;
        return to_compare_ages == other.m_ages && to_compare_infection_states == other.m_infection_states;
    }

    /**
     * @brief Add an AgeGroup to the set of AgeGroup%s that are either allowed or required to be tested.
     * @param[in] age_group AgeGroup to be added.
     */
    void add_age_group(const AgeGroup age_group)
    {
        m_ages.set((size_t)age_group, true);
    }

    /**
     * @brief Remove an AgeGroup from the set of AgeGroup%s that are either allowed or required to be tested.
     * @param[in] age_group AgeGroup to be removed.
     */
    void remove_age_group(const AgeGroup age_group)
    {
        m_ages.set((size_t)age_group, false);
    }

    /**
     * @brief Add an #InfectionState to the set of #InfectionState%s that are either allowed or required to be tested.
     * @param[in] infection_state #InfectionState to be added.
     */
    void add_infection_state(const InfectionState infection_state)
    {
        m_infection_states.set((size_t)infection_state, true);
    }

    /**
     * @brief Remove an #InfectionState from the set of #InfectionState%s that are either allowed or required to be
     * tested.
     * @param[in] infection_state #InfectionState to be removed.
     */
    void remove_infection_state(const InfectionState infection_state)
    {
        m_infection_states.set((size_t)infection_state, false);
    }

    /**
     * @brief Check if a Person and a Location meet all the required properties to get tested.
     * @param[in] p Person to be checked.
     * @param[in] t TimePoint when to evaluate the TestingCriteria.
     */
    bool evaluate(const Person& p, TimePoint t) const
    {
        return has_requested_age(p) && has_requested_infection_state(p, t);
    }

private:
    /**
     * @brief Check if a Person has the required age to get tested.
     * @param[in] p Person to be checked.
     */
    bool has_requested_age(const Person& p) const
    {
        if (m_ages.none()) {
            return true; // no condition on the AgeGroup
        }
        return m_ages[(size_t)p.get_age()];
    }

    /**
     * @brief Check if a Person has the required InfectionState to get tested.
     * @param[in] p Person to be checked.
     * @param[in] t TimePoint when to check.
     */
    bool has_requested_infection_state(const Person& p, TimePoint t) const
    {
        if (m_infection_states.none()) {
            return true; // no condition on the InfectionState
        }
        return m_infection_states[(size_t)p.get_infection_state(t)];
    }

    std::bitset<(size_t)AgeGroup::Count>
        m_ages; ///< BitSet of #AgeGroup%s that are either allowed or required to be tested.
    std::bitset<(size_t)InfectionState::Count>
        m_infection_states; /**< BitSet of #InfectionState%s that are either allowed or required to
    be tested.*/
};

/**
 * @brief TestingScheme to regular test Person%s.
 */
class TestingScheme
{
public:
    /**
     * @brief Create a TestingScheme.
     * @param[in] testing_criteria Vector of TestingCriteria that are checked for testing.
     * @param[in] minimal_time_since_last_test TimeSpan of how often this scheme applies, i. e., when a new test is
     * performed after a Person's last test.
     * @param start_date Starting date of the scheme.
     * @param end_date Ending date of the scheme.
     * @param test_type The type of test to be performed.
     * @param probability Probability of the test to be performed if a testing rule applies.
     */
    TestingScheme(const TestingCriteria& testing_criteria, TimeSpan minimal_time_since_last_test, TimePoint start_date,
                  TimePoint end_date, const GenericTest& test_type, ScalarType probability);

    /**
     * @brief Compares two TestingScheme%s for functional equality.
     */
    bool operator==(const TestingScheme& other) const;

    /**
     * @brief Get the activity status of the scheme.
     * @return Whether the TestingScheme is currently active.
     */
    bool is_active() const;

    /**
     * @brief Checks if the scheme is active at a given time and updates activity status.
     * @param[in] t TimePoint to be updated at.
     */
    void update_activity_status(TimePoint t);

    /**
     * @brief Runs the TestingScheme and potentially tests a Person.
     * @param[in] person Person to check.
     * @param[in] t TimePoint when to run the scheme.
     * @return If the person is allowed to enter the Location by the scheme.
     */
    bool run_scheme(Person& person, TimePoint t) const;

private:
    TestingCriteria m_testing_criteria; ///< Vector with all TestingCriteria of the scheme.
    TimeSpan m_minimal_time_since_last_test; ///< Shortest period of time between two tests.
    TimePoint m_start_date; ///< Starting date of the scheme.
    TimePoint m_end_date; ///< Ending date of the scheme.
    GenericTest m_test_type; ///< Type of the test.
    ScalarType m_probability; ///< Probability of performing the test.
    bool m_is_active = false; ///< Whether the scheme is currently active.
};

/**
 * @brief Set of TestingSchemes that are checked for testing.
 */
class TestingStrategy
{
public:
    /**
     * @brief Create a TestingStrategy.
     * @param[in] testing_schemes Vector of TestingSchemes that are checked for testing.
     */
    TestingStrategy() = default;
    explicit TestingStrategy(const std::map<LocationId, std::vector<TestingScheme>>& location_to_schemes_map);

    /**
     * @brief Add a TestingScheme to the set of schemes that are checked for testing at a certain Location.
     * @param[in] loc_id LocationId key for TestingScheme to be added.
     * @param[in] scheme TestingScheme to be added.
     */
    void add_testing_scheme(const LocationId& loc_id, const TestingScheme& scheme);

    /**
     * @brief Add a TestingScheme to the set of schemes that are checked for testing at a certain LocationType.
     * A TestingScheme applies to all Location of the same type is store in 
     * LocationId{INVALID_LOCATION_INDEX, location_type} of m_location_to_schemes_map.
     * @param[in] loc_type LocationId key for TestingScheme to be added.
     * @param[in] scheme TestingScheme to be added.
     */
    void add_testing_scheme(const LocationType& loc_type, const TestingScheme& scheme);

    /**
     * @brief Remove a TestingScheme from the set of schemes that are checked for testing at a certain Location.
     * @param[in] loc_id LocationId key for TestingScheme to be remove.
     * @param[in] scheme TestingScheme to be removed.
     */
    void remove_testing_scheme(const LocationId& loc_id, const TestingScheme& scheme);

    /**
     * @brief Remove a TestingScheme from the set of schemes that are checked for testing at a certain Location.
     * A TestingScheme applies to all Location of the same type is store in 
     * LocationId{INVALID_LOCATION_INDEX, location_type} of m_location_to_schemes_map.
     * @param[in] loc_type LocationType key for TestingScheme to be remove.
     * @param[in] scheme TestingScheme to be removed.
     */
    void remove_testing_scheme(const LocationType& loc_type, const TestingScheme& scheme);

    /**
     * @brief Checks if the given TimePoint is within the interval of start and end date of each TestingScheme and then
     * changes the activity status for each TestingScheme accordingly.
     * @param t TimePoint to check the activity status of each TestingScheme.
     */
    void update_activity_status(const TimePoint t);

    /**
     * @brief Runs the TestingStrategy and potentially tests a Person.
     * @param[in] person Person to check.
     * @param[in] location Location to check.
     * @param[in] t TimePoint when to run the strategy.
     * @return If the Person is allowed to enter the Location.
     */
    bool run_strategy(Person& person, const Location& location, TimePoint t);

private:
    std::map<LocationId, std::vector<TestingScheme>>
        m_location_to_schemes_map; ///< Set of schemes that are checked for testing.
};

} // namespace abm
} // namespace mio

#endif

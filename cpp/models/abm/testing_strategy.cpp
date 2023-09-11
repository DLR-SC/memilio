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

#include "abm/testing_strategy.h"
#include "memilio/utils/random_number_generator.h"

namespace mio
{
namespace abm
{

TestingScheme::TestingScheme(const TestingCriteria<LocationType>& testing_criteria,
                             TimeSpan minimal_time_since_last_test, TimePoint start_date, TimePoint end_date,
                             const GenericTest& test_type, double probability)
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

bool TestingScheme::run_scheme(Person& person, const Location& location, TimePoint t) const
{
    if (person.get_time_since_negative_test() > m_minimal_time_since_last_test) {
        double random = UniformDistribution<double>::get_instance()();
        if (random < m_probability) {
            if (m_testing_criteria.evaluate(person, location, t)) {
                return !person.get_tested(t, m_test_type.get_default());
            }
        }
    }
    return true;
}

TestingStrategy::TestingStrategy(const std::vector<TestingScheme>& testing_schemes)
    : m_testing_schemes(testing_schemes)
{
}

void TestingStrategy::add_testing_scheme(const TestingScheme& scheme)
{
    if (std::find(m_testing_schemes.begin(), m_testing_schemes.end(), scheme) == m_testing_schemes.end()) {
        m_testing_schemes.push_back(scheme);
    }
}

void TestingStrategy::remove_testing_scheme(const TestingScheme& scheme)
{
    auto last = std::remove(m_testing_schemes.begin(), m_testing_schemes.end(), scheme);
    m_testing_schemes.erase(last, m_testing_schemes.end());
}

void TestingStrategy::update_activity_status(TimePoint t)
{
    for (auto& ts : m_testing_schemes) {
        ts.update_activity_status(t);
    }
}

bool TestingStrategy::run_strategy(Person& person, const Location& location, TimePoint t) const
{
    // Person who is in quarantine but not yet home should go home. Otherwise they can't because they test positive.
    if (location.get_type() == mio::abm::LocationType::Home && person.is_in_quarantine()) {
        return true;
    }
    return std::all_of(m_testing_schemes.begin(), m_testing_schemes.end(), [&person, location, t](TestingScheme ts) {
        if (ts.is_active()) {
            return ts.run_scheme(person, location, t);
        }
        return true;
    });
}

} // namespace abm
} // namespace mio

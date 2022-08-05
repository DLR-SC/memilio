/*
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*        & Helmholtz Centre for Infection Research (HZI)
*
* Authors: David Kerkmann
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

#include "abm/testing_scheme.h"
#include "memilio/utils/random_number_generator.h"

namespace mio
{
namespace abm
{

TestingScheme::TestingScheme(const std::vector<TestingRule> testing_rules, const TimeSpan testing_frequency,
                             TimePoint start_date, TimePoint end_date, const double probability,
                             const GenericTest& test_type)
    : m_testing_rules(testing_rules)
    , m_testing_frequency(testing_frequency)
    , m_probability(probability)
    , m_start_date(start_date)
    , m_end_date(end_date)
    , m_test_type(test_type)
{
}

void TestingScheme::add_testing_rule(const TestingRule rule)
{
    m_testing_rules.push_back(rule);
    auto last = std::unique(m_testing_rules.begin(), m_testing_rules.end());
    m_testing_rules.erase(last, m_testing_rules.end());
}

void TestingScheme::remove_testing_rule(const TestingRule rule)
{
    auto last = std::remove(m_testing_rules.begin(), m_testing_rules.end(), rule);
    m_testing_rules.erase(last, m_testing_rules.end());
}

bool TestingScheme::is_active() const
{
    return m_is_active;
}
void TestingScheme::update_activity_status(const TimePoint t)
{
    m_is_active = (m_start_date <= t && t <= m_end_date);
}

bool TestingScheme::run_scheme(Person& person, const Location& location) const
{
    if (person.get_time_since_negative_test() > m_testing_frequency) {
        double random = UniformDistribution<double>::get_instance()();
        if (random < m_probability) {
            if (std::any_of(m_testing_rules.begin(), m_testing_rules.end(), [person, location](TestingRule tr) {
                    return tr.evaluate(person, location);
                })) {
                return !person.get_tested(m_test_type.get_default());
            }
        }
    }
    return true;
}

} // namespace abm
} // namespace mio

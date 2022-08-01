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
#ifndef EPI_ABM_TESTING_SCHEME_H
#define EPI_ABM_TESTING_SCHEME_H

#include "abm/parameters.h"
#include "abm/testing_rule.h"

namespace mio
{
namespace abm
{

/**
 * Testing Scheme to regular test people
 */
class TestingScheme
{
public:
    /**
     * create a testing scheme.
     * @param interval the interval in which people who go to the location get tested
     * @param probability probability with which a person gets tested
     */
    TestingScheme(const std::vector<TestingRule> testing_rules = {}, const TimeSpan interval = TimeSpan(), const double probability = 1);
    
    bool operator==(const TestingScheme& other) const
    {
        return this == &other; // compare pointers. Still possible to clone Rules.
    }
    /**
     * get the time interval of this testing scheme
     */
    const TimeSpan& get_interval() const;

    /**
     * get probability of this testing scheme
     */
    double get_probability() const;
    
    /**
     * set the time interval of this testing scheme
     */
    void set_interval(TimeSpan t);

    /**
     * set probability of this testing scheme
     */
    void set_probability(double p);

    void add_testing_rule(const TestingRule rule);
    void remove_testing_rule(const TestingRule rule);
    const std::vector<TestingRule>& get_testing_rules() const;
    void set_testing_rules(const std::vector<TestingRule> testing_rules);
    
    const TimePoint& get_start_date() const;
    const TimePoint& get_end_date() const;
    const TimeSpan get_duration() const;
    bool isActive(const TimePoint t) const;
    /**
     * runs the testing scheme and tests a person if necessary
     * @return if the person is allowed to enter the location
     */
    bool run_scheme(Person& person, const Location& location, const GlobalTestingParameters& params) const;

private:
    std::vector<TestingRule> m_testing_rules;
    TimeSpan m_time_interval;
    TimePoint m_start_date;
    TimePoint m_end_date;
    double m_probability;
    
};

} // namespace abm
} // namespace mio

#endif

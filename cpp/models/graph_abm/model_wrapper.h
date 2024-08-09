/*
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Julia Bicker
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

#ifndef MIO_ABM_MODEL_WRAPPER_H
#define MIO_ABM_MODEL_WRAPPER_H

#include "abm/model.h"
#include "abm/time.h"
#include "abm/location_id.h"
#include "memilio/utils/compiler_diagnostics.h"
#include "memilio/utils/logging.h"
#include "memilio/utils/mioomp.h"
#include "abm/mobility_rules.h"
#include "abm/mobility_rules.h"
#include <list>

namespace mio
{
using namespace abm;
class ModelWrapper : public abm::Model
{
    using Model::Model;
    using Base = Model;

public:
    /**
     * @brief Get person buffer. 
     */
    std::vector<size_t>& get_person_buffer()
    {
        return m_person_buffer;
    }

    /** 
    * @brief Removes person from the model.
    * @param[in] pos Index of person in m_persons vector.
    */
    void remove_person(size_t pos)
    {
        Base::m_persons.erase(Base::m_persons.begin() + pos);
        Base::m_activeness_statuses.erase(Base::m_activeness_statuses.begin() + pos);
    }

    void evolve(TimePoint t, TimeSpan dt)
    {
        Base::begin_step(t, dt);
        log_info("Graph ABM Model interaction.");
        Base::interaction(t, dt);
        log_info("Graph ABM Model mobility.");
        perform_mobility(t, dt);
    }

private:
    void perform_mobility(TimePoint t, TimeSpan dt)
    {
        log_warning("perform_mobility");
        const uint32_t num_persons = static_cast<uint32_t>(Base::m_persons.size());
    PRAGMA_OMP(parallel for)
    for (uint32_t person_id = 0; person_id < num_persons; ++person_id) {
        if (Base::m_activeness_statuses[person_id]) {
            Person& person    = Base::m_persons[person_id];
            auto personal_rng = PersonalRandomNumberGenerator(Base::m_rng, person);

            auto try_mobility_rule = [&](auto rule) -> bool {
                //run mobility rule and check if change of location can actually happen
                auto target_type = rule(personal_rng, person, t, dt, parameters);
                if (person.get_assigned_location_model_id(target_type) == Base::m_id) {
                    const Location& target_location   = Base::get_location(Base::find_location(target_type, person_id));
                    const LocationId current_location = person.get_location();
                    if (Base::m_testing_strategy.run_strategy(personal_rng, person, target_location, t)) {
                        if (target_location.get_id() != current_location &&
                            Base::get_number_persons(target_location.get_id()) <
                                target_location.get_capacity().persons) {
                            bool wears_mask = person.apply_mask_intervention(personal_rng, target_location);
                            if (wears_mask) {
                                Base::change_location(person_id, target_location.get_id());
                            }
                            return true;
                        }
                    }
                }
                else { //person moves to other world
                    Base::m_activeness_statuses[person_id] = false;
                    person.set_location(target_type, abm::LocationId::invalid_id(), std::numeric_limits<int>::max());
                    m_person_buffer.push_back(person_id);
                    m_are_exposure_caches_valid       = false;
                    m_is_local_population_cache_valid = false;
                    return true;
                }
                return false;
            };

            //run mobility rules one after the other if the corresponding location type exists
            //shortcutting of bool operators ensures the rules stop after the first rule is applied
            if (Base::m_use_mobility_rules) {
                try_mobility_rule(&get_buried) || try_mobility_rule(&return_home_when_recovered) ||
                    try_mobility_rule(&go_to_hospital) || try_mobility_rule(&go_to_icu) ||
                    try_mobility_rule(&go_to_school) || try_mobility_rule(&go_to_work) ||
                    try_mobility_rule(&go_to_shop) || try_mobility_rule(&go_to_event) ||
                    try_mobility_rule(&go_to_quarantine);
            }
            else {
                //no daily routine mobility, just infection related
                try_mobility_rule(&get_buried) || try_mobility_rule(&return_home_when_recovered) ||
                    try_mobility_rule(&go_to_hospital) || try_mobility_rule(&go_to_icu) ||
                    try_mobility_rule(&go_to_quarantine);
            }
        }
    }

    log_warning("begin trips");

    // check if a person makes a trip
    bool weekend     = t.is_weekend();
    size_t num_trips = Base::m_trip_list.num_trips(weekend);

    if (num_trips != 0) {
        while (Base::m_trip_list.get_current_index() < num_trips &&
               Base::m_trip_list.get_next_trip_time(weekend).seconds() < (t + dt).time_since_midnight().seconds()) {
            auto& trip = Base::m_trip_list.get_next_trip(weekend);
            log_warning("get_index");
            auto person_index = Base::get_person_index(trip.person_id);
            log_warning("get_index end");
            auto& person = Base::get_person(person_index);
            log_warning("get_person end");
            auto personal_rng = PersonalRandomNumberGenerator(Base::m_rng, person);
            if (!person.is_in_quarantine(t, parameters) && person.get_infection_state(t) != InfectionState::Dead) {
                log_warning("if id is dest model id");
                if (trip.destination_model_id == Base::m_id) {
                    log_warning("target loc");
                    auto& target_location = Base::get_location(trip.destination);
                    log_warning("target loc end");
                    if (Base::m_testing_strategy.run_strategy(personal_rng, person, target_location, t)) {
                        person.apply_mask_intervention(personal_rng, target_location);
                        log_warning("change loc");
                        Base::change_location(person_index, target_location.get_id(), trip.trip_mode);
                        log_warning("change loc end");
                    }
                }
                else {
                    //person moves to other world
                    Base::m_activeness_statuses[person_index] = false;
                    person.set_location(trip.destination_type, abm::LocationId::invalid_id(),
                                        std::numeric_limits<int>::max());
                    m_person_buffer.push_back(person_index);
                    m_are_exposure_caches_valid       = false;
                    m_is_local_population_cache_valid = false;
                }
            }
            Base::m_trip_list.increase_index();
        }
    }
    if (((t).days() < std::floor((t + dt).days()))) {
        Base::m_trip_list.reset_index();
    }
    }

    std::vector<size_t> m_person_buffer; ///< List with indices of persons that are deactivated per target world.
};
} // namespace mio

#endif //MIO_ABM_MODEL_WRAPPER_H
/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*        & Helmholtz Centre for Infection Research (HZI)
*
* Authors: Daniel Abele, Majid Abedi, Elisabeth Kluth, David Kerkmann, Sascha Korf, Martin J. Kuehn, Khoa Nguyen
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
#ifndef EPI_ABM_WORLD_H
#define EPI_ABM_WORLD_H

#include "abm/parameters.h"
#include "abm/location.h"
#include "abm/person.h"
#include "abm/lockdown_rules.h"
#include "abm/trip_list.h"
#include "abm/testing_strategy.h"
#include "memilio/utils/pointer_dereferencing_iterator.h"
#include "memilio/utils/stl_util.h"

#include <vector>
#include <memory>

namespace mio
{
namespace abm
{

/**
 * @brief The World of the Simulation.
 * It consists of Location%s and Person%s (Agents).
 */
class World
{
public:
    using LocationIterator      = PointerDereferencingIterator<std::vector<std::shared_ptr<Location>>::iterator>;
    using ConstLocationIterator = PointerDereferencingIterator<std::vector<std::shared_ptr<Location>>::const_iterator>;
    using PersonIterator        = PointerDereferencingIterator<std::vector<std::shared_ptr<Person>>::iterator>;
    using ConstPersonIterator   = PointerDereferencingIterator<std::vector<std::shared_ptr<Person>>::const_iterator>;

    /**
     * @brief Create a World.
     * @param[in] params Parameters of the Infection that are the same everywhere in the World.
     */
    World(const GlobalInfectionParameters& params = {})
        : m_locations((uint32_t)LocationType::Count)
        , m_infection_parameters(params)
        , m_migration_parameters()
        , m_trip_list()
    {
        use_migration_rules(true);
    }

    //type is move-only for stable references of persons/locations
    World(World&& other) = default;
    World& operator=(World&& other) = default;
    World(const World&)             = delete;
    World& operator=(const World&) = delete;

    /** 
     * Prepare the World for the next simulation step.
     * @param[in] t Current time.
     * @param[in] dt Length of the time step.
     */
    void begin_step(TimePoint t, TimeSpan dt);

    /** 
     * Follow up on the World after the simulation step.
     * @param[in] t Current time.
     * @param[in] dt Length of the time step.
     */
    void end_step(TimePoint t, TimeSpan dt);

    /** 
     * Evolve the world one time step.
     * @param[in] t Current time.
     * @param[in] dt Length of the time step.
     */
    void evolve(TimePoint t, TimeSpan dt);

    /** 
     * Add a Location to the world.
     * @param[in] type Type of Location to add.
     * @param[in] num_cells [Default: 1] Number of Cell%s that the Location is divided into.
     * @return Index and type of the newly created Location.
     */
    LocationId add_location(LocationType type, uint32_t num_cells = 1);

    /** 
     * @brief Add a Person to the world.
     * @param[in] id Index and type of the initial Location of the Person.
     * @param[in] age AgeGroup of the person.
     * @return Reference to the newly created Person.
     */
    Person& add_person(const LocationId id, AgeGroup age);

    /**
     * @brief Get a range of all Location%s in the World.
     * @return A range of all Location%s.
     */
    Range<std::pair<std::vector<std::vector<std::shared_ptr<Location>>>::const_iterator,
                    std::vector<std::vector<std::shared_ptr<Location>>>::const_iterator>>
    get_locations() const;

    /**
     * @brief Get a range of all Person%s in the World.
     * @return A range of all Person%s.
     */
    Range<std::pair<ConstPersonIterator, ConstPersonIterator>> get_persons() const;

    /**
     * @brief Get an individualized Location.
     * @param[in] id LocationId of the Location.
     * @return Reference to the Location.
     */
    const Location& get_individualized_location(LocationId id) const;

    Location& get_individualized_location(LocationId id);

    /**
     * @brief Get the current Location of a Person.
     * @param[in] person The Person.
     * @return Reference to the current Location of the Person.
     */
    const Location& get_location(const Person& person) const;

    Location& get_location(const Person& person);

    /**
     * @brief Find an assigned Location of a Person.
     * @param[in] type The #LocationType that specifies the assigned Location.
     * @param[in] person The Person.
     * @return Pointer to the assigned Location.
     */
    Location& find_location(LocationType type, const Person& person);

    /** 
     * @brief Get the number of Persons in one #InfectionState at all Location%s of a type.
     * @param[in] s Specified #InfectionState.
     * @param[in] type Specified #LocationType.
     */
    int get_subpopulation_combined(TimePoint t, InfectionState s, LocationType type) const;

    /** 
     * @brief Get the MigrationParameters.
     * @return Reference to the MigrationParameters.
     */
    MigrationParameters& get_migration_parameters();

    const MigrationParameters& get_migration_parameters() const;

    /** 
     * @brief Get the GlobalInfectionParameters.
     * @return Reference to the GlobalInfectionParameters.
     */
    GlobalInfectionParameters& get_global_infection_parameters();

    const GlobalInfectionParameters& get_global_infection_parameters() const;

    /**
     * @brief Get the migration data.
     * @return Reference to the list of Trip%s that the Person%s make.
     */
    TripList& get_trip_list();

    const TripList& get_trip_list() const;

    /** 
     * @brief Decide if migration rules (like go to school/work) are used or not;
     * The migration rules regarding hospitalization/ICU/quarantine are always used.
     * @param[in] param If true uses the migration rules for migration to school/work etc., else only the rules 
     * regarding hospitalization/ICU/quarantine.
     */
    void use_migration_rules(bool param);
    bool use_migration_rules() const;

    /** 
     * @brief Get the TestingStrategy.
     * @return Refernce to the list of TestingSchemes that are checked for testing.
     */
    TestingStrategy& get_testing_strategy();

    const TestingStrategy& get_testing_strategy() const;

private:
    /**
     * @brief Person%s interact at their Location and may become infected.
     * @param[in] t The current TimePoint.
     * @param[in] dt The length of the time step of the Simulation.
     */
    void interaction(TimePoint t, TimeSpan dt);
    /**
     * @brief Person%s move in the World according to rules.
     * @param[in] t The current TimePoint.
     * @param[in] dt The length of the time step of the Simulation.
     */
    void migration(TimePoint t, TimeSpan dt);

    std::vector<std::shared_ptr<Person>> m_persons;
    std::vector<std::vector<std::shared_ptr<Location>>> m_locations;
    TestingStrategy m_testing_strategy;
    GlobalInfectionParameters m_infection_parameters;
    MigrationParameters m_migration_parameters;
    TripList m_trip_list;
    bool m_use_migration_rules;
    std::vector<std::pair<LocationType (*)(const Person&, TimePoint, TimeSpan, const MigrationParameters&),
                          std::vector<LocationType>>>
        m_migration_rules;
};

} // namespace abm
} // namespace mio

#endif

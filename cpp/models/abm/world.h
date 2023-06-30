/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*        & Helmholtz Centre for Infection Research (HZI)
*
* Authors: Daniel Abele, Majid Abedi, Elisabeth Kluth, David Kerkmann, Sascha Korf, Martin J. Kuehn
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
#include "abm/lockdown_rules.h" // IWYU pragma: keep
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
template<typename FP=double>
class World
{
public:
    using LocationIterator      = PointerDereferencingIterator<typename std::vector<std::unique_ptr<Location<FP>>>::iterator>;
    using ConstLocationIterator = PointerDereferencingIterator<typename std::vector<std::unique_ptr<Location<FP>>>::const_iterator>;
    using PersonIterator        = PointerDereferencingIterator<typename std::vector<std::unique_ptr<Person<FP>>>::iterator>;
    using ConstPersonIterator   = PointerDereferencingIterator<typename std::vector<std::unique_ptr<Person<FP>>>::const_iterator>;

    /**
     * @brief Create a World.
     * @param[in] params Parameters of the Infection that are the same everywhere in the World.
     */
    World(const GlobalInfectionParameters<FP>& params = {})
        : m_infection_parameters(params)
        , m_migration_parameters()
        , m_trip_list()
    {
        use_migration_rules(true);
    }

    //type is move-only for stable references of persons/locations
    World(World&& other)            = default;
    World& operator=(World&& other) = default;
    World(const World&)             = delete;
    World& operator=(const World&)  = delete;

    /** 
     * @brief Prepare the World for the next Simulation step.
     * @param[in] t Current time.
     * @param[in] dt Length of the time step.
     */
    void begin_step(TimePoint t, TimeSpan dt);

    /** 
     * @brief Follow up on the World after the Simulation step.
     * @param[in] t Current time.
     * @param[in] dt Length of the time step.
     */
    void end_step(TimePoint t, TimeSpan dt);

    /** 
     * @brief Evolve the world one time step.
     * @param[in] t Current time.
     * @param[in] dt Length of the time step.
     */
    void evolve(TimePoint t, TimeSpan dt);

    /** 
     * @brief Add a Location to the World.
     * @param[in] type Type of Location to add.
     * @param[in] num_cells [Default: 1] Number of Cell%s that the Location is divided into.
     * @return Index and type of the newly created Location.
     */
    LocationId add_location(LocationType type, uint32_t num_cells = 1);

    /** 
     * @brief Add a Person to the World.
     * @param[in] id Index and type of the initial Location of the Person.
     * @param[in] age AgeGroup of the person.
     * @return Reference to the newly created Person.
     */
    Person<FP>& add_person(const LocationId id, AgeGroup age);

    /**
     * @brief Get a range of all Location%s in the World.
     * @return A range of all Location%s.
     */
    Range<std::pair<ConstLocationIterator, ConstLocationIterator>> get_locations() const;

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
    const Location<FP>& get_individualized_location(LocationId id) const;

    Location<FP>& get_individualized_location(LocationId id);

    /**
     * @brief Find an assigned Location of a Person.
     * @param[in] type The #LocationType that specifies the assigned Location.
     * @param[in] person The Person.
     * @return Reference to the assigned Location.
     */
    Location<FP>& find_location(LocationType type, const Person& person);

    /** 
     * @brief Get the number of Persons in one #InfectionState at all Location%s of a type.
     * @param[in] s Specified #InfectionState.
     * @param[in] type Specified #LocationType.
     */
    size_t get_subpopulation_combined(TimePoint t, InfectionState s, LocationType type) const;

    /** 
     * @brief Get the MigrationParameters.
     * @return Reference to the MigrationParameters.
     */
    MigrationParameters<FP>& get_migration_parameters();

    const MigrationParameters<FP>& get_migration_parameters() const;

    /** 
     * @brief Get the GlobalInfectionParameters.
     * @return Reference to the GlobalInfectionParameters.
     */
    GlobalInfectionParameters<FP>& get_global_infection_parameters();

    const GlobalInfectionParameters<FP>& get_global_infection_parameters() const;

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
     * @return Reference to the list of TestingScheme%s that are checked for testing.
     */
    TestingStrategy<FP>& get_testing_strategy();

    const TestingStrategy<FP>& get_testing_strategy() const;

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

    std::vector<std::unique_ptr<Person<FP>>> m_persons; ///< Vector with pointers to every Person.
    std::vector<std::unique_ptr<Location<FP>>> m_locations; ///< Vector with pointers to every Location.
    TestingStrategy<FP> m_testing_strategy; ///< List of TestingScheme%s that are checked for testing.
    GlobalInfectionParameters<FP> m_infection_parameters; /** Parameters of the Infection that are the same everywhere in
    the World.*/
    MigrationParameters<FP> m_migration_parameters; ///< Parameters that describe the migration between Location%s.
    TripList m_trip_list; ///< List of all Trip%s the Person%s do.
    bool m_use_migration_rules; ///< Whether migration rules are considered.
    std::vector<std::pair<LocationType (*)(const Person<FP>&, TimePoint, TimeSpan,
                                           const MigrationParameters<FP>&),
                          std::vector<LocationType>>>
        m_migration_rules; ///< Rules that govern the migration between Location%s.
};

} // namespace abm
} // namespace mio

#endif

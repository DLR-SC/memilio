/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*
* Authors: Daniel Abele, Elisabeth Kluth, Khoa Nguyen, David Kerkmann
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
#ifndef EPI_ABM_LOCATION_H
#define EPI_ABM_LOCATION_H

#include "abm/person.h"
#include "abm/mask_type.h"
#include "abm/parameters.h"
#include "abm/location_type.h"
#include "abm/infection_state.h"
#include "abm/vaccine.h"

#include "memilio/math/eigen.h"
#include "memilio/utils/custom_index_array.h"
#include "memilio/utils/time_series.h"
#include "memilio/utils/memory.h"
#include <array>
#include <random>

namespace mio
{
namespace abm
{
class Person;

/**
 * CellCapacity describes the size of a cell. 
 * It consists of a volume and a capacity in persons which is an upper bound for the number
 * of people that can be in the cell at the same time.
 */
struct CellCapacity {
    CellCapacity()
        : volume(0)
        , persons(std::numeric_limits<int>::max())
    {
    }
    uint32_t volume;
    uint32_t persons;
};

/**
 * LocationId identifies a Location uniquely. It consists of the LocationType of the Location and an Index.
 * The index corresponds to the index into the structure m_locations from world, where all Locations are saved.
 */
struct LocationId {
    uint32_t index;
    LocationType type;

    bool operator==(const LocationId& rhs) const
    {
        return (index == rhs.index && type == rhs.type);
    }

    bool operator!=(const LocationId& rhs) const
    {
        return !(index == rhs.index && type == rhs.type);
    }
};

/**
 * The Location can be split up into several Cell%s. This allows a finer division of the people at the Location.
 */
struct Cell {
    std::vector<observer_ptr<Person>> m_persons;
    CustomIndexArray<ScalarType, VirusVariant, AgeGroup> m_cached_exposure_rate_contacts;
    CustomIndexArray<ScalarType, VirusVariant> m_cached_exposure_rate_air;
    CellCapacity m_capacity;

    explicit Cell(std::vector<observer_ptr<Person>> persons = {})
        : m_persons(std::move(persons))
        , m_cached_exposure_rate_contacts({{VirusVariant::Count, AgeGroup::Count}, 0.})
        , m_cached_exposure_rate_air({{VirusVariant::Count}, 0.})
        , m_capacity()
    {
    }

    /**
    * @brief Computes a relative cell size for the Cell.
    * @return The relative cell size for the Cell.
    */
    ScalarType compute_space_per_person_relative();

    /**
    * @brief Get subpopulation of a particular InfectionState in the Cell.
    * @param[in] t TimePoint of querry.
    * @param[in] state InfectionState of interest.
    * @return Amount of persons of the InfectionState in the Cell.
    */
    uint32_t get_subpopulation(TimePoint t, InfectionState state) const;

}; // namespace mio

/**
 * All locations in the simulated world where persons gather.
 */
class Location
{
public:
    /**
     * Construct a Location of a certain type.
     * @param type The type of the location.
     * @param index The index of the location.
     * @param num_cells [Default: 1] The number of cells in which the Location is divided.
     */
    Location(LocationType type, uint32_t index, uint32_t num_cells = 1);

    /**
    * @brief Compare two Location%s.
    */
    bool operator==(const Location& other) const
    {
        return (m_type == other.m_type && m_index == other.m_index);
    }

    bool operator!=(const Location& other) const
    {
        return !(*this == other);
    }

    /**
     * @brief Get the LocationType of this Location.
     */
    LocationType get_type() const
    {
        return m_type;
    }

    /**
     * @brief Get the index of this Location.
     */
    unsigned get_index() const
    {
        return m_index;
    }

    /**
     * @brief Compute the transmission factor for contact transmission of the virus in a Cell.
     * @param[in] cell_index Cell index of the Cell.
     * @param[in] virus VirusVariant of interest.
     * @param[in] age_receiver AgeGroup of the receiving Person.
     * @param[in] age_transmitter AgeGroup of the transmitting Person.
     * @returns Amount of average infections with the virus from the AgeGroup of the transmitter per day.
    */
    ScalarType transmission_contacts_per_day(uint32_t cell_index, VirusVariant virus, AgeGroup age_receiver,
                                             AgeGroup age_transmitter) const;

    /**
     * @brief Compute the transmission factor for a aerosol transmission of the virus in a Cell.
     * @param[in] cell_index Cell index of the Cell.
     * @param[in] virus VirusVariant of interest.
     * @returns Amount of average infections with the virus per day.
    */
    ScalarType transmission_air_per_day(uint32_t cell_index, VirusVariant virus) const;

    /** 
     * @brief A person interacts with the population at this Location and may become infected.
     * @param person The Person that interacts with the population.
     * @param dt Length of the current simulation time step.
     * @param global_params Global infection parameters.
     */
    void interact(Person& person, TimePoint t, TimeSpan dt, GlobalInfectionParameters& global_params) const;

    /** 
     * @brief Add a Person to the population at this Location.
     * @param person The Person arriving.
     * @param cell_idx [Default: 0] Index of the Cell the Person shall go to.
    */
    void add_person(Person& person, std::vector<uint32_t> cells = {0});

    /** 
     * @brief Remove a Person from the population of this Location.
     * @param person The Person leaving.
     */
    void remove_person(Person& person);

    /** 
     * @brief Prepare the location for the next simulation step.
     * @param t Current TimePoint of the simulation.
     * @param dt The duration of the simulation step.
     */
    void cache_exposure_rates(TimePoint t, TimeSpan dt);

    /**
     * @return parameters of the infection that are specific to this location
     */
    LocalInfectionParameters& get_infection_parameters()
    {
        return m_parameters;
    }

    const LocalInfectionParameters& get_infection_parameters() const
    {
        return m_parameters;
    }

    /**
     * @brief Get the Cell%s of this Location.
     */
    const std::vector<Cell>& get_cells() const
    {
        return m_cells;
    }

    /**
     * @brief Get the type of Mask that is demanded when entering this Location.
     */
    MaskType get_required_mask() const
    {
        return m_required_mask;
    }

    /**
     * @brief Set the required MaskType for entering this Location.
     * @param[in] type The type of the Mask.
     */
    void set_required_mask(MaskType type)
    {
        m_required_mask = type;
    }

    /**
     * @brief Get the contact exposure rate in the cell.
     * @param[in] cell_idx CellIndex of interest.
     * @return Air exposure rate in the cell.
     */
    CustomIndexArray<ScalarType, VirusVariant, AgeGroup> get_cached_exposure_rate_contacts(uint32_t cell_idx)
    {
        return m_cells[cell_idx].m_cached_exposure_rate_contacts;
    }

    /**
     * @brief Get the air exposure rate in the cell.
     * @param[in] cell_idx CellIndex of interest.
     * @return Contact exposure rate in the cell.
     */
    CustomIndexArray<ScalarType, VirusVariant> get_cached_exposure_rate_air(uint32_t cell_idx)
    {
        return m_cells[cell_idx].m_cached_exposure_rate_air;
    }

    /**
    * @brief Set the capacity of a cell in the Location in persons and volume.
    * @param persons Maximum number of Person%s that can visit the Cell at the same time.
    * @param volume Volume of the Cell in m^3.
    */
    void set_capacity(uint32_t persons, uint32_t volume, uint32_t cell_idx = 0)
    {
        m_cells[cell_idx].m_capacity.persons = persons;
        m_cells[cell_idx].m_capacity.volume  = volume;
    }

    /**
    * @return The capacity of a Cell in persons and volume.
    */
    CellCapacity get_capacity(uint32_t cell_idx = 0)
    {
        return m_cells[cell_idx].m_capacity;
    }

    /**
     * @brief Set the capacity adapted transmission risk flag.
     * @param[in] consider_capacity If true considers the capacity of the location for the computation of relative 
     * transmission risk.
     */
    void set_capacity_adapted_transmission_risk_flag(bool consider_capacity)
    {
        m_capacity_adapted_transmission_risk = consider_capacity;
    }

    /**
     * @brief Get the information whether NPIs are active at this Location.
     * If true requires e.g. Mask%s when entering a Location.
     */
    bool get_npi_active() const
    {
        return m_npi_active;
    }

    /**
     * @brief Activate or deactivate NPIs at this Location.
     * @param[in] new_status Status of NPIs.
     */
    void set_npi_active(bool new_status)
    {
        m_npi_active = new_status;
    }

    /**
     * @brief Get the total number of Person%s at the Location.
     * @return Number of Person%s.
     */
    uint32_t get_number_persons();

    /**
     * @brief Get the number of Person%s of a particular InfectionState for all Cell%s.
     * @param[in] t TimePoint of querry.
     * @param[in] state InfectionState of interest.
     * @return Amount of Person%s of the InfectionState in all Cell%s.
     */
    uint32_t get_subpopulation(TimePoint t, InfectionState state) const;

    /**
     * Add a timepoint to the subpopulations timeseries.
     * @param[in] t The TimePoint to be added.
    */
    void store_subpopulations(const TimePoint t);

    /**
     * @brief Initialize the history of subpopulations.
     * @param[in] t The TimePoint of initialization.
    */
    void initialize_subpopulations(TimePoint t);

    /**
     * @brief Get the complete history of subpopulations.
    */
    const TimeSeries<ScalarType>& get_subpopulations() const;

private:
    LocationType m_type; ///< Type of the Location.
    uint32_t m_index; ///< Index of the Location.
    bool m_capacity_adapted_transmission_risk; /**< If true considers the LocationCapacity for the computation of the 
    transmission risk.*/
    LocalInfectionParameters m_parameters; ///< Infection parameters for the Location.
    TimeSeries<ScalarType> m_subpopulations{Eigen::Index(InfectionState::Count)};
    std::vector<Cell> m_cells;
    MaskType m_required_mask; ///< Least secure type of Mask that is needed to enter the Location.
    bool m_npi_active; ///< If true requires e.g. Mask%s to enter the Location.
};

} // namespace abm
} // namespace mio

#endif

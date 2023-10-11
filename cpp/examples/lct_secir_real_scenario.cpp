/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Lena Ploetzke
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

#include "boost/outcome/try.hpp"
#include "lct_secir/model.h"
#include "lct_secir/infection_state.h"
#include "lct_secir/parameters.h"
#include "lct_secir/simulation.h"
#include "lct_secir/parameters_io.h"

#include "memilio/config.h"
#include "memilio/io/result_io.h"
#include "memilio/io/io.h"
#include "memilio/utils/time_series.h"
#include "memilio/math/eigen.h"
#include "boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp"
#include "boost/filesystem.hpp"
#include <iostream>

/**
 * @brief Indices of contact matrix corresponding to locations where contacts occur.
 */
enum class ContactLocation
{
    Home = 0,
    School,
    Work,
    Other,
    Count
};

/**
 * @brief Different types of NPI, used as DampingType.
 */
enum class Intervention
{
    Home,
    SchoolClosure,
    HomeOffice,
    GatheringBanFacilitiesClosure,
    PhysicalDistanceAndMasks,
    Count
};

/**
 * @brief Different level of NPI, used as DampingLevel.
 */
enum class InterventionLevel
{
    Main,
    PhysicalDistanceAndMasks,
    Count
};

// Map the ContactLocation%s to file names.
static const std::map<ContactLocation, std::string> contact_locations = {{ContactLocation::Home, "home"},
                                                                         {ContactLocation::School, "school_pf_eig"},
                                                                         {ContactLocation::Work, "work"},
                                                                         {ContactLocation::Other, "other"}};

/**
 * @brief Add NPIs to a given contact matrix from 01/06/2020 on.
 *
 * NPIs have been adjusted so that they approximate the trend of the RKI data for a period of 45 days from 01/06/2020 on.
 * 
 * @param[in] contact_matrices The contact matrices where the NPIs shpuld be added to.
 * @param[in] start_date Start date of the simulation used for setting the NPIs.
 */
void set_npi_june(mio::ContactMatrixGroup& contact_matrices, mio::Date start_date)
{
    // ---------------------01/06/2020--------------------------------
    /* NPIs from Paper "Assessment of effective mitigation ..." (doi: 10.1016/j.mbs.2021.108648) with slightly higher value for 
    GatheringBanFacilitiesClosure. */
    auto offset_npi = mio::SimulationTime(mio::get_offset_in_days(mio::Date(2020, 6, 1), start_date));
    // Contact reduction at home.
    ScalarType v = 0.1;
    contact_matrices[size_t(ContactLocation::Home)].add_damping(Eigen::MatrixXd::Constant(1, 1, v),
                                                                mio::DampingLevel(int(InterventionLevel::Main)),
                                                                mio::DampingType(int(Intervention::Home)), offset_npi);
    // Home-Office + people stopped working.
    v = (0.25 + 0.025);
    contact_matrices[size_t(ContactLocation::Work)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::HomeOffice)), offset_npi);
    // GatheringBanFacilitiesClosure affects ContactLocation Other.
    v = 0.2;
    contact_matrices[size_t(ContactLocation::Other)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::GatheringBanFacilitiesClosure)), offset_npi);
    // PhysicalDistanceAndMasks in all locations.
    v = 0.1;
    for (auto&& contact_location : contact_locations) {
        contact_matrices[size_t(contact_location.first)].add_damping(
            Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
            mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    }
    // Remote schooling.
    v = 0.5;
    contact_matrices[size_t(ContactLocation::School)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::SchoolClosure)), offset_npi);

    // ---------------------02/06/2020--------------------------------
    /* Values of new infections per day of the RKI data are rising from 02/06/2020 on.
     This could be partly explained by the relatively low number of cases and psychological effects.
     Most NPIs need to be lifted in order to reproduce the trend of the number of cases.*/
    offset_npi = mio::SimulationTime(mio::get_offset_in_days(mio::Date(2020, 6, 2), start_date));
    // Lifted contact reduction at home.
    v = 0.;
    contact_matrices[size_t(ContactLocation::Home)].add_damping(Eigen::MatrixXd::Constant(1, 1, v),
                                                                mio::DampingLevel(int(InterventionLevel::Main)),
                                                                mio::DampingType(int(Intervention::Home)), offset_npi);
    // Lifted home-Office, people continued to stop working.
    v = (0. + 0.025);
    contact_matrices[size_t(ContactLocation::Work)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::HomeOffice)), offset_npi);
    // Lifted gatheringBanFacilitiesClosure affects ContactLocation Other.
    v = 0.;
    contact_matrices[size_t(ContactLocation::Other)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::GatheringBanFacilitiesClosure)), offset_npi);
    // Lifted physicalDistanceAndMasks in all locations.
    v = 0.;
    for (auto&& contact_location : contact_locations) {
        contact_matrices[size_t(contact_location.first)].add_damping(
            Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
            mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    }
    // Lifted part of remote schooling.
    v = 0.25;
    contact_matrices[size_t(ContactLocation::School)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::SchoolClosure)), offset_npi);

    // ---------------------14/06/2020--------------------------------
    // Number of cases are rising again so that further NPIs had to be implemented. People became more cautious again.
    offset_npi = mio::SimulationTime(mio::get_offset_in_days(mio::Date(2020, 6, 14), start_date));
    // Contact reduction at home.
    v = 0.1;
    contact_matrices[size_t(ContactLocation::Home)].add_damping(Eigen::MatrixXd::Constant(1, 1, v),
                                                                mio::DampingLevel(int(InterventionLevel::Main)),
                                                                mio::DampingType(int(Intervention::Home)), offset_npi);
    // Home-Office + people stopped working.
    v = (0.25 + 0.025);
    contact_matrices[size_t(ContactLocation::Work)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::HomeOffice)), offset_npi);
    // GatheringBanFacilitiesClosure affects ContactLocation Other.
    v = 0.25;
    contact_matrices[size_t(ContactLocation::Other)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::GatheringBanFacilitiesClosure)), offset_npi);
    // PhysicalDistanceAndMasks in all locations.
    v = 0.25;
    for (auto&& contact_location : contact_locations) {
        contact_matrices[size_t(contact_location.first)].add_damping(
            Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
            mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    }
    // Remote schooling.
    v = 0.35;
    contact_matrices[size_t(ContactLocation::School)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::SchoolClosure)), offset_npi);

    // ---------------------03/07/2020--------------------------------
    // Number of cases are at a low level. People begin to meet again but are more cautious than on 02/06/2020.
    offset_npi = mio::SimulationTime(mio::get_offset_in_days(mio::Date(2020, 7, 3), start_date));
    // Contact reduction at home.
    v = 0.;
    contact_matrices[size_t(ContactLocation::Home)].add_damping(Eigen::MatrixXd::Constant(1, 1, v),
                                                                mio::DampingLevel(int(InterventionLevel::Main)),
                                                                mio::DampingType(int(Intervention::Home)), offset_npi);
    // Home-Office + people stopped working.
    v = (0. + 0.025);
    contact_matrices[size_t(ContactLocation::Work)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::HomeOffice)), offset_npi);
    // GatheringBanFacilitiesClosure affects ContactLocation Other.
    v = 0.1;
    contact_matrices[size_t(ContactLocation::Other)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::GatheringBanFacilitiesClosure)), offset_npi);
    // PhysicalDistanceAndMasks in ContactLocation%s Home.
    v = 0.;
    contact_matrices[size_t(ContactLocation::Home)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
        mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    // PhysicalDistanceAndMasks in ContactLocation%s School.
    v = 0.1;
    contact_matrices[size_t(ContactLocation::School)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
        mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    // PhysicalDistanceAndMasks in ContactLocation%s Work and Other.
    v = 0.1;
    contact_matrices[size_t(ContactLocation::Work)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
        mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    v = 0.;
    contact_matrices[size_t(ContactLocation::Other)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
        mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    // Remote schooling.
    v = 0.2;
    contact_matrices[size_t(ContactLocation::School)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::SchoolClosure)), offset_npi);
}

/**
 * @brief Add NPIs to a given contact matrix from 01/10/2020 on.
 *
 * NPIs from the Paper "Assessment of effective mitigation ..." (doi: 10.1016/j.mbs.2021.108648) are used with slight 
 * modifications for a period of 45 days from 01/10/2020 on.
 * 
 * @param[in] contact_matrices The contact matrices where the NPIs shpuld be added to.
 * @param[in] start_date Start date of the simulation used for setting the NPIs.
 * @param[in] lockdown_hard Proportion of counties for which a hard lockdown is implemented.
 */
void set_npi_october(mio::ContactMatrixGroup& contact_matrices, mio::Date start_date, ScalarType lockdown_hard)
{
    // ---------------------01/10/2020--------------------------------
    // NPIs from Paper for october.
    auto offset_npi = mio::SimulationTime(mio::get_offset_in_days(mio::Date(2020, 10, 1), start_date));
    // For the beginning of the time period, we assume only half of the defined proportion of counties is in a hard lockdown.
    lockdown_hard = lockdown_hard / 2;
    // Contact reduction at home.
    double v = 0.3 * (1 - lockdown_hard) + lockdown_hard * 0.5;
    contact_matrices[size_t(ContactLocation::Home)].add_damping(Eigen::MatrixXd::Constant(1, 1, v),
                                                                mio::DampingLevel(int(InterventionLevel::Main)),
                                                                mio::DampingType(int(Intervention::Home)), offset_npi);
    // Home-Office + people stopped working.
    v = (0.25 + 0.025) * (1 - lockdown_hard) + lockdown_hard * (0.25 + 0.15);
    contact_matrices[size_t(ContactLocation::Work)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::HomeOffice)), offset_npi);
    // GatheringBanFacilitiesClosure affects ContactLocation Other.
    v = 0.1 * (1 - lockdown_hard) + lockdown_hard * 0.7;
    contact_matrices[size_t(ContactLocation::Other)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::GatheringBanFacilitiesClosure)), offset_npi);
    // PhysicalDistanceAndMasks in all locations.
    v = 0.3 * (1 - lockdown_hard) + lockdown_hard * 0.7;
    for (auto&& contact_location : contact_locations) {
        contact_matrices[size_t(contact_location.first)].add_damping(
            Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
            mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    }
    // Remote schooling.
    v = lockdown_hard * 0.25;
    contact_matrices[size_t(ContactLocation::School)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::SchoolClosure)), offset_npi);

    // ---------------------24/10/2020--------------------------------
    /* We assume that the stricter NPIs of november defined in the paper are beginning about a week earlier, 
    which can be seen from the RKI data.
    Moreover the lockdown value of PhysicalDistanceAndMasks in the location school is assumed to apply for all counties.*/
    offset_npi = mio::SimulationTime(mio::get_offset_in_days(mio::Date(2020, 10, 24), start_date));
    // For the second half of the simulation, the proportion of counties in hard lockdown is increased to compensate for the lower proportion before.
    lockdown_hard = lockdown_hard * 3;
    // Contact reduction at home.
    v = 0.5;
    contact_matrices[size_t(ContactLocation::Home)].add_damping(Eigen::MatrixXd::Constant(1, 1, v),
                                                                mio::DampingLevel(int(InterventionLevel::Main)),
                                                                mio::DampingType(int(Intervention::Home)), offset_npi);
    // Home-Office + people stopped working.
    v = (0.25 + 0.05) * (1 - lockdown_hard) + lockdown_hard * (0.25 + 0.15);
    contact_matrices[size_t(ContactLocation::Work)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::HomeOffice)), offset_npi);
    // GatheringBanFacilitiesClosure affects ContactLocation Other.
    v = 0.7;
    contact_matrices[size_t(ContactLocation::Other)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::GatheringBanFacilitiesClosure)), offset_npi);
    // PhysicalDistanceAndMasks in ContactLocation%s Home.
    v = 0.3 * (1 - lockdown_hard) + lockdown_hard * 0.7;
    contact_matrices[size_t(ContactLocation::Home)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
        mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    // PhysicalDistanceAndMasks in ContactLocation%s School.
    v = 0.7;
    contact_matrices[size_t(ContactLocation::School)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
        mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    // PhysicalDistanceAndMasks in ContactLocation%s Work and Other.
    v = 0.5 * (1 - lockdown_hard) + lockdown_hard * 0.7;
    contact_matrices[size_t(ContactLocation::Work)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
        mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    contact_matrices[size_t(ContactLocation::Other)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::PhysicalDistanceAndMasks)),
        mio::DampingType(int(Intervention::PhysicalDistanceAndMasks)), offset_npi);
    // Remote schooling.
    v = lockdown_hard * 0.25;
    contact_matrices[size_t(ContactLocation::School)].add_damping(
        Eigen::MatrixXd::Constant(1, 1, v), mio::DampingLevel(int(InterventionLevel::Main)),
        mio::DampingType(int(Intervention::SchoolClosure)), offset_npi);
}

/**
 * @brief Set the contact pattern of parameters for a Model without division in age groups.
 *
 * The contacts are calculated using contact matrices from files in the data directory for different locations.
 * Also set Nonpharmaceutical Interventions influencing the ContactPatterns used for simulation in the timeframe from start_date to end_date.
 * 
 * @param[in] data_dir Directory to files with minimum and baseline contact matrices.
 * @param[in] parameters Object that the contact pattern will be added to.
 * @param[in] simulation_parameters Map with parameters necessary for the calculation of contacts an NPIs which can be different for diffferent start dates.
 *      Function uses the values to define start and end date, lockdown_hard and scale_contacts.
 * @returns Any io errors that happen during reading of the input files.
 */
mio::IOResult<void> set_contact_matrices(const fs::path& data_dir, mio::lsecir::Parameters& parameters,
                                         std::map<std::string, ScalarType> simulation_parameters)
{
    // Files in data_dir are containing contact matrices with 6 agegroups. We use this to compute a contact pattern without division of age groups.
    // Age group sizes are calculated using table number 12411-04-02-4-B from www.regionalstatistik.de for the date 31.12.2020.
    const double age_group_sizes[] = {3969138.0, 7508662, 18921292, 28666166, 18153339, 5936434};
    const int total                = 83155031.0;
    const int numagegroups         = 6;
    mio::Date start_date =
        mio::Date(2020, (int)simulation_parameters["start_month"], (int)simulation_parameters["start_day"]);
    mio::Date end_date = mio::offset_date_by_days(start_date, 45);

    auto contact_matrices = mio::ContactMatrixGroup(contact_locations.size(), 1);
    // Load and set minimum and baseline contacts for each contact location.
    for (auto&& contact_location : contact_locations) {
        BOOST_OUTCOME_TRY(baseline,
                          mio::read_mobility_plain(
                              (data_dir / "contacts" / ("baseline_" + contact_location.second + ".txt")).string()));
        BOOST_OUTCOME_TRY(minimum,
                          mio::read_mobility_plain(
                              (data_dir / "contacts" / ("minimum_" + contact_location.second + ".txt")).string()));
        ScalarType base = 0;
        ScalarType min  = 0;
        for (int i = 0; i < numagegroups; i++) {
            for (int j = 0; j < numagegroups; j++) {
                // Calculate a weighted average according to the age group sizes of the total contacts.
                base += age_group_sizes[i] / total * baseline(i, j);
                min += age_group_sizes[i] / total * minimum(i, j);
            }
        }
        contact_matrices[size_t(contact_location.first)].get_baseline() =
            simulation_parameters["scale_contacts"] * Eigen::MatrixXd::Constant(1, 1, base);
        contact_matrices[size_t(contact_location.first)].get_minimum() =
            simulation_parameters["scale_contacts"] * Eigen::MatrixXd::Constant(1, 1, min);
    }

    // ----- Add NPIs to the contact matrices. -----
    // Set of NPIs for June.
    if (mio::Date(2020, 6, 1) < end_date) {
        set_npi_june(contact_matrices, start_date);
    }

    // Set of NPIs for October.
    auto start_npi_october = mio::Date(2020, 10, 1);
    if (start_npi_october < end_date) {
        set_npi_october(contact_matrices, start_date, simulation_parameters["lockdown_hard"]);
    }

    // Set ContactPatterns in parameters.
    parameters.get<mio::lsecir::ContactPatterns>() = mio::UncertainContactMatrix(contact_matrices);

    return mio::success();
}

/**
 * @brief Performs a simulation of a real scenario with an LCT and an ODE model.
 *
 * @param[in] path Path of the RKI file that should be used to compute initial values for simulations.
 * @param[in] simulation_parameters Map with parameters necessary for the simulation which can be different for diffferent start dates.
 *        Provide the parameters "start_month", "start_day","seasonality" (parameter k for the seasonality of the models), 
 *        "RelativeTransmissionNoSymptoms", "RiskOfInfectionFromSymptomatic", "scale_confirmed_cases" (to scale the RKI data while computing an initialization vector),
 *        "lockdown_hard" (Proportion of counties for which a hard lockdown is implemented) and 
 *        "scale_contacts" (scales contacts per hand to match the new infections in the RKI data).
 * @param[in] save_dir Specifies the directory where the results should be stored. Provide an empty string if results should not be saved.
 * @param[in] print_result Specifies if the results should be printed.
 * @returns Any io errors that happen during reading of the RKI file or files for contact matrices or saving the results.
 */
mio::IOResult<void> simulate(std::string const& path, std::map<std::string, ScalarType> simulation_parameters,
                             std::string save_dir = "", bool print_result = false)
{
    // Set values needed for initialization.
    ScalarType total_population = 83155031.0;
    mio::Date start_date =
        mio::Date(2020, (int)simulation_parameters["start_month"], (int)simulation_parameters["start_day"]);
    mio::Date end_date = mio::offset_date_by_days(start_date, 45);

    // Define parameters used for simulation and initialization.
    mio::lsecir::Parameters parameters;
    parameters.get<mio::lsecir::TimeExposed>()                      = 3.335;
    parameters.get<mio::lsecir::TimeInfectedNoSymptoms>()           = 3.31331;
    parameters.get<mio::lsecir::TimeInfectedSymptoms>()             = 6.94547;
    parameters.get<mio::lsecir::TimeInfectedSevere>()               = 11.634346;
    parameters.get<mio::lsecir::TimeInfectedCritical>()             = 17.476959;
    parameters.get<mio::lsecir::TransmissionProbabilityOnContact>() = 0.0733271;
    parameters.get<mio::lsecir::RelativeTransmissionNoSymptoms>() =
        simulation_parameters["RelativeTransmissionNoSymptoms"];
    parameters.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() =
        simulation_parameters["RiskOfInfectionFromSymptomatic"];
    parameters.get<mio::lsecir::Seasonality>()                    = simulation_parameters["seasonality"];
    parameters.get<mio::lsecir::StartDay>()                       = mio::get_day_in_year(start_date);
    parameters.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 0.206901;
    parameters.get<mio::lsecir::SeverePerInfectedSymptoms>()      = 0.0786429;
    parameters.get<mio::lsecir::CriticalPerSevere>()              = 0.173176;
    parameters.get<mio::lsecir::DeathsPerCritical>()              = 0.217177;

    auto status = set_contact_matrices("../../data", parameters, simulation_parameters);

    // Define number of subcompartments.
    std::vector<int> vec_subcompartments((int)mio::lsecir::InfectionStateBase::Count, 1);
    // Use subcompartments with a soujourn time of approximately one day in each subcompartment.
    vec_subcompartments[(int)mio::lsecir::InfectionStateBase::Exposed] =
        round(parameters.get<mio::lsecir::TimeExposed>());
    vec_subcompartments[(int)mio::lsecir::InfectionStateBase::InfectedNoSymptoms] =
        round(parameters.get<mio::lsecir::TimeInfectedNoSymptoms>());
    vec_subcompartments[(int)mio::lsecir::InfectionStateBase::InfectedSymptoms] =
        round(parameters.get<mio::lsecir::TimeInfectedSymptoms>());
    vec_subcompartments[(int)mio::lsecir::InfectionStateBase::InfectedSevere] =
        round(parameters.get<mio::lsecir::TimeInfectedSevere>());
    // Both realistic distributions for times corresponding to InfectedCritical of the IDE model are exponential distributions.
    vec_subcompartments[(int)mio::lsecir::InfectionStateBase::InfectedCritical] =
        round(parameters.get<mio::lsecir::TimeInfectedCritical>());
    mio::lsecir::InfectionState infectionState(vec_subcompartments);

    // Calculate initial value vector for subcompartments with RKI data.
    BOOST_OUTCOME_TRY(init_subcompartments, mio::lsecir::get_initial_data_from_file(
                                                path, start_date, infectionState, std::move(parameters),
                                                total_population, simulation_parameters["scale_confirmed_cases"]));
    // Sum subcompartments to get initial values ​​for the ODE model.
    Eigen::VectorXd init_base((int)mio::lsecir::InfectionStateBase::Count);
    for (int i = 0; i < (int)mio::lsecir::InfectionStateBase::Count; i++) {
        init_base[i] =
            init_subcompartments
                .segment(Eigen::Index(infectionState.get_firstindex(i)), Eigen::Index(infectionState.get_number(i)))
                .sum();
    }

    // Initialize LCT model and perform simulation.
    mio::lsecir::Model model_lct(std::move(init_subcompartments), infectionState, std::move(parameters));
    mio::TimeSeries<ScalarType> result_lct = mio::lsecir::simulate(
        0, mio::get_offset_in_days(end_date, start_date), 0.1, model_lct,
        std::make_shared<mio::ControlledStepperWrapper<boost::numeric::odeint::runge_kutta_cash_karp54>>(1e-10, 1e-5, 0,
                                                                                                         0.1));
    // Calculate result without division in subcompartments.
    mio::TimeSeries<ScalarType> populations_lct = model_lct.calculate_populations(result_lct);

    // Initialize ODE model and perform simulation.
    mio::lsecir::InfectionState infectionState_ode(std::vector<int>((int)mio::lsecir::InfectionStateBase::Count, 1));
    mio::lsecir::Model model_ode(std::move(init_base), infectionState_ode, std::move(parameters));
    mio::TimeSeries<ScalarType> result_ode = mio::lsecir::simulate(
        0, mio::get_offset_in_days(end_date, start_date), 0.1, model_ode,
        std::make_shared<mio::ControlledStepperWrapper<boost::numeric::odeint::runge_kutta_cash_karp54>>(1e-10, 1e-5, 0,
                                                                                                         0.1));

    if (print_result) {
        // Print results.
        std::cout << "Result LCT model:" << std::endl;
        mio::lsecir::print_TimeSeries(populations_lct, model_lct.get_heading_CompartmentsBase());
        std::cout << "Result ODE model:" << std::endl;
        mio::lsecir::print_TimeSeries(result_ode, model_ode.get_heading_CompartmentsBase());
    }
    if (!save_dir.empty()) {
        // Save results.
        std::string filename = save_dir + "real_lct_2020_" + std::to_string((int)simulation_parameters["start_month"]) +
                               "_" + std::to_string((int)simulation_parameters["start_day"]) + ".h5";
        auto save_result_status = mio::save_result({populations_lct}, {0}, 1, filename);
        filename = save_dir + "real_ode_2020_" + std::to_string((int)simulation_parameters["start_month"]) + "_" +
                   std::to_string((int)simulation_parameters["start_day"]) + ".h5";
        save_result_status = mio::save_result({result_ode}, {0}, 1, filename);
    }
    // Print commands to get the number of new infections on the first day of simulation. Could be used to scale the contacts.
    std::cout << "Number of new infections on the first day of simulation: " << std::endl;
    std::cout << "LCT model:"
              << (populations_lct[0][0] - populations_lct[1][0]) /
                     (populations_lct.get_time(1) - populations_lct.get_time(0))
              << std::endl;
    std::cout << "ODE model:"
              << (result_ode[0][0] - result_ode[1][0]) / (result_ode.get_time(1) - result_ode.get_time(0)) << std::endl;

    return mio::success();
}

int main()
{
    std::string save_dir                                               = "../../data/simulation_lct/real/";
    std::map<std::string, ScalarType> simulation_parameters_2020_06_01 = {{"start_month", 6},
                                                                          {"start_day", 1},
                                                                          {"seasonality", 0.2},
                                                                          {"RelativeTransmissionNoSymptoms", 0.7},
                                                                          {"RiskOfInfectionFromSymptomatic", 0.2},
                                                                          {"scale_confirmed_cases", 1.},
                                                                          {"lockdown_hard", 0.03 * 14 / (45 * 401.)},
                                                                          {"scale_contacts", 445. / 516.7}};

    std::map<std::string, ScalarType> simulation_parameters_2020_10_01 = {{"start_month", 10},
                                                                          {"start_day", 1},
                                                                          {"seasonality", 0.2},
                                                                          {"RelativeTransmissionNoSymptoms", 1},
                                                                          {"RiskOfInfectionFromSymptomatic", 0.3},
                                                                          {"scale_confirmed_cases", 2.},
                                                                          {"lockdown_hard", 371 * 14 / (45 * 401.)},
                                                                          {"scale_contacts", 11154.2 / 13106.}};
    /* Values for "RelativeTransmissionNoSymptoms", "RelativeTransmissionNoSymptoms" and "seasonality" are suitable values based on doi: 10.1016/j.mbs.2021.108648.
    "scale_confirmed_cases" are values directly from this paper. 
    "lockdown_hard" is based on the number of beginnings of a strict lockdown for 14 days of the 45 days simulation period in the lockdown. Value is not used for 01/06/2020 and 
    scaled in octiber to assume that in the beginning of the period are less counties in lockdown and more in the second half. 
    "lockdown_hard" should give the average percentage of counties that are in a hard lockdown on a simulation day. 
    "scale_contacts" is used to match the predicted number of new infections on the first simulation day to the RKI data. */

    // Paths are valid if file is executed eg in memilio/build/bin
    auto result =
        simulate("../../data/pydata/Germany/cases_all_germany_ma7.json", simulation_parameters_2020_06_01, save_dir);
    if (!result) {
        printf("%s\n", result.error().formatted_message().c_str());
        return -1;
    }

    result =
        simulate("../../data/pydata/Germany/cases_all_germany_ma7.json", simulation_parameters_2020_10_01, save_dir);

    if (!result) {
        printf("%s\n", result.error().formatted_message().c_str());
        return -1;
    }
    return 0;
}
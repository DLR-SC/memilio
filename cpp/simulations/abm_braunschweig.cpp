#include <fstream>
#include <vector>
#include <iostream>
#include "abm/abm.h"
#include "memilio/io/result_io.h"
#include "memilio/utils/uncertain_value.h"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "abm/movement_data.h"
#include "abm/common_abm_loggers.h"

namespace fs = boost::filesystem;

/**
 * Set a value and distribution of an UncertainValue.
 * Assigns average of min and max as a value and UNIFORM(min, max) as a distribution.
 * @param p uncertain value to set.
 * @param min minimum of distribution.
 * @param max minimum of distribution.
 */
void assign_uniform_distribution(mio::UncertainValue& p, ScalarType min, ScalarType max)
{
    p = mio::UncertainValue(0.5 * (max + min));
    p.set_distribution(mio::ParameterDistributionUniform(min, max));
}

/**
 * Determine the infection state of a person at the beginning of the simulation.
 * The infection states are chosen randomly. They are distributed according to the probabilites set in the example.
 * @return random infection state
 */
mio::abm::InfectionState determine_infection_state(ScalarType exposed, ScalarType infected_no_symptoms,
                                                   ScalarType infected_symptoms, ScalarType recovered)
{
    ScalarType susceptible          = 1 - exposed - infected_no_symptoms - infected_symptoms - recovered;
    std::vector<ScalarType> weights = {
        susceptible,           exposed,  infected_no_symptoms, infected_symptoms / 3, infected_symptoms / 3,
        infected_symptoms / 3, recovered};
    if (weights.size() != (size_t)mio::abm::InfectionState::Count - 1) {
        mio::log_error("Initialization in ABM wrong, please correct vector length.");
    }
    auto state = mio::DiscreteDistribution<size_t>::get_instance()(weights);
    return (mio::abm::InfectionState)state;
}

/**
 * Assign an infection state to each person.
 */
void assign_infection_state(mio::abm::World& world, mio::abm::TimePoint t, double exposed_prob,
                            double infected_no_symptoms_prob, double infected_symptoms_prob, double recovered_prob)
{
    auto persons = world.get_persons();
    for (auto& person : persons) {
        auto infection_state =
            determine_infection_state(exposed_prob, infected_no_symptoms_prob, infected_symptoms_prob, recovered_prob);
        if (infection_state != mio::abm::InfectionState::Susceptible)
            person.add_new_infection(mio::abm::Infection(mio::abm::VirusVariant::Wildtype, person.get_age(),
                                                         world.get_global_infection_parameters(), t, infection_state));
    }
}

void split_line(std::string string, std::vector<double>* row)
{
    std::vector<std::string> strings;

    std::string x = ",,", y = ",-1,";
    size_t pos;
    while ((pos = string.find(x)) != std::string::npos) {
        string.replace(pos, 2, y);
    } // Temporary fix to handle empty cells.
    boost::split(strings, string, boost::is_any_of(","));
    std::transform(strings.begin(), strings.end(), std::back_inserter(*row), [&](std::string s) {
        if (s.rfind('.') != std::string::npos) {
            return std::stod(s);
        }
        else {
            return (double)std::stoi(s);
        }
    });
}

mio::abm::LocationType get_location_type(mio::abm::ActivityType acitivity_end)
{
    mio::abm::LocationType type;
    switch (acitivity_end) {
    case mio::abm::ActivityType::Workplace:
        type = mio::abm::LocationType::Work;
        break;
    case mio::abm::ActivityType::Education:
        type = mio::abm::LocationType::School;
        break;
    case mio::abm::ActivityType::Shopping:
        type = mio::abm::LocationType::BasicsShop;
        break;
    case mio::abm::ActivityType::Leisure:
        type = mio::abm::LocationType::SocialEvent; // Freizeit
        break;
    case mio::abm::ActivityType::PrivateMatters:
        type = mio::abm::LocationType::BasicsShop; // Private Erledigung
        break;
    case mio::abm::ActivityType::OtherActivity:
        type = mio::abm::LocationType::SocialEvent; // Sonstiges
        break;
    case mio::abm::ActivityType::Home:
        type = mio::abm::LocationType::Home;
        break;
    default:
        type = mio::abm::LocationType::Home;
        break;
    }
    return type;
}

mio::abm::AgeGroup determine_age_group(uint32_t age)
{
    if (age <= 4) {
        return mio::abm::AgeGroup::Age0to4;
    }
    else if (age <= 14) {
        return mio::abm::AgeGroup::Age5to14;
    }
    else if (age <= 34) {
        return mio::abm::AgeGroup::Age15to34;
    }
    else if (age <= 59) {
        return mio::abm::AgeGroup::Age35to59;
    }
    else if (age <= 79) {
        return mio::abm::AgeGroup::Age60to79;
    }
    else {
        return mio::abm::AgeGroup::Age80plus;
    }
}

void create_world_from_data(mio::abm::World& world, const std::string& filename)
{
    int max_number_persons = 10000;
    // Open File
    const fs::path p = filename;
    if (!fs::exists(p)) {
        mio::log_error("Cannot read in data. File does not exist.");
    }
    // File pointer
    std::fstream fin;

    // Open an existing file
    fin.open(filename, std::ios::in);
    std::vector<double> row;
    std::vector<std::string> row_string;
    std::string line;

    // Read the Titles from the Data file
    std::getline(fin, line);
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    std::vector<std::string> titles;
    boost::split(titles, line, boost::is_any_of(","));
    uint32_t count_of_titles              = 0;
    std::map<std::string, uint32_t> index = {};
    for (auto const& title : titles) {
        index.insert({title, count_of_titles});
        row_string.push_back(title);
        count_of_titles++;
    }

    std::map<uint32_t, mio::abm::LocationId> locations = {};
    std::map<uint32_t, mio::abm::Person&> persons      = {};

    // For the world we need: One Hospital, One ICU, One Home for each unique householdID, One Person for each person_id with respective age and home_id

    // We assume that no person goes to an hospitla, altough e.g. "Sonstiges" could be a hospital
    auto hospital = world.add_location(mio::abm::LocationType::Hospital);
    world.get_individualized_location(hospital).get_infection_parameters().set<mio::abm::MaximumContacts>(5);
    world.get_individualized_location(hospital).set_capacity(584, 26242);
    auto icu = world.add_location(mio::abm::LocationType::ICU);
    world.get_individualized_location(icu).get_infection_parameters().set<mio::abm::MaximumContacts>(5);
    world.get_individualized_location(icu).set_capacity(30, 1350);

    // First we create the persons and their homes and also the locations
    int number_of_persons = 0;
    while (std::getline(fin, line) && number_of_persons < max_number_persons) {
        row.clear();

        // read columns in this row
        split_line(line, &row);
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        uint32_t person_id          = (int)row[index["puid"]];
        uint32_t age                = (int)row[index["age"]];
        uint32_t home_id            = (int)row[index["huid"]];
        uint32_t target_location_id = (int)std::abs(row[index["loc_id_end"]]);
        // uint32_t origin_location_id            = (int)std::abs(row[index["loc_id_start"]]);
        mio::abm::GeographicalLocation geo_loc = {std::abs(row[index["lon_start"]]), std::abs(row[index["lat_start"]])};
        mio::abm::TransportMode transport_mode = mio::abm::TransportMode(
            row[index["travel_mode"]] -
            1); // 1:Bike, 2:Car (Driver), 3:Car (Co-Driver)), 4:Public Transport, 5:Walking, 6:Other/Unknown
        mio::abm::ActivityType activity_end = mio::abm::ActivityType(
            row[index["activity_end"]] -
            1); // 1:Workplace, 2:Education, 3:Shopping, 4:Leisure, 5:Private Matters, 6:Other Activity, 7:Home, 8:Unknown Activity
        uint32_t trip_start = (int)row[index["start_time"]];

        mio::abm::LocationId home;
        auto it_home = locations.find(home_id);
        if (it_home == locations.end()) {
            home = world.add_location(mio::abm::LocationType::Home, 1);
            world.get_individualized_location(home).set_geographical_location(geo_loc);
            locations.insert({home_id, home});
        }
        else {
            home = it_home->second;
        }

        auto it_person = persons.find(person_id);
        if (it_person == persons.end()) {
            auto& person = world.add_person(home, determine_age_group(age));
            person.set_assigned_location(home);
            person.set_assigned_location(hospital);
            person.set_assigned_location(icu);
            persons.insert({person_id, person});
            number_of_persons++;
        }

        mio::abm::LocationId location;
        auto it_location = locations.find(
            target_location_id); // Check if location already exists also for home which have the same id (home_id = target_location_id)
        if (it_location == locations.end()) {
            location = world.add_location(
                get_location_type(mio::abm::ActivityType(activity_end)),
                1); //Assume one place has one activity, this may be untrue but not important for now(?)
            world.get_individualized_location(location).set_geographical_location(geo_loc);
            locations.insert({target_location_id, location});
        }

        // Add the trip to the trip list person and location must exist at this point
        auto person          = persons.find(person_id)->second;
        auto target_location = locations.find(target_location_id)->second;
        // auto origin_location = locations.find(origin_location_id)->second;
        person.set_assigned_location(
            target_location); //This assumes that we only have in each tripchain only one location type for each person
        world.get_trip_list().add_trip(mio::abm::Trip(person.get_person_id(),
                                                      mio::abm::TimePoint(0) + mio::abm::hours(trip_start),
                                                      target_location, target_location, transport_mode, activity_end));
    }

    world.get_trip_list().use_weekday_trips_on_weekend();
}

void set_parameters(mio::abm::GlobalInfectionParameters infection_params)
{
    infection_params.set<mio::abm::IncubationPeriod>({{mio::abm::VirusVariant::Count, mio::abm::AgeGroup::Count}, 4.});

    // Set protection level from high viral load. Information based on: https://doi.org/10.1093/cid/ciaa886
    infection_params.get<mio::abm::HighViralLoadProtectionFactor>() = [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.863}, {1, 0.969}, {7, 0.029}, {10, 0.002}, {14, 0.0014}, {21, 0}}, days);
    };

    //0-4
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age0to4}]  = 0.276;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age0to4}] = 0.092;
    infection_params
        .get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] =
        0.142;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] =
        0.001;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] = 0.186;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] = 0.015;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] = 0.143;
    infection_params.get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] =
        0.001;

    //5-14
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age5to14}]  = 0.276;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age5to14}] = 0.092;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age5to14}]   = 0.142;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] =
        0.001;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] = 0.186;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] = 0.015;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] = 0.143;
    infection_params.get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] =
        0.001;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] = 0.;

    //15-34
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age15to34}]  = 0.315;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age15to34}] = 0.079;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age15to34}]   = 0.139;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] =
        0.003;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] = 0.157;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] = 0.013;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] =
        0.126;
    infection_params
        .get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] = 0.021;

    //35-59
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age35to59}]  = 0.315;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age35to59}] = 0.079;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age35to59}]   = 0.136;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] =
        0.009;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] = 0.113;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] = 0.02;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] = 0.05;
    infection_params
        .get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] = 0.008;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] =
        0.;

    //60-79
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age60to79}]  = 0.315;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age60to79}] = 0.079;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age60to79}]   = 0.123;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] =
        0.024;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] = 0.083;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] = 0.035;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] =
        0.035;
    infection_params
        .get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] = 0.023;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] =
        0.;

    //80+
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age80plus}]  = 0.315;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age80plus}] = 0.079;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age80plus}]   = 0.115;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] =
        0.033;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] = 0.055;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] = 0.036;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] =
        0.035;
    infection_params
        .get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] = 0.052;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] =
        0.;

    // Set each parameter for vaccinated people including personal infection and vaccine protection levels.
    // Summary: https://doi.org/10.1038/s41577-021-00550-x,

    //0-4
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age0to4}]  = 0.161;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age0to4}] = 0.132;
    infection_params
        .get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] =
        0.143;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] =
        0.001;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] = 0.186;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] = 0.015;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] = 0.143;
    infection_params.get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] =
        0.001;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age0to4}] = 0.0;
    // Protection of reinfection is the same for all age-groups, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5, https://doi.org/10.1038/s41591-021-01377-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age0to4, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.852},
                                                                              {180, 0.852},
                                                                              {210, 0.845},
                                                                              {240, 0.828},
                                                                              {270, 0.797},
                                                                              {300, 0.759},
                                                                              {330, 0.711},
                                                                              {360, 0.661},
                                                                              {390, 0.616},
                                                                              {420, 0.580},
                                                                              {450, 0.559},
                                                                              {450, 0.550}},
                                                                             days);
    };
    // Information is based on: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age0to4, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.91}, {60, 0.92}, {90, 0.88}, {120, 0.84}, {150, 0.81}, {180, 0.88}, {450, 0.5}}, days);
    };

    // Set up age-related severe protection levels, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age0to4, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.967},
                                                                              {30, 0.975},
                                                                              {60, 0.977},
                                                                              {90, 0.974},
                                                                              {120, 0.963},
                                                                              {150, 0.947},
                                                                              {180, 0.93},
                                                                              {210, 0.929},
                                                                              {240, 0.923},
                                                                              {270, 0.908},
                                                                              {300, 0.893},
                                                                              {330, 0.887},
                                                                              {360, 0.887},
                                                                              {450, 0.5}},
                                                                             days);
    };
    // Information is based on: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age0to4, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.88}, {60, 0.91}, {90, 0.98}, {120, 0.94}, {150, 0.88}, {450, 0.5}}, days);
    };

    //5-14
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age5to14}]  = 0.161;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age5to14}] = 0.132;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age5to14}]   = 0.143;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] =
        0.001;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] = 0.186;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] = 0.015;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] = 0.143;
    infection_params.get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] =
        0.001;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age5to14}] =
        0.0;
    // Protection of reinfection is the same for all age-groups, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5, https://doi.org/10.1038/s41591-021-01377-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age5to14, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.852},
                                                                              {180, 0.852},
                                                                              {210, 0.845},
                                                                              {240, 0.828},
                                                                              {270, 0.797},
                                                                              {300, 0.759},
                                                                              {330, 0.711},
                                                                              {360, 0.661},
                                                                              {390, 0.616},
                                                                              {420, 0.580},
                                                                              {450, 0.559},
                                                                              {450, 0.550}},
                                                                             days);
    };
    // Information is based on: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age5to14, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.91}, {60, 0.92}, {90, 0.88}, {120, 0.84}, {150, 0.81}, {180, 0.88}, {450, 0.5}}, days);
    };
    // Set up age-related severe protection levels, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age5to14, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.967},
                                                                              {30, 0.975},
                                                                              {60, 0.977},
                                                                              {90, 0.974},
                                                                              {120, 0.963},
                                                                              {150, 0.947},
                                                                              {180, 0.93},
                                                                              {210, 0.929},
                                                                              {240, 0.923},
                                                                              {270, 0.908},
                                                                              {300, 0.893},
                                                                              {330, 0.887},
                                                                              {360, 0.887},
                                                                              {450, 0.5}},
                                                                             days);
    };
    // Information is based on: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age5to14, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.88}, {60, 0.91}, {90, 0.98}, {120, 0.94}, {150, 0.88}, {450, 0.5}}, days);
    };

    //15-34
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age15to34}]  = 0.179;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age15to34}] = 0.126;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age15to34}]   = 0.142;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] =
        0.001;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] = 0.157;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] = 0.013;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] =
        0.126;
    infection_params
        .get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] = 0.021;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age15to34}] =
        0.0;
    // Set up personal infection and vaccine protection levels, based on: https://doi.org/10.1038/s41577-021-00550-x, https://doi.org/10.1038/s41591-021-01377-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age15to34, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.852},
                                                                              {180, 0.852},
                                                                              {210, 0.845},
                                                                              {240, 0.828},
                                                                              {270, 0.797},
                                                                              {300, 0.759},
                                                                              {330, 0.711},
                                                                              {360, 0.661},
                                                                              {390, 0.616},
                                                                              {420, 0.580},
                                                                              {450, 0.559},
                                                                              {450, 0.550}},
                                                                             days);
    };
    // Information is based on: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age15to34, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.89}, {60, 0.84}, {90, 0.78}, {120, 0.68}, {150, 0.57}, {180, 0.39}, {450, 0.1}}, days);
    };
    // Set up age-related severe protection levels, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age15to34, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.967},
                                                                              {30, 0.975},
                                                                              {60, 0.977},
                                                                              {90, 0.974},
                                                                              {120, 0.963},
                                                                              {150, 0.947},
                                                                              {180, 0.93},
                                                                              {210, 0.929},
                                                                              {240, 0.923},
                                                                              {270, 0.908},
                                                                              {300, 0.893},
                                                                              {330, 0.887},
                                                                              {360, 0.887},
                                                                              {450, 0.5}},
                                                                             days);
    };
    // Information is from: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age15to34, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.88}, {60, 0.91}, {90, 0.98}, {120, 0.94}, {150, 0.88}, {180, 0.90}, {450, 0.5}}, days);
    };

    //35-59
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age35to59}]  = 0.179;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age35to59}] = 0.126;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age35to59}]   = 0.141;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] =
        0.003;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] = 0.113;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] = 0.02;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] = 0.05;
    infection_params
        .get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] = 0.008;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age35to59}] =
        0.0;
    // Protection of reinfection is the same for all age-groups, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5, https://doi.org/10.1038/s41591-021-01377-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age35to59, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.852},
                                                                              {180, 0.852},
                                                                              {210, 0.845},
                                                                              {240, 0.828},
                                                                              {270, 0.797},
                                                                              {300, 0.759},
                                                                              {330, 0.711},
                                                                              {360, 0.661},
                                                                              {390, 0.616},
                                                                              {420, 0.580},
                                                                              {450, 0.559},
                                                                              {450, 0.550}},
                                                                             days);
    };
    // Information is based on: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age35to59, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.89}, {60, 0.84}, {90, 0.78}, {120, 0.68}, {150, 0.57}, {180, 0.39}, {450, 0.1}}, days);
    };
    // Set up age-related severe protection levels, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age35to59, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.967},
                                                                              {30, 0.975},
                                                                              {60, 0.977},
                                                                              {90, 0.974},
                                                                              {120, 0.963},
                                                                              {150, 0.947},
                                                                              {180, 0.93},
                                                                              {210, 0.929},
                                                                              {240, 0.923},
                                                                              {270, 0.908},
                                                                              {300, 0.893},
                                                                              {330, 0.887},
                                                                              {360, 0.887},
                                                                              {450, 0.5}},
                                                                             days);
    };
    // Information is from: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age35to59, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.88}, {60, 0.91}, {90, 0.98}, {120, 0.94}, {150, 0.88}, {180, 0.90}, {450, 0.5}}, days);
    };

    //60-79
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age60to79}]  = 0.179;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age60to79}] = 0.126;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age60to79}]   = 0.136;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] =
        0.009;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] = 0.083;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] = 0.035;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] =
        0.035;
    infection_params
        .get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] = 0.023;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age60to79}] =
        0.0;
    // Protection of reinfection is the same for all age-groups, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5, https://doi.org/10.1038/s41591-021-01377-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age60to79, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.852},
                                                                              {180, 0.852},
                                                                              {210, 0.845},
                                                                              {240, 0.828},
                                                                              {270, 0.797},
                                                                              {300, 0.759},
                                                                              {330, 0.711},
                                                                              {360, 0.661},
                                                                              {390, 0.616},
                                                                              {420, 0.580},
                                                                              {450, 0.559},
                                                                              {450, 0.550}},
                                                                             days);
    };
    // Information is based on: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age60to79, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.87}, {60, 0.85}, {90, 0.78}, {120, 0.67}, {150, 0.61}, {180, 0.50}, {450, 0.1}}, days);
    };
    // Set up personal severe protection levels.
    // Protection of severe infection of age group 65 + is different from other age group, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age60to79, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.967},
                                                                              {30, 0.975},
                                                                              {60, 0.977},
                                                                              {90, 0.974},
                                                                              {120, 0.963},
                                                                              {150, 0.947},
                                                                              {180, 0.93},
                                                                              {210, 0.929},
                                                                              {240, 0.923},
                                                                              {270, 0.908},
                                                                              {300, 0.893},
                                                                              {330, 0.887},
                                                                              {360, 0.887},
                                                                              {360, 0.5}},
                                                                             days);
    };
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age60to79, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.91}, {60, 0.86}, {90, 0.91}, {120, 0.94}, {150, 0.95}, {180, 0.90}, {450, 0.5}}, days);
    };

    //80+
    infection_params.get<mio::abm::InfectedNoSymptomsToSymptoms>()[{mio::abm::VirusVariant::Wildtype,
                                                                    mio::abm::AgeGroup::Age80plus}]  = 0.179;
    infection_params.get<mio::abm::InfectedNoSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                     mio::abm::AgeGroup::Age80plus}] = 0.126;
    infection_params.get<mio::abm::InfectedSymptomsToRecovered>()[{mio::abm::VirusVariant::Wildtype,
                                                                   mio::abm::AgeGroup::Age80plus}]   = 0.133;
    infection_params
        .get<mio::abm::InfectedSymptomsToSevere>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] =
        0.012;
    infection_params
        .get<mio::abm::SevereToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] = 0.055;
    infection_params
        .get<mio::abm::SevereToCritical>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] = 0.036;
    infection_params
        .get<mio::abm::CriticalToRecovered>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] =
        0.035;
    infection_params
        .get<mio::abm::CriticalToDead>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] = 0.052;
    infection_params
        .get<mio::abm::RecoveredToSusceptible>()[{mio::abm::VirusVariant::Wildtype, mio::abm::AgeGroup::Age80plus}] =
        0.0;
    // Protection of reinfection is the same for all age-groups, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5, https://doi.org/10.1038/s41591-021-01377-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age80plus, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.852},
                                                                              {180, 0.852},
                                                                              {210, 0.845},
                                                                              {240, 0.828},
                                                                              {270, 0.797},
                                                                              {300, 0.759},
                                                                              {330, 0.711},
                                                                              {360, 0.661},
                                                                              {390, 0.616},
                                                                              {420, 0.580},
                                                                              {450, 0.559},
                                                                              {450, 0.550}},
                                                                             days);
    };
    // Information is from: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::InfectionProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age80plus, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.80}, {60, 0.79}, {90, 0.75}, {120, 0.56}, {150, 0.49}, {180, 0.43}, {450, 0.1}}, days);
    };
    // Set up personal severe protection levels.
    // Protection of severe infection of age group 65 + is different from other age group, based on:
    // https://doi.org/10.1016/S0140-6736(22)02465-5
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::NaturalInfection, mio::abm::AgeGroup::Age0to4, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>({{0, 0.967},
                                                                              {30, 0.975},
                                                                              {60, 0.977},
                                                                              {90, 0.974},
                                                                              {120, 0.963},
                                                                              {150, 0.947},
                                                                              {180, 0.93},
                                                                              {210, 0.929},
                                                                              {240, 0.923},
                                                                              {270, 0.908},
                                                                              {300, 0.893},
                                                                              {330, 0.887},
                                                                              {360, 0.887},
                                                                              {360, 0.5}},
                                                                             days);
    };
    // Information is based on: https://doi.org/10.1016/S0140-6736(21)02183-8
    infection_params.get<mio::abm::SeverityProtectionFactor>()[{
        mio::abm::ExposureType::GenericVaccine, mio::abm::AgeGroup::Age80plus, mio::abm::VirusVariant::Wildtype}] =
        [](ScalarType days) -> ScalarType {
        return mio::linear_interpolation_of_data_set<ScalarType, ScalarType>(
            {{0, 0.5}, {30, 0.84}, {60, 0.88}, {90, 0.89}, {120, 0.86}, {150, 0.85}, {180, 0.83}, {450, 0.5}}, days);
    };
}

/**
 * Create a sampled simulation with start time t0.
 * @param t0 the start time of the simulation
*/
mio::abm::Simulation create_sampled_simulation(const mio::abm::TimePoint& t0)
{
    // Assumed percentage of infection state at the beginning of the simulation.
    ScalarType exposed_prob = 0.005, infected_no_symptoms_prob = 0.001, infected_symptoms_prob = 0.001,
               recovered_prob = 0.0;

    //Set global infection parameters (similar to infection parameters in SECIR model) and initialize the world
    mio::abm::GlobalInfectionParameters infection_params;
    set_parameters(infection_params);
    auto world = mio::abm::World(infection_params);

    // Create the world object from statistical data.
    create_world_from_data(world, "C:/Users/korf_sa/Documents/rep/cpp/simulations/bs_niedersachsen.csv");
    world.use_migration_rules(false);

    // Assign an infection state to each person.
    assign_infection_state(world, t0, exposed_prob, infected_no_symptoms_prob, infected_symptoms_prob, recovered_prob);

    auto t_lockdown = mio::abm::TimePoint(0) + mio::abm::days(20);

    // During the lockdown, 25% of people work from home and schools are closed for 90% of students.
    // Social events are very rare.
    mio::abm::set_home_office(t_lockdown, 0.25, world.get_migration_parameters());
    mio::abm::set_school_closure(t_lockdown, 0.9, world.get_migration_parameters());
    mio::abm::close_social_events(t_lockdown, 0.9, world.get_migration_parameters());

    auto sim = mio::abm::Simulation(t0, std::move(world));
    return sim;
}

template <typename T>
void write_log_to_file(const T& history)
{
    auto logg          = history.get_log();
    auto loc_id        = std::get<0>(logg)[0];
    auto agent_id      = std::get<1>(logg)[0];
    auto movement_data = std::get<2>(logg);
    // Write lo to a text file.
    std::ofstream myfile("locations_lookup.txt");
    myfile << "location_id, latitude, longitude\n";
    for (uint32_t loc_id_index = 0; loc_id_index < loc_id.size(); ++loc_id_index) {
        auto id           = std::get<0>(loc_id[loc_id_index]);
        auto id_longitute = std::get<1>(loc_id[loc_id_index]).longitude;
        auto id_latitude  = std::get<1>(loc_id[loc_id_index]).latitude;
        myfile << id << ", " << id_longitute << ", " << id_latitude << "\n";
    }
    myfile.close();

    std::ofstream myfile2("agents_lookup.txt");
    myfile2 << "agent_id, home_id, age\n";
    for (uint32_t agent_id_index = 0; agent_id_index < agent_id.size(); ++agent_id_index) {
        auto id      = std::get<0>(agent_id[agent_id_index]);
        auto home_id = std::get<1>(agent_id[agent_id_index]);
        auto age     = (int)std::get<2>(agent_id[agent_id_index]);
        myfile2 << id << ", " << home_id << ", " << age << "\n";
    }
    myfile2.close();

    std::ofstream myfile3("movement_data.txt");
    myfile3 << "trip_id, agent_id, start_location, end_location, start_time, end_time, transport_mode, activity, "
               "infection_state \n";
    int trips_id = 0;
    for (uint32_t movement_data_index = 0; movement_data_index < movement_data.size(); ++movement_data_index) {
        for (uint32_t trip_index = 0; trip_index < movement_data[movement_data_index].size(); trip_index++) {
            auto start_location  = movement_data[movement_data_index][trip_index].from_id;
            auto end_location    = movement_data[movement_data_index][trip_index].to_id;
            auto start_time      = movement_data[movement_data_index][trip_index].start_time.seconds();
            auto end_time        = movement_data[movement_data_index][trip_index].end_time.seconds();
            auto transport_mode  = (int)movement_data[movement_data_index][trip_index].transport_mode;
            auto activity        = (int)movement_data[movement_data_index][trip_index].activity_type;
            auto infection_state = (int)movement_data[movement_data_index][trip_index].infection_state;
            myfile3 << trips_id << ", " << start_location << " , " << end_location << " , " << start_time << " , "
                    << end_time << " , " << transport_mode << " , " << activity << " , " << infection_state << "\n";
            trips_id++;
        }
        myfile3 << "timestep Nr.:" << movement_data_index << "\n";
    }
    myfile3.close();
}

mio::IOResult<void> run(const fs::path& result_dir, size_t num_runs, bool save_single_runs = true)
{

    auto t0               = mio::abm::TimePoint(0); // Start time per simulation
    auto tmax             = mio::abm::TimePoint(0) + mio::abm::hours(30); // End time per simulation
    auto ensemble_results = std::vector<std::vector<mio::TimeSeries<ScalarType>>>{}; // Vector of collected results
    ensemble_results.reserve(size_t(num_runs));
    auto run_idx            = size_t(1); // The run index
    auto save_result_result = mio::IOResult<void>(mio::success()); // Variable informing over successful IO operations

    // Loop over a number of runs
    while (run_idx <= num_runs) {

        // Create the sampled simulation with start time t0.
        auto sim = create_sampled_simulation(t0);
        //output object
        mio::History<mio::DataWriterToMemory, LogLocationInformation, LogPersonInformation, LogMovementData> history;
        // Collect the id of location in world.
        std::vector<int> loc_ids;
        for (auto& location : sim.get_world().get_locations()) {
            loc_ids.push_back(location.get_index());
        }
        // Advance the world to tmax
        sim.advance(tmax, history);
        // TODO: update result of the simulation to be a vector of location result.
        auto temp_sim_result = std::vector<mio::TimeSeries<ScalarType>>{sim.get_result()};
        // Push result of the simulation back to the result vector
        ensemble_results.push_back(temp_sim_result);
        // Option to save the current run result to file
        if (save_result_result && save_single_runs) {
            auto result_dir_run = result_dir / ("abm_result_run_" + std::to_string(run_idx) + ".h5");
            BOOST_OUTCOME_TRY(save_result(ensemble_results.back(), loc_ids, 1, result_dir_run.string()));
        }
        write_log_to_file(history);
        ++run_idx;
    }
    BOOST_OUTCOME_TRY(save_result_result);
    return mio::success();
}

int main(int argc, char** argv)
{
    mio::set_log_level(mio::LogLevel::warn);

    std::string result_dir = ".";
    size_t num_runs        = 1;
    bool save_single_runs  = true;

    if (argc == 2) {
        num_runs = atoi(argv[1]);
        printf("Number of run is %s.\n", argv[1]);
        printf("Saving results to the current directory.\n");
    }

    else if (argc == 3) {
        num_runs   = atoi(argv[1]);
        result_dir = argv[2];
        printf("Number of run is %s.\n", argv[1]);
        printf("Saving results to \"%s\".\n", result_dir.c_str());
    }
    else {
        printf("Usage:\n");
        printf("abm_example <num_runs>\n");
        printf("\tRun the simulation for <num_runs> time(s).\n");
        printf("\tStore the results in the current directory.\n");
        printf("abm_example <num_runs> <result_dir>\n");
        printf("\tRun the simulation for <num_runs> time(s).\n");
        printf("\tStore the results in <result_dir>.\n");
    }

    // mio::thread_local_rng().seed({...}); //set seeds, e.g., for debugging
    //printf("Seeds: ");
    //for (auto s : mio::thread_local_rng().get_seeds()) {
    //    printf("%u, ", s);
    //}
    //printf("\n");

    auto result = run(result_dir, num_runs, save_single_runs);
    if (!result) {
        printf("%s\n", result.error().formatted_message().c_str());
        return -1;
    }
    return 0;
}

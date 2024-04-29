/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Martin Siggel, Daniel Abele, Martin J. Kuehn, Jan Kleinert, Khoa Nguyen
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

#include "pybind_util.h"
#include "epidemiology/damping.h"
#include "epidemiology/contact_matrix.h"
#include "epidemiology/damping_sampling.h"
#include "epidemiology/uncertain_matrix.h"
#include "epidemiology/dynamic_npis.h"
#include "mobility/metapopulation_mobility_instant.h"
#include "utils/date.h"
#include "utils/logging.h"
#include "utils/time_series.h"
#include "utils/parameter_distributions.h"
#include "utils/uncertain_value.h"
#include "utils/index.h"
#include "utils/custom_index_array.h"

#include "memilio/mobility/metapopulation_mobility_instant.h"
#include "memilio/utils/date.h"
#include "memilio/geography/regions.h"
#include "memilio/epidemiology/contact_matrix.h"
#include "memilio/epidemiology/simulation_day.h"
#include "memilio/io/mobility_io.h"
#include "memilio/io/epi_data.h"

namespace py = pybind11;

namespace pymio
{

template <>
std::string pretty_name<mio::AgeGroup>()
{
    return "AgeGroup";
}

template <>
std::string pretty_name<mio::SimulationDay>()
{
    return "SimulationDay";
}

} // namespace pymio

PYBIND11_MODULE(_simulation, m)
{
    pymio::bind_CustomIndexArray<mio::UncertainValue, mio::AgeGroup>(m, "AgeGroupArray");
    pymio::bind_class<mio::AgeGroup, pymio::EnablePickling::Required, mio::Index<mio::AgeGroup>>(m, "AgeGroup").def(py::init<size_t>());

    pymio::bind_CustomIndexArray<double, mio::AgeGroup, mio::SimulationDay>(m, "AgeGroupSimulationDayArray");
    pymio::bind_class<mio::SimulationDay, pymio::EnablePickling::Required, mio::Index<mio::SimulationDay>>(m, "SimulationDay").def(py::init<size_t>());

    pymio::bind_date(m, "Date");

    auto damping_class = pymio::bind_class<mio::SquareDamping, pymio::EnablePickling::Required>(m, "Damping");
    pymio::bind_damping_members(damping_class);

    auto dampings_class = pymio::bind_class<mio::SquareDampings, pymio::EnablePickling::Required>(m, "Dampings");
    pymio::bind_dampings_members(dampings_class);

    pymio::bind_time_series(m, "TimeSeries");

    pymio::bind_parameter_distribution(m, "ParameterDistribution");
    pymio::bind_parameter_distribution_normal(m, "ParameterDistributionNormal");
    pymio::bind_parameter_distribution_uniform(m, "ParameterDistributionUniform");

    pymio::bind_uncertain_value(m, "UncertainValue");

    auto contact_matrix_class = pymio::bind_class<mio::ContactMatrix, pymio::EnablePickling::Required>(m, "ContactMatrix");
    pymio::bind_damping_expression_members(contact_matrix_class);
    contact_matrix_class.def_property_readonly("num_groups", &mio::ContactMatrix::get_num_groups);

    auto contact_matrix_group_class = pymio::bind_class<mio::ContactMatrixGroup, pymio::EnablePickling::Required>(m, "ContactMatrixGroup");
    pymio::bind_damping_expression_group_members(contact_matrix_group_class);
    contact_matrix_group_class.def_property_readonly("num_groups", &mio::ContactMatrixGroup::get_num_groups);

    pymio::bind_damping_sampling(m, "DampingSampling");

    pymio::bind_uncertain_contact_matrix(m, "UncertainContactMatrix");

    auto migration_damping_class = pymio::bind_class<mio::VectorDamping, pymio::EnablePickling::Required>(m, "MigrationDamping");
    pymio::bind_damping_members(migration_damping_class);

    auto migration_dampings_class = pymio::bind_class<mio::VectorDampings, pymio::EnablePickling::Required>(m, "MigrationDampings");
    pymio::bind_dampings_members(migration_dampings_class);

    auto migration_coeffs_class = pymio::bind_class<mio::MigrationCoefficients, pymio::EnablePickling::Required>(m, "MigrationCoefficients");
    pymio::bind_damping_expression_members(migration_coeffs_class);

    auto migration_coeff_group_class = pymio::bind_class<mio::MigrationCoefficientGroup, pymio::EnablePickling::Required>(m, "MigrationCoefficientGroup");
    pymio::bind_damping_expression_group_members(migration_coeff_group_class);

    pymio::bind_dynamicNPI_members(m, "DynamicNPIs");

    pymio::bind_migration_parameters(m, "MigrationParameters");
    pymio::bind_migration_parameter_edge(m, "MigrationParameterEdge");
    pymio::bind_migration(m, "Migration");
    pymio::bind_migration_edge(m, "MigrationEdge");

    m.def(
        "get_state_id_de",
        [](int county) {
            return int(mio::regions::get_state_id(int(mio::regions::CountyId(county))));
        },
        py::arg("county_id"));
    m.def(
        "get_holidays_de",
        [](int state, mio::Date start_date, mio::Date end_date) {
            auto h = mio::regions::get_holidays(mio::regions::StateId(state), start_date, end_date);
            return std::vector<std::pair<mio::Date, mio::Date>>(h.begin(), h.end());
        },
        py::arg("state_id"), py::arg("start_date") = mio::Date(std::numeric_limits<int>::min(), 1, 1),
        py::arg("end_date") = mio::Date(std::numeric_limits<int>::max(), 1, 1));

    m.def(
        "read_mobility_plain",
        [](const std::string& filename) {
            auto result = mio::read_mobility_plain(filename);
            return pymio::check_and_throw(result);
        },
        py::return_value_policy::move);

#ifdef MEMILIO_HAS_JSONCPP
    m.def(
        "get_node_ids",
        [](const std::string& path, bool is_node_for_county) {
            auto result = mio::get_node_ids(path, is_node_for_county);
            return pymio::check_and_throw(result);
        },
        py::return_value_policy::move);
#endif // MEMILIO_HAS_JSONCPP

    pymio::bind_logging(m, "LogLevel");

    m.def("seed_random_number_generator", [] {
        mio::thread_local_rng().seed(mio::RandomNumberGenerator::generate_seeds());
    });

    m.attr("__version__") = "dev";
}

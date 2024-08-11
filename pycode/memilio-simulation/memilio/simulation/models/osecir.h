/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Daniel Abele, Maximilian Betz
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
#ifndef PYMIO_OSECIR_H
#define PYMIO_OSECIR_H

//Includes from pymio
#include "memilio/config.h"
#include "pybind_util.h"
#include "compartments/simulation.h"
#include "compartments/flow_simulation.h"
#include "compartments/compartmentalmodel.h"
#include "epidemiology/age_group.h"
#include "epidemiology/populations.h"
#include "utils/custom_index_array.h"
#include "utils/parameter_set.h"
#include "utils/index.h"
#include "mobility/graph_simulation.h"
#include "mobility/metapopulation_mobility_instant.h"
#include "io/mobility_io.h"
#include "io/result_io.h"

//Includes from MEmilio
#include "ode_secir/model.h"
#include "ode_secir/analyze_result.h"
#include "ode_secir/parameter_space.h"
#include "ode_secir/parameters_io.h"
#include "memilio/compartments/parameter_studies.h"
#include "memilio/data/analyze_result.h"
#include "memilio/mobility/graph.h"
#include "memilio/io/mobility_io.h"
#include "memilio/io/epi_data.h"

#include "pybind11/pybind11.h"
#include "pybind11/stl_bind.h"
#include "Eigen/Core"
#include <vector>

namespace py = pybind11;

namespace pymio
{
//specialization of pretty_name
template <>
inline std::string pretty_name<mio::osecir::InfectionState>()
{
    return "InfectionState";
}

} // namespace pymio

void bind_osecir(py::module_& m);

#endif //PYMIO_OSECIR_H

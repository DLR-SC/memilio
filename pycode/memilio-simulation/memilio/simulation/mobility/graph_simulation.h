/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*
* Authors: Martin Siggel, Daniel Abele, Martin J. Kuehn, Jan Kleinert, Maximilian Betz
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
#ifndef PYMIO_GRAPH_SIMULATION_H
#define PYMIO_GRAPH_SIMULATION_H

#include "memilio/mobility/mobility.h"
#include "memilio/mobility/graph_simulation.h"

#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace pymio
{

/*
 * @brief bind GraphSimulation for any node and edge type
 */
template <class Graph>
void bind_GraphSimulation(py::module& m, std::string const& name)
{
    using GS = mio::GraphSimulation<Graph>;
    py::class_<GS>(m, name.c_str())
        .def(py::init([](Graph& graph, double t0, double dt) {
                 return std::make_unique<GS>(mio::make_migration_sim(t0, dt, std::move(graph)));
             }),
             py::arg("graph"), py::arg("t0") = 0.0, py::arg("dt") = 1.0)
        .def_property_readonly(
            "graph", [](GS& self) -> Graph& { return self.get_graph(); },
            py::return_value_policy::reference_internal)
        .def_property_readonly("t", &GS::get_t)
        .def("advance", &GS::advance, py::arg("tmax"));
}


} // namespace pymio

#endif //PYMIO_GRAPH_SIMULATION_H
/* 
* Copyright (C) 2020-2024 MEmilio
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
#include "mobility/metapopulation_mobility_instant.h"

#include "pybind11/eigen.h"

namespace py = pybind11;

namespace pymio
{

void bind_migration_parameters(py::module_& m, std::string const& name)
{
    py::class_<mio::MigrationParameters<double>>(m, name.c_str())
        .def(py::init<const Eigen::VectorXd&>(), py::arg("coeffs"))
        .def(py::init<const mio::MigrationCoefficientGroup&>(), py::arg("coeffs"))
        .def_property(
            "coefficients", py::overload_cast<>(&mio::MigrationParameters<double>::get_coefficients),
            [](mio::MigrationParameters<double>& self, const mio::MigrationCoefficientGroup& v) {
                self.get_coefficients() = v;
            },
            py::return_value_policy::reference_internal);
}

void bind_migration_parameter_edge(py::module_& m, std::string const& name)
{
    py::class_<mio::Edge<mio::MigrationParameters<double>>>(m, name.c_str())
        .def_property_readonly("start_node_idx",
                               [](const mio::Edge<mio::MigrationParameters<double>>& self) {
                                   return self.start_node_idx;
                               })
        .def_property_readonly("end_node_idx",
                               [](const mio::Edge<mio::MigrationParameters<double>>& self) {
                                   return self.end_node_idx;
                               })
        .def_property_readonly(
            "property", [](const mio::Edge<mio::MigrationEdge<double>>& self) -> auto& { return self.property; },
            py::return_value_policy::reference_internal);
}

void bind_migration(py::module_& m, std::string const& name)
{
    py::class_<mio::MigrationEdge<double>>(m, name.c_str())
        .def(py::init<const Eigen::VectorXd&>(), py::arg("coeffs"))
        .def(py::init<const mio::MigrationParameters<double>&>(), py::arg("params"))
        .def_property_readonly(
            "parameters", [](const mio::MigrationEdge<double>& self) -> auto& { return self.get_parameters(); },
            py::return_value_policy::reference_internal);
}

void bind_migration_edge(py::module_& m, std::string const& name)
{
    py::class_<mio::Edge<mio::MigrationEdge<double>>>(m, name.c_str())
        .def_property_readonly("start_node_idx",
                               [](const mio::Edge<mio::MigrationEdge<double>>& self) {
                                   return self.start_node_idx;
                               })
        .def_property_readonly("end_node_idx",
                               [](const mio::Edge<mio::MigrationEdge<double>>& self) {
                                   return self.end_node_idx;
                               })
        .def_property_readonly(
            "property", [](const mio::Edge<mio::MigrationEdge<double>>& self) -> auto& { return self.property; },
            py::return_value_policy::reference_internal);
}

} // namespace pymio

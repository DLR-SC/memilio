/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Henrik Zunker
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
#ifndef PYMIO_EPI_DYNAMIC_LOCKDOWN_H
#define PYMIO_EPI_DYNAMIC_LOCKDOWN_H

#include "memilio/config.h"
#include "memilio/epidemiology/damping.h"
#include "memilio/epidemiology/damping_sampling.h"
#include "memilio/epidemiology/dynamic_npis.h"
#include "pybind_util.h"

#include "pybind11/pybind11.h"
#include <utility>

namespace pymio
{

void bind_dynamicNPI_members(pybind11::module_& m, std::string const& name)
{
    bind_Range<decltype(std::declval<mio::DynamicNPIs>().get_thresholds())>(m, "_ThresholdRange");
    using C = mio::DynamicNPIs;
    pybind11::class_<C>(m, name.c_str())
        .def(pybind11::init<>())
        .def_property(
            "interval",
            [](mio::DynamicNPIs& self) {
                return static_cast<double>(self.get_interval());
            },
            [](mio::DynamicNPIs& self, double v) {
                self.set_interval(mio::SimulationTime(v));
            })
        .def_property(
            "duration",
            [](mio::DynamicNPIs& self) {
                return static_cast<double>(self.get_duration());
            },
            [](mio::DynamicNPIs& self, double v) {
                self.set_duration(mio::SimulationTime(v));
            })
        .def_property(
            "base_value",
            [](mio::DynamicNPIs& self) {
                return static_cast<double>(self.get_base_value());
            },
            [](mio::DynamicNPIs& self, double v) {
                self.set_base_value(v);
            })
        .def_property_readonly("threshold",
                               [](mio::DynamicNPIs& self) {
                                   return self.get_thresholds();
                               })
        .def("set_threshold", [](mio::DynamicNPIs& self, double threshold, const std::vector<mio::DampingSampling>& v) {
            self.set_threshold(threshold, v);
        });

} // namespace pymio

} // namespace pymio

#endif //PYMIO_EPI_DYNAMIC_LOCKDOWN_H

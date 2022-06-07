/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*
* Authors: Maximilian Betz
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
#include "utils/logging.h"
#include "memilio/utils/logging.h"

namespace pymio
{

void bind_logging(pybind11::module& m, std::string const& name)
{
    pybind11::enum_<mio::LogLevel>(m, name.c_str())
        .value("Off", mio::LogLevel::off)
        .value("Critical", mio::LogLevel::critical)
        .value("Error", mio::LogLevel::err)
        .value("Warning", mio::LogLevel::warn)
        .value("Info", mio::LogLevel::info)
        .value("Debug", mio::LogLevel::debug)
        .value("Trace", mio::LogLevel::trace);
    m.def("set_log_level", &mio::set_log_level);
}

} // namespace pymio
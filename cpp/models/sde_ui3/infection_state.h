/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Nils Wassmuth, Rene Schmieding, Martin J. Kuehn
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

#ifndef MIO_SDE_UI3_INFECTIONSTATE_H
#define MIO_SDE_UI3_INFECTIONSTATE_H

namespace mio
{
namespace sui3
{

/**
 * @brief The InfectionState enum describes the possible
 * categories for the infectious state of persons
 */
enum class InfectionState
{   
    Uninfected_I0,
    Uninfected_I1,
    Uninfected_I2,
    Uninfected_I3,
    Infected_V1I0,
    Infected_V1I1,
    Infected_V1I2,
    Infected_V1I3,
    Infected_V2I0,
    Infected_V2I1,
    Infected_V2I2,
    Infected_V2I3,
    Infected_V3I0,
    Infected_V3I1,
    Infected_V3I2,
    Infected_V3I3,
    Count
};

} // namespace sui3
} // namespace mio

#endif // MIO_SDE_UI3_INFECTIONSTATE_H

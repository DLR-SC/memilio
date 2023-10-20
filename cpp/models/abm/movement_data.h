/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Sascha Korf
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

#ifndef ABM_MOVEMENT_DATA_H
#define ABM_MOVEMENT_DATA_H

#include "abm/time.h"

namespace mio
{
namespace abm
{

enum class TransportMode : uint32_t
{
    Bike = 0,
    CarDriver,
    CarPassenger,
    PublicTransport,
    Walking,
    Other,
    Unknown
};

enum class ActivityType : uint32_t
{
    Workplace = 0,
    Education,
    Shopping,
    Leisure,
    PrivateMatters,
    OtherActivity,
    Home,
    UnknownActivity
};

struct movement_data {
    uint32_t agent_id;
    uint32_t from_id;
    uint32_t to_id;
    mio::abm::TimePoint start_time;
    mio::abm::TimePoint end_time;
    mio::abm::TransportMode transport_mode;
    mio::abm::ActivityType activity_type;
    mio::abm::InfectionState infection_state;
};

} // namespace abm
} // namespace mio

#endif //ABM_MOVEMENT_DATA_H

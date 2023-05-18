/*
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*
* Authors: David Kerkmann, Khoa Nguyen
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
#ifndef EPI_ABM_VACCINE_H
#define EPI_ABM_VACCINE_H

#include "abm/time.h"

#include <cstdint>

namespace mio
{
namespace abm
{

/** 
 * Vaccine in ABM.
 * can be used as 0-based index
 */
enum class Vaccine : std::uint32_t
{
    Moderna           = 0,
    Pfizer            = 1,
    Astrazeneca       = 2,
    JohnsonAndJohnson = 3,
    Novavax           = 4,
    Sinovac           = 5,
    Sputnik           = 6,
    Count //last!!
};

/**
 * A vaccination is a tuple of TimePoint and Vaccine.
 * The TimePoint describes the time of administration of the Vaccine.
*/
struct Vaccination {
    Vaccination(Vaccine v, TimePoint t)
    {
        vaccine = v;
        time    = t;
    }

    Vaccine vaccine;
    TimePoint time;
};

} // namespace abm
} // namespace mio

#endif

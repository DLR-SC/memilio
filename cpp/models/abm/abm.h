/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*        & Helmholtz Centre for Infection Research (HZI)
*
* Authors: Daniel Abele, Majid Abedi, Elisabeth Kluth
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
/** single include header for ABM */

#ifndef EPI_ABM_H
#define EPI_ABM_H

#include "models/abm/parameters.h"
#include "models/abm/simulation.h"
#include "models/abm/world.h"
#include "models/abm/person.h"
#include "models/abm/location.h"
#include "models/abm/location_type.h"
#include "memilio/utils/random_number_generator.h"
#include "models/abm/migration_rules.h"
#include "models/abm/testing_strategy.h"
#include "models/abm/infection.h"
#include "models/abm/infection_state.h"
#include "models/abm/virus_variant.h"
#include "models/abm/vaccine.h"
#include "models/abm/age.h"
#include "models/abm/household.h"
#include "models/abm/lockdown_rules.h"

#endif

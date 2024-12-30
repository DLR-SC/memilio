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

#ifndef MIO_SDE_UI10_MODEL_H
#define MIO_SDE_UI10_MODEL_H

#include "memilio/compartments/flow_model.h"
#include "memilio/epidemiology/populations.h"
#include "memilio/utils/random_number_generator.h"
#include "sde_ui10/infection_state.h"
#include "sde_ui10/parameters.h"

namespace mio
{
namespace sui10
{

/********************
 * define the model *
 ********************/

using Flows = TypeList<Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V1I0>, //Infection
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V2I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V3I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V4I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V5I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V6I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V7I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V8I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V9I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V10I0>,

                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V1I1>, 
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V2I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V3I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V4I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V5I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V6I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V7I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V8I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V9I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V10I1>,

                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V1I2>, 
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V2I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V3I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V4I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V5I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V6I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V7I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V8I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V9I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V10I2>,

                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V1I3>, 
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V2I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V3I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V4I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V5I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V6I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V7I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V8I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V9I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V10I3>,

                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V1I4>, 
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V2I4>,
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V3I4>,
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V4I4>,
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V5I4>,
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V6I4>,
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V7I4>,
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V8I4>,
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V9I4>,
                       Flow<InfectionState::Uninfected_I4, InfectionState::Infected_V10I4>,

                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V1I5>, 
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V2I5>,
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V3I5>,
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V4I5>,
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V5I5>,
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V6I5>,
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V7I5>,
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V8I5>,
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V9I5>,
                       Flow<InfectionState::Uninfected_I5, InfectionState::Infected_V10I5>,

                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V1I6>, 
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V2I6>,
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V3I6>,
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V4I6>,
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V5I6>,
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V6I6>,
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V7I6>,
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V8I6>,
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V9I6>,
                       Flow<InfectionState::Uninfected_I6, InfectionState::Infected_V10I6>,

                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V1I7>, 
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V2I7>,
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V3I7>,
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V4I7>,
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V5I7>,
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V6I7>,
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V7I7>,
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V8I7>,
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V9I7>,
                       Flow<InfectionState::Uninfected_I7, InfectionState::Infected_V10I7>,

                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V1I8>, 
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V2I8>,
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V3I8>,
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V4I8>,
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V5I8>,
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V6I8>,
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V7I8>,
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V8I8>,
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V9I8>,
                       Flow<InfectionState::Uninfected_I8, InfectionState::Infected_V10I8>,

                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V1I9>, 
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V2I9>,
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V3I9>,
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V4I9>,
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V5I9>,
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V6I9>,
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V7I9>,
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V8I9>,
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V9I9>,
                       Flow<InfectionState::Uninfected_I9, InfectionState::Infected_V10I9>,

                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V1I10>, 
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V2I10>,
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V3I10>,
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V4I10>,
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V5I10>,
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V6I10>,
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V7I10>,
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V8I10>,
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V9I10>,
                       Flow<InfectionState::Uninfected_I10, InfectionState::Infected_V10I10>,


                       Flow<InfectionState::Infected_V1I0, InfectionState::Infected_V2I0>, //mutation
                       Flow<InfectionState::Infected_V1I1, InfectionState::Infected_V2I1>,
                       Flow<InfectionState::Infected_V1I2, InfectionState::Infected_V2I2>,
                       Flow<InfectionState::Infected_V1I3, InfectionState::Infected_V2I3>,
                       Flow<InfectionState::Infected_V1I4, InfectionState::Infected_V2I4>,
                       Flow<InfectionState::Infected_V1I5, InfectionState::Infected_V2I5>,
                       Flow<InfectionState::Infected_V1I6, InfectionState::Infected_V2I6>,
                       Flow<InfectionState::Infected_V1I7, InfectionState::Infected_V2I7>,
                       Flow<InfectionState::Infected_V1I8, InfectionState::Infected_V2I8>,
                       Flow<InfectionState::Infected_V1I9, InfectionState::Infected_V2I9>,
                       Flow<InfectionState::Infected_V1I10, InfectionState::Infected_V2I10>,

                       Flow<InfectionState::Infected_V2I0, InfectionState::Infected_V1I0>,
                       Flow<InfectionState::Infected_V2I0, InfectionState::Infected_V3I0>,
                       Flow<InfectionState::Infected_V2I1, InfectionState::Infected_V1I1>,
                       Flow<InfectionState::Infected_V2I1, InfectionState::Infected_V3I1>,
                       Flow<InfectionState::Infected_V2I2, InfectionState::Infected_V1I2>,
                       Flow<InfectionState::Infected_V2I2, InfectionState::Infected_V3I2>,
                       Flow<InfectionState::Infected_V2I3, InfectionState::Infected_V1I3>,
                       Flow<InfectionState::Infected_V2I3, InfectionState::Infected_V3I3>,
                       Flow<InfectionState::Infected_V2I4, InfectionState::Infected_V1I4>,
                       Flow<InfectionState::Infected_V2I4, InfectionState::Infected_V3I4>,
                       Flow<InfectionState::Infected_V2I5, InfectionState::Infected_V1I5>,
                       Flow<InfectionState::Infected_V2I5, InfectionState::Infected_V3I5>,
                       Flow<InfectionState::Infected_V2I6, InfectionState::Infected_V1I6>,
                       Flow<InfectionState::Infected_V2I6, InfectionState::Infected_V3I6>,
                       Flow<InfectionState::Infected_V2I7, InfectionState::Infected_V1I7>,
                       Flow<InfectionState::Infected_V2I7, InfectionState::Infected_V3I7>,
                       Flow<InfectionState::Infected_V2I8, InfectionState::Infected_V1I8>,
                       Flow<InfectionState::Infected_V2I8, InfectionState::Infected_V3I8>,
                       Flow<InfectionState::Infected_V2I9, InfectionState::Infected_V1I9>,
                       Flow<InfectionState::Infected_V2I9, InfectionState::Infected_V3I9>,
                       Flow<InfectionState::Infected_V2I10, InfectionState::Infected_V1I10>,
                       Flow<InfectionState::Infected_V2I10, InfectionState::Infected_V3I10>,

                       Flow<InfectionState::Infected_V3I0, InfectionState::Infected_V2I0>,
                       Flow<InfectionState::Infected_V3I0, InfectionState::Infected_V4I0>,
                       Flow<InfectionState::Infected_V3I1, InfectionState::Infected_V2I1>,
                       Flow<InfectionState::Infected_V3I1, InfectionState::Infected_V4I1>,
                       Flow<InfectionState::Infected_V3I2, InfectionState::Infected_V2I2>,
                       Flow<InfectionState::Infected_V3I2, InfectionState::Infected_V4I2>,
                       Flow<InfectionState::Infected_V3I3, InfectionState::Infected_V2I3>,
                       Flow<InfectionState::Infected_V3I3, InfectionState::Infected_V4I3>,
                       Flow<InfectionState::Infected_V3I4, InfectionState::Infected_V2I4>,
                       Flow<InfectionState::Infected_V3I4, InfectionState::Infected_V4I4>,
                       Flow<InfectionState::Infected_V3I5, InfectionState::Infected_V2I5>,
                       Flow<InfectionState::Infected_V3I5, InfectionState::Infected_V4I5>,
                       Flow<InfectionState::Infected_V3I6, InfectionState::Infected_V2I6>,
                       Flow<InfectionState::Infected_V3I6, InfectionState::Infected_V4I6>,
                       Flow<InfectionState::Infected_V3I7, InfectionState::Infected_V2I7>,
                       Flow<InfectionState::Infected_V3I7, InfectionState::Infected_V4I7>,
                       Flow<InfectionState::Infected_V3I8, InfectionState::Infected_V2I8>,
                       Flow<InfectionState::Infected_V3I8, InfectionState::Infected_V4I8>,
                       Flow<InfectionState::Infected_V3I9, InfectionState::Infected_V2I9>,
                       Flow<InfectionState::Infected_V3I9, InfectionState::Infected_V4I9>,
                       Flow<InfectionState::Infected_V3I10, InfectionState::Infected_V2I10>,
                       Flow<InfectionState::Infected_V3I10, InfectionState::Infected_V4I10>,

                       Flow<InfectionState::Infected_V4I0, InfectionState::Infected_V3I0>,
                       Flow<InfectionState::Infected_V4I0, InfectionState::Infected_V5I0>,
                       Flow<InfectionState::Infected_V4I1, InfectionState::Infected_V3I1>,
                       Flow<InfectionState::Infected_V4I1, InfectionState::Infected_V5I1>,
                       Flow<InfectionState::Infected_V4I2, InfectionState::Infected_V3I2>,
                       Flow<InfectionState::Infected_V4I2, InfectionState::Infected_V5I2>,
                       Flow<InfectionState::Infected_V4I3, InfectionState::Infected_V3I3>,
                       Flow<InfectionState::Infected_V4I3, InfectionState::Infected_V5I3>,
                       Flow<InfectionState::Infected_V4I4, InfectionState::Infected_V3I4>,
                       Flow<InfectionState::Infected_V4I4, InfectionState::Infected_V5I4>,
                       Flow<InfectionState::Infected_V4I5, InfectionState::Infected_V3I5>,
                       Flow<InfectionState::Infected_V4I5, InfectionState::Infected_V5I5>,
                       Flow<InfectionState::Infected_V4I6, InfectionState::Infected_V3I6>,
                       Flow<InfectionState::Infected_V4I6, InfectionState::Infected_V5I6>,
                       Flow<InfectionState::Infected_V4I7, InfectionState::Infected_V3I7>,
                       Flow<InfectionState::Infected_V4I7, InfectionState::Infected_V5I7>,
                       Flow<InfectionState::Infected_V4I8, InfectionState::Infected_V3I8>,
                       Flow<InfectionState::Infected_V4I8, InfectionState::Infected_V5I8>,
                       Flow<InfectionState::Infected_V4I9, InfectionState::Infected_V3I9>,
                       Flow<InfectionState::Infected_V4I9, InfectionState::Infected_V5I9>,
                       Flow<InfectionState::Infected_V4I10, InfectionState::Infected_V3I10>,
                       Flow<InfectionState::Infected_V4I10, InfectionState::Infected_V5I10>,

                       Flow<InfectionState::Infected_V5I0, InfectionState::Infected_V4I0>,
                       Flow<InfectionState::Infected_V5I0, InfectionState::Infected_V6I0>,
                       Flow<InfectionState::Infected_V5I1, InfectionState::Infected_V4I1>,
                       Flow<InfectionState::Infected_V5I1, InfectionState::Infected_V6I1>,
                       Flow<InfectionState::Infected_V5I2, InfectionState::Infected_V4I2>,
                       Flow<InfectionState::Infected_V5I2, InfectionState::Infected_V6I2>,
                       Flow<InfectionState::Infected_V5I3, InfectionState::Infected_V4I3>,
                       Flow<InfectionState::Infected_V5I3, InfectionState::Infected_V6I3>,
                       Flow<InfectionState::Infected_V5I4, InfectionState::Infected_V4I4>,
                       Flow<InfectionState::Infected_V5I4, InfectionState::Infected_V6I4>,
                       Flow<InfectionState::Infected_V5I5, InfectionState::Infected_V4I5>,
                       Flow<InfectionState::Infected_V5I5, InfectionState::Infected_V6I5>,
                       Flow<InfectionState::Infected_V5I6, InfectionState::Infected_V4I6>,
                       Flow<InfectionState::Infected_V5I6, InfectionState::Infected_V6I6>,
                       Flow<InfectionState::Infected_V5I7, InfectionState::Infected_V4I7>,
                       Flow<InfectionState::Infected_V5I7, InfectionState::Infected_V6I7>,
                       Flow<InfectionState::Infected_V5I8, InfectionState::Infected_V4I8>,
                       Flow<InfectionState::Infected_V5I8, InfectionState::Infected_V6I8>,
                       Flow<InfectionState::Infected_V5I9, InfectionState::Infected_V4I9>,
                       Flow<InfectionState::Infected_V5I9, InfectionState::Infected_V6I9>,
                       Flow<InfectionState::Infected_V5I10, InfectionState::Infected_V4I10>,
                       Flow<InfectionState::Infected_V5I10, InfectionState::Infected_V6I10>,

                       Flow<InfectionState::Infected_V6I0, InfectionState::Infected_V5I0>,
                       Flow<InfectionState::Infected_V6I0, InfectionState::Infected_V7I0>,
                       Flow<InfectionState::Infected_V6I1, InfectionState::Infected_V5I1>,
                       Flow<InfectionState::Infected_V6I1, InfectionState::Infected_V7I1>,
                       Flow<InfectionState::Infected_V6I2, InfectionState::Infected_V5I2>,
                       Flow<InfectionState::Infected_V6I2, InfectionState::Infected_V7I2>,
                       Flow<InfectionState::Infected_V6I3, InfectionState::Infected_V5I3>,
                       Flow<InfectionState::Infected_V6I3, InfectionState::Infected_V7I3>,
                       Flow<InfectionState::Infected_V6I4, InfectionState::Infected_V5I4>,
                       Flow<InfectionState::Infected_V6I4, InfectionState::Infected_V7I4>,
                       Flow<InfectionState::Infected_V6I5, InfectionState::Infected_V5I5>,
                       Flow<InfectionState::Infected_V6I5, InfectionState::Infected_V7I5>,
                       Flow<InfectionState::Infected_V6I6, InfectionState::Infected_V5I6>,
                       Flow<InfectionState::Infected_V6I6, InfectionState::Infected_V7I6>,
                       Flow<InfectionState::Infected_V6I7, InfectionState::Infected_V5I7>,
                       Flow<InfectionState::Infected_V6I7, InfectionState::Infected_V7I7>,
                       Flow<InfectionState::Infected_V6I8, InfectionState::Infected_V5I8>,
                       Flow<InfectionState::Infected_V6I8, InfectionState::Infected_V7I8>,
                       Flow<InfectionState::Infected_V6I9, InfectionState::Infected_V5I9>,
                       Flow<InfectionState::Infected_V6I9, InfectionState::Infected_V7I9>,
                       Flow<InfectionState::Infected_V6I10, InfectionState::Infected_V5I10>,
                       Flow<InfectionState::Infected_V6I10, InfectionState::Infected_V7I10>,

                       Flow<InfectionState::Infected_V7I0, InfectionState::Infected_V6I0>,
                       Flow<InfectionState::Infected_V7I0, InfectionState::Infected_V8I0>,
                       Flow<InfectionState::Infected_V7I1, InfectionState::Infected_V6I1>,
                       Flow<InfectionState::Infected_V7I1, InfectionState::Infected_V8I1>,
                       Flow<InfectionState::Infected_V7I2, InfectionState::Infected_V6I2>,
                       Flow<InfectionState::Infected_V7I2, InfectionState::Infected_V8I2>,
                       Flow<InfectionState::Infected_V7I3, InfectionState::Infected_V6I3>,
                       Flow<InfectionState::Infected_V7I3, InfectionState::Infected_V8I3>,
                       Flow<InfectionState::Infected_V7I4, InfectionState::Infected_V6I4>,
                       Flow<InfectionState::Infected_V7I4, InfectionState::Infected_V8I4>,
                       Flow<InfectionState::Infected_V7I5, InfectionState::Infected_V6I5>,
                       Flow<InfectionState::Infected_V7I5, InfectionState::Infected_V8I5>,
                       Flow<InfectionState::Infected_V7I6, InfectionState::Infected_V6I6>,
                       Flow<InfectionState::Infected_V7I6, InfectionState::Infected_V8I6>,
                       Flow<InfectionState::Infected_V7I7, InfectionState::Infected_V6I7>,
                       Flow<InfectionState::Infected_V7I7, InfectionState::Infected_V8I7>,
                       Flow<InfectionState::Infected_V7I8, InfectionState::Infected_V6I8>,
                       Flow<InfectionState::Infected_V7I8, InfectionState::Infected_V8I8>,
                       Flow<InfectionState::Infected_V7I9, InfectionState::Infected_V6I9>,
                       Flow<InfectionState::Infected_V7I9, InfectionState::Infected_V8I9>,
                       Flow<InfectionState::Infected_V7I10, InfectionState::Infected_V6I10>,
                       Flow<InfectionState::Infected_V7I10, InfectionState::Infected_V8I10>,

                       Flow<InfectionState::Infected_V8I0, InfectionState::Infected_V7I0>,
                       Flow<InfectionState::Infected_V8I0, InfectionState::Infected_V9I0>,
                       Flow<InfectionState::Infected_V8I1, InfectionState::Infected_V7I1>,
                       Flow<InfectionState::Infected_V8I1, InfectionState::Infected_V9I1>,
                       Flow<InfectionState::Infected_V8I2, InfectionState::Infected_V7I2>,
                       Flow<InfectionState::Infected_V8I2, InfectionState::Infected_V9I2>,
                       Flow<InfectionState::Infected_V8I3, InfectionState::Infected_V7I3>,
                       Flow<InfectionState::Infected_V8I3, InfectionState::Infected_V9I3>,
                       Flow<InfectionState::Infected_V8I4, InfectionState::Infected_V7I4>,
                       Flow<InfectionState::Infected_V8I4, InfectionState::Infected_V9I4>,
                       Flow<InfectionState::Infected_V8I5, InfectionState::Infected_V7I5>,
                       Flow<InfectionState::Infected_V8I5, InfectionState::Infected_V9I5>,
                       Flow<InfectionState::Infected_V8I6, InfectionState::Infected_V7I6>,
                       Flow<InfectionState::Infected_V8I6, InfectionState::Infected_V9I6>,
                       Flow<InfectionState::Infected_V8I7, InfectionState::Infected_V7I7>,
                       Flow<InfectionState::Infected_V8I7, InfectionState::Infected_V9I7>,
                       Flow<InfectionState::Infected_V8I8, InfectionState::Infected_V7I8>,
                       Flow<InfectionState::Infected_V8I8, InfectionState::Infected_V9I8>,
                       Flow<InfectionState::Infected_V8I9, InfectionState::Infected_V7I9>,
                       Flow<InfectionState::Infected_V8I9, InfectionState::Infected_V9I9>,
                       Flow<InfectionState::Infected_V8I10, InfectionState::Infected_V7I10>,
                       Flow<InfectionState::Infected_V8I10, InfectionState::Infected_V9I10>,

                       Flow<InfectionState::Infected_V9I0, InfectionState::Infected_V8I0>,
                       Flow<InfectionState::Infected_V9I0, InfectionState::Infected_V10I0>,
                       Flow<InfectionState::Infected_V9I1, InfectionState::Infected_V8I1>,
                       Flow<InfectionState::Infected_V9I1, InfectionState::Infected_V10I1>,
                       Flow<InfectionState::Infected_V9I2, InfectionState::Infected_V8I2>,
                       Flow<InfectionState::Infected_V9I2, InfectionState::Infected_V10I2>,
                       Flow<InfectionState::Infected_V9I3, InfectionState::Infected_V8I3>,
                       Flow<InfectionState::Infected_V9I3, InfectionState::Infected_V10I3>,
                       Flow<InfectionState::Infected_V9I4, InfectionState::Infected_V8I4>,
                       Flow<InfectionState::Infected_V9I4, InfectionState::Infected_V10I4>,
                       Flow<InfectionState::Infected_V9I5, InfectionState::Infected_V8I5>,
                       Flow<InfectionState::Infected_V9I5, InfectionState::Infected_V10I5>,
                       Flow<InfectionState::Infected_V9I6, InfectionState::Infected_V8I6>,
                       Flow<InfectionState::Infected_V9I6, InfectionState::Infected_V10I6>,
                       Flow<InfectionState::Infected_V9I7, InfectionState::Infected_V8I7>,
                       Flow<InfectionState::Infected_V9I7, InfectionState::Infected_V10I7>,
                       Flow<InfectionState::Infected_V9I8, InfectionState::Infected_V8I8>,
                       Flow<InfectionState::Infected_V9I8, InfectionState::Infected_V10I8>,
                       Flow<InfectionState::Infected_V9I9, InfectionState::Infected_V8I9>,
                       Flow<InfectionState::Infected_V9I9, InfectionState::Infected_V10I9>,
                       Flow<InfectionState::Infected_V9I10, InfectionState::Infected_V8I10>,
                       Flow<InfectionState::Infected_V9I10, InfectionState::Infected_V10I10>,


                       Flow<InfectionState::Infected_V10I0, InfectionState::Infected_V9I0>,
                       Flow<InfectionState::Infected_V10I1, InfectionState::Infected_V9I1>,
                       Flow<InfectionState::Infected_V10I2, InfectionState::Infected_V9I2>,
                       Flow<InfectionState::Infected_V10I3, InfectionState::Infected_V9I3>,
                       Flow<InfectionState::Infected_V10I4, InfectionState::Infected_V9I4>,
                       Flow<InfectionState::Infected_V10I5, InfectionState::Infected_V9I5>,
                       Flow<InfectionState::Infected_V10I6, InfectionState::Infected_V9I6>,
                       Flow<InfectionState::Infected_V10I7, InfectionState::Infected_V9I7>,
                       Flow<InfectionState::Infected_V10I8, InfectionState::Infected_V9I8>,
                       Flow<InfectionState::Infected_V10I9, InfectionState::Infected_V9I9>,
                       Flow<InfectionState::Infected_V10I10, InfectionState::Infected_V9I10>,


                       Flow<InfectionState::Infected_V1I0, InfectionState::Uninfected_I1>, //recovery
                       Flow<InfectionState::Infected_V1I1, InfectionState::Uninfected_I1>,
                       Flow<InfectionState::Infected_V1I2, InfectionState::Uninfected_I2>,
                       Flow<InfectionState::Infected_V1I3, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V1I4, InfectionState::Uninfected_I4>,
                       Flow<InfectionState::Infected_V1I5, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V1I6, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V1I7, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V1I8, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V1I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V1I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V2I0, InfectionState::Uninfected_I2>,
                       Flow<InfectionState::Infected_V2I1, InfectionState::Uninfected_I2>,
                       Flow<InfectionState::Infected_V2I2, InfectionState::Uninfected_I2>,
                       Flow<InfectionState::Infected_V2I3, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V2I4, InfectionState::Uninfected_I4>,
                       Flow<InfectionState::Infected_V2I5, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V2I6, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V2I7, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V2I8, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V2I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V2I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V3I0, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V3I1, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V3I2, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V3I3, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V3I4, InfectionState::Uninfected_I4>,
                       Flow<InfectionState::Infected_V3I5, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V3I6, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V3I7, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V3I8, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V3I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V3I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V4I0, InfectionState::Uninfected_I4>,
                       Flow<InfectionState::Infected_V4I1, InfectionState::Uninfected_I4>,
                       Flow<InfectionState::Infected_V4I2, InfectionState::Uninfected_I4>,
                       Flow<InfectionState::Infected_V4I3, InfectionState::Uninfected_I4>,
                       Flow<InfectionState::Infected_V4I4, InfectionState::Uninfected_I4>,
                       Flow<InfectionState::Infected_V4I5, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V4I6, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V4I7, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V4I8, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V4I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V4I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V5I0, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V5I1, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V5I2, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V5I3, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V5I4, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V5I5, InfectionState::Uninfected_I5>,
                       Flow<InfectionState::Infected_V5I6, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V5I7, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V5I8, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V5I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V5I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V6I0, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V6I1, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V6I2, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V6I3, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V6I4, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V6I5, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V6I6, InfectionState::Uninfected_I6>,
                       Flow<InfectionState::Infected_V6I7, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V6I8, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V6I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V6I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V7I0, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V7I1, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V7I2, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V7I3, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V7I4, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V7I5, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V7I6, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V7I7, InfectionState::Uninfected_I7>,
                       Flow<InfectionState::Infected_V7I8, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V7I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V7I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V8I0, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I1, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I2, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I3, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I4, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I5, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I6, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I7, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I8, InfectionState::Uninfected_I8>,
                       Flow<InfectionState::Infected_V8I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V8I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V9I0, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I1, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I2, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I3, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I4, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I5, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I6, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I7, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I8, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I9, InfectionState::Uninfected_I9>,
                       Flow<InfectionState::Infected_V9I10, InfectionState::Uninfected_I10>,

                       Flow<InfectionState::Infected_V10I0, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I1, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I2, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I3, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I4, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I5, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I6, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I7, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I8, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I9, InfectionState::Uninfected_I10>,
                       Flow<InfectionState::Infected_V10I10, InfectionState::Uninfected_I10>>;

class Model : public FlowModel<ScalarType, InfectionState, Populations<ScalarType, InfectionState>, Parameters, Flows>
{
    using Base = FlowModel<ScalarType, InfectionState, mio::Populations<ScalarType, InfectionState>, Parameters, Flows>;

public:
    Model()
        : Base(Populations({InfectionState::Count}, 0.), ParameterSet())
    {
    }

    void get_flows(Eigen::Ref<const Vector<>> pop,
                   Eigen::Ref<const Vector<>> y, ScalarType t,
                   Eigen::Ref<Vector<>> flows) const
    {
        auto& params         = this->parameters;
        ScalarType coeffStoI_V1 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV1>() / populations.get_total();
        ScalarType coeffStoI_V2 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV2>() / populations.get_total();
        ScalarType coeffStoI_V3 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV3>() / populations.get_total();
        ScalarType coeffStoI_V4 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV4>() / populations.get_total();
        ScalarType coeffStoI_V5 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV5>() / populations.get_total();
        ScalarType coeffStoI_V6 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV6>() / populations.get_total();
        ScalarType coeffStoI_V7 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV7>() / populations.get_total();
        ScalarType coeffStoI_V8 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV8>() / populations.get_total();
        ScalarType coeffStoI_V9 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV9>() / populations.get_total();
        ScalarType coeffStoI_V10 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV10>() / populations.get_total();

        ScalarType sum_infected_v1 = pop[(size_t)InfectionState::Infected_V1I0] + pop[(size_t)InfectionState::Infected_V1I1] 
            + pop[(size_t)InfectionState::Infected_V1I2] + pop[(size_t)InfectionState::Infected_V1I3] 
            + pop[(size_t)InfectionState::Infected_V1I4] + pop[(size_t)InfectionState::Infected_V1I5]
            + pop[(size_t)InfectionState::Infected_V1I6] + pop[(size_t)InfectionState::Infected_V1I7]
            + pop[(size_t)InfectionState::Infected_V1I8] + pop[(size_t)InfectionState::Infected_V1I9]
            + pop[(size_t)InfectionState::Infected_V1I10];
        ScalarType sum_infected_v2 = pop[(size_t)InfectionState::Infected_V2I0] + pop[(size_t)InfectionState::Infected_V2I1] 
            + pop[(size_t)InfectionState::Infected_V2I2] + pop[(size_t)InfectionState::Infected_V2I3] 
            + pop[(size_t)InfectionState::Infected_V2I4] + pop[(size_t)InfectionState::Infected_V2I5]
            + pop[(size_t)InfectionState::Infected_V2I6] + pop[(size_t)InfectionState::Infected_V2I7]
            + pop[(size_t)InfectionState::Infected_V2I8] + pop[(size_t)InfectionState::Infected_V2I9]
            + pop[(size_t)InfectionState::Infected_V2I10];
        ScalarType sum_infected_v3 = pop[(size_t)InfectionState::Infected_V3I0] + pop[(size_t)InfectionState::Infected_V3I1] 
            + pop[(size_t)InfectionState::Infected_V3I2] + pop[(size_t)InfectionState::Infected_V3I3] 
            + pop[(size_t)InfectionState::Infected_V3I4] + pop[(size_t)InfectionState::Infected_V3I5]
            + pop[(size_t)InfectionState::Infected_V3I6] + pop[(size_t)InfectionState::Infected_V3I7]
            + pop[(size_t)InfectionState::Infected_V3I8] + pop[(size_t)InfectionState::Infected_V3I9]
            + pop[(size_t)InfectionState::Infected_V3I10];
        ScalarType sum_infected_v4 = pop[(size_t)InfectionState::Infected_V4I0] + pop[(size_t)InfectionState::Infected_V4I1] 
            + pop[(size_t)InfectionState::Infected_V4I2] + pop[(size_t)InfectionState::Infected_V4I3] 
            + pop[(size_t)InfectionState::Infected_V4I4] + pop[(size_t)InfectionState::Infected_V4I5]
            + pop[(size_t)InfectionState::Infected_V4I6] + pop[(size_t)InfectionState::Infected_V4I7]
            + pop[(size_t)InfectionState::Infected_V4I8] + pop[(size_t)InfectionState::Infected_V4I9]
            + pop[(size_t)InfectionState::Infected_V4I10];
        ScalarType sum_infected_v5 = pop[(size_t)InfectionState::Infected_V5I0] + pop[(size_t)InfectionState::Infected_V5I1] 
            + pop[(size_t)InfectionState::Infected_V5I2] + pop[(size_t)InfectionState::Infected_V5I3] 
            + pop[(size_t)InfectionState::Infected_V5I4] + pop[(size_t)InfectionState::Infected_V5I5]
            + pop[(size_t)InfectionState::Infected_V5I6] + pop[(size_t)InfectionState::Infected_V5I7]
            + pop[(size_t)InfectionState::Infected_V5I8] + pop[(size_t)InfectionState::Infected_V5I9]
            + pop[(size_t)InfectionState::Infected_V5I10];
        ScalarType sum_infected_v6 = pop[(size_t)InfectionState::Infected_V6I0] + pop[(size_t)InfectionState::Infected_V6I1] 
            + pop[(size_t)InfectionState::Infected_V6I2] + pop[(size_t)InfectionState::Infected_V6I3] 
            + pop[(size_t)InfectionState::Infected_V6I4] + pop[(size_t)InfectionState::Infected_V6I5]
            + pop[(size_t)InfectionState::Infected_V6I6] + pop[(size_t)InfectionState::Infected_V6I7]
            + pop[(size_t)InfectionState::Infected_V6I8] + pop[(size_t)InfectionState::Infected_V6I9]
            + pop[(size_t)InfectionState::Infected_V6I10];
        ScalarType sum_infected_v7 = pop[(size_t)InfectionState::Infected_V7I0] + pop[(size_t)InfectionState::Infected_V7I1] 
            + pop[(size_t)InfectionState::Infected_V7I2] + pop[(size_t)InfectionState::Infected_V7I3] 
            + pop[(size_t)InfectionState::Infected_V7I4] + pop[(size_t)InfectionState::Infected_V7I5]
            + pop[(size_t)InfectionState::Infected_V7I6] + pop[(size_t)InfectionState::Infected_V7I7]
            + pop[(size_t)InfectionState::Infected_V7I8] + pop[(size_t)InfectionState::Infected_V7I9]
            + pop[(size_t)InfectionState::Infected_V7I10];
        ScalarType sum_infected_v8 = pop[(size_t)InfectionState::Infected_V8I0] + pop[(size_t)InfectionState::Infected_V8I1] 
            + pop[(size_t)InfectionState::Infected_V8I2] + pop[(size_t)InfectionState::Infected_V8I3] 
            + pop[(size_t)InfectionState::Infected_V8I4] + pop[(size_t)InfectionState::Infected_V8I5]
            + pop[(size_t)InfectionState::Infected_V8I6] + pop[(size_t)InfectionState::Infected_V8I7]
            + pop[(size_t)InfectionState::Infected_V8I8] + pop[(size_t)InfectionState::Infected_V8I9]
            + pop[(size_t)InfectionState::Infected_V8I10];
        ScalarType sum_infected_v9 = pop[(size_t)InfectionState::Infected_V9I0] + pop[(size_t)InfectionState::Infected_V9I1] 
            + pop[(size_t)InfectionState::Infected_V9I2] + pop[(size_t)InfectionState::Infected_V9I3] 
            + pop[(size_t)InfectionState::Infected_V9I4] + pop[(size_t)InfectionState::Infected_V9I5]
            + pop[(size_t)InfectionState::Infected_V9I6] + pop[(size_t)InfectionState::Infected_V9I7]
            + pop[(size_t)InfectionState::Infected_V9I8] + pop[(size_t)InfectionState::Infected_V9I9]
            + pop[(size_t)InfectionState::Infected_V9I10];
        ScalarType sum_infected_v10 = pop[(size_t)InfectionState::Infected_V10I0] + pop[(size_t)InfectionState::Infected_V10I1] 
            + pop[(size_t)InfectionState::Infected_V10I2] + pop[(size_t)InfectionState::Infected_V10I3] 
            + pop[(size_t)InfectionState::Infected_V10I4] + pop[(size_t)InfectionState::Infected_V10I5]
            + pop[(size_t)InfectionState::Infected_V10I6] + pop[(size_t)InfectionState::Infected_V10I7]
            + pop[(size_t)InfectionState::Infected_V10I8] + pop[(size_t)InfectionState::Infected_V10I9]
            + pop[(size_t)InfectionState::Infected_V10I10];      


        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V1I0>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V2I0>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V3I0>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V4I0>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V5I0>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V6I0>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V7I0>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V8I0>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V9I0>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V10I0>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V1I1>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V2I1>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V3I1>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V4I1>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V5I1>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V6I1>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V7I1>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V8I1>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V9I1>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V10I1>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v10;
        
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V1I2>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V2I2>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V3I2>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V4I2>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V5I2>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V6I2>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V7I2>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V8I2>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V9I2>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V10I2>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v10;
        
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V1I3>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V2I3>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V3I3>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V4I3>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V5I3>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V6I3>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V7I3>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V8I3>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V9I3>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V10I3>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V1I4>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V2I4>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V3I4>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V4I4>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V5I4>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V6I4>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V7I4>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V8I4>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V9I4>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V10I4>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V1I5>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V2I5>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V3I5>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V4I5>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V5I5>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V6I5>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V7I5>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V8I5>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V9I5>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V10I5>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V1I6>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V2I6>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V3I6>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V4I6>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V5I6>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V6I6>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V7I6>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V8I6>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V9I6>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V10I6>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V1I7>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V2I7>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V3I7>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V4I7>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V5I7>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V6I7>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V7I7>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V8I7>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V9I7>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V10I7>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V1I8>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V2I8>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V3I8>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V4I8>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V5I8>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V6I8>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V7I8>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V8I8>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V9I8>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V10I8>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V1I9>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V2I9>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V3I9>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V4I9>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V5I9>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V6I9>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V7I9>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V8I9>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V9I9>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V10I9>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V1I10>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V2I10>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V3I10>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V4I10>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V5I10>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V6I10>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V7I10>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V8I10>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V9I10>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V10I10>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v10;
        
        flows[get_flat_flow_index<InfectionState::Infected_V1I0, InfectionState::Infected_V2I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I0];
        flows[get_flat_flow_index<InfectionState::Infected_V1I1, InfectionState::Infected_V2I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I1];
        flows[get_flat_flow_index<InfectionState::Infected_V1I2, InfectionState::Infected_V2I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I2];
        flows[get_flat_flow_index<InfectionState::Infected_V1I3, InfectionState::Infected_V2I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I3];
        flows[get_flat_flow_index<InfectionState::Infected_V1I4, InfectionState::Infected_V2I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I4];
        flows[get_flat_flow_index<InfectionState::Infected_V1I5, InfectionState::Infected_V2I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I5];
        flows[get_flat_flow_index<InfectionState::Infected_V1I6, InfectionState::Infected_V2I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I6];
        flows[get_flat_flow_index<InfectionState::Infected_V1I7, InfectionState::Infected_V2I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I7];
        flows[get_flat_flow_index<InfectionState::Infected_V1I8, InfectionState::Infected_V2I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I8];
        flows[get_flat_flow_index<InfectionState::Infected_V1I9, InfectionState::Infected_V2I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I9];
        flows[get_flat_flow_index<InfectionState::Infected_V1I10, InfectionState::Infected_V2I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Infected_V1I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Infected_V3I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Infected_V1I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Infected_V3I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Infected_V1I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Infected_V3I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Infected_V1I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Infected_V3I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I4, InfectionState::Infected_V1I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I4];
        flows[get_flat_flow_index<InfectionState::Infected_V2I4, InfectionState::Infected_V3I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I4];
        flows[get_flat_flow_index<InfectionState::Infected_V2I5, InfectionState::Infected_V1I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I5];
        flows[get_flat_flow_index<InfectionState::Infected_V2I5, InfectionState::Infected_V3I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I5];
        flows[get_flat_flow_index<InfectionState::Infected_V2I6, InfectionState::Infected_V1I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I6];
        flows[get_flat_flow_index<InfectionState::Infected_V2I6, InfectionState::Infected_V3I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I6];
        flows[get_flat_flow_index<InfectionState::Infected_V2I7, InfectionState::Infected_V1I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I7];
        flows[get_flat_flow_index<InfectionState::Infected_V2I7, InfectionState::Infected_V3I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I7];
        flows[get_flat_flow_index<InfectionState::Infected_V2I8, InfectionState::Infected_V1I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I8];
        flows[get_flat_flow_index<InfectionState::Infected_V2I8, InfectionState::Infected_V3I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I8];
        flows[get_flat_flow_index<InfectionState::Infected_V2I9, InfectionState::Infected_V1I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I9];
        flows[get_flat_flow_index<InfectionState::Infected_V2I9, InfectionState::Infected_V3I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I9];
        flows[get_flat_flow_index<InfectionState::Infected_V2I10, InfectionState::Infected_V1I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I10];
        flows[get_flat_flow_index<InfectionState::Infected_V2I10, InfectionState::Infected_V3I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V3I0, InfectionState::Infected_V2I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I0];
        flows[get_flat_flow_index<InfectionState::Infected_V3I0, InfectionState::Infected_V4I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I0];
        flows[get_flat_flow_index<InfectionState::Infected_V3I1, InfectionState::Infected_V2I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I1];
        flows[get_flat_flow_index<InfectionState::Infected_V3I1, InfectionState::Infected_V4I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I1];
        flows[get_flat_flow_index<InfectionState::Infected_V3I2, InfectionState::Infected_V2I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I2];
        flows[get_flat_flow_index<InfectionState::Infected_V3I2, InfectionState::Infected_V4I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I2];
        flows[get_flat_flow_index<InfectionState::Infected_V3I3, InfectionState::Infected_V2I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I3];
        flows[get_flat_flow_index<InfectionState::Infected_V3I3, InfectionState::Infected_V4I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I3];
        flows[get_flat_flow_index<InfectionState::Infected_V3I4, InfectionState::Infected_V2I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I4];
        flows[get_flat_flow_index<InfectionState::Infected_V3I4, InfectionState::Infected_V4I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I4];
        flows[get_flat_flow_index<InfectionState::Infected_V3I5, InfectionState::Infected_V2I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I5];
        flows[get_flat_flow_index<InfectionState::Infected_V3I5, InfectionState::Infected_V4I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I5];
        flows[get_flat_flow_index<InfectionState::Infected_V3I6, InfectionState::Infected_V2I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I6];
        flows[get_flat_flow_index<InfectionState::Infected_V3I6, InfectionState::Infected_V4I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I6];
        flows[get_flat_flow_index<InfectionState::Infected_V3I7, InfectionState::Infected_V2I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I7];
        flows[get_flat_flow_index<InfectionState::Infected_V3I7, InfectionState::Infected_V4I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I7];
        flows[get_flat_flow_index<InfectionState::Infected_V3I8, InfectionState::Infected_V2I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I8];
        flows[get_flat_flow_index<InfectionState::Infected_V3I8, InfectionState::Infected_V4I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I8];
        flows[get_flat_flow_index<InfectionState::Infected_V3I9, InfectionState::Infected_V2I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I9];
        flows[get_flat_flow_index<InfectionState::Infected_V3I9, InfectionState::Infected_V4I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I9];
        flows[get_flat_flow_index<InfectionState::Infected_V3I10, InfectionState::Infected_V2I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I10];
        flows[get_flat_flow_index<InfectionState::Infected_V3I10, InfectionState::Infected_V4I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V4I0, InfectionState::Infected_V3I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I0];
        flows[get_flat_flow_index<InfectionState::Infected_V4I0, InfectionState::Infected_V5I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I0];
        flows[get_flat_flow_index<InfectionState::Infected_V4I1, InfectionState::Infected_V3I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I1];
        flows[get_flat_flow_index<InfectionState::Infected_V4I1, InfectionState::Infected_V5I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I1];
        flows[get_flat_flow_index<InfectionState::Infected_V4I2, InfectionState::Infected_V3I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I2];
        flows[get_flat_flow_index<InfectionState::Infected_V4I2, InfectionState::Infected_V5I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I2];
        flows[get_flat_flow_index<InfectionState::Infected_V4I3, InfectionState::Infected_V3I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I3];
        flows[get_flat_flow_index<InfectionState::Infected_V4I3, InfectionState::Infected_V5I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I3];
        flows[get_flat_flow_index<InfectionState::Infected_V4I4, InfectionState::Infected_V3I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I4];
        flows[get_flat_flow_index<InfectionState::Infected_V4I4, InfectionState::Infected_V5I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I4];
        flows[get_flat_flow_index<InfectionState::Infected_V4I5, InfectionState::Infected_V3I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I5];
        flows[get_flat_flow_index<InfectionState::Infected_V4I5, InfectionState::Infected_V5I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I5];
        flows[get_flat_flow_index<InfectionState::Infected_V4I6, InfectionState::Infected_V3I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I6];
        flows[get_flat_flow_index<InfectionState::Infected_V4I6, InfectionState::Infected_V5I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I6];
        flows[get_flat_flow_index<InfectionState::Infected_V4I7, InfectionState::Infected_V3I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I7];
        flows[get_flat_flow_index<InfectionState::Infected_V4I7, InfectionState::Infected_V5I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I7];
        flows[get_flat_flow_index<InfectionState::Infected_V4I8, InfectionState::Infected_V3I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I8];
        flows[get_flat_flow_index<InfectionState::Infected_V4I8, InfectionState::Infected_V5I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I8];
        flows[get_flat_flow_index<InfectionState::Infected_V4I9, InfectionState::Infected_V3I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I9];
        flows[get_flat_flow_index<InfectionState::Infected_V4I9, InfectionState::Infected_V5I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I9];
        flows[get_flat_flow_index<InfectionState::Infected_V4I10, InfectionState::Infected_V3I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I10];
        flows[get_flat_flow_index<InfectionState::Infected_V4I10, InfectionState::Infected_V5I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V5I0, InfectionState::Infected_V4I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I0];
        flows[get_flat_flow_index<InfectionState::Infected_V5I0, InfectionState::Infected_V6I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I0];
        flows[get_flat_flow_index<InfectionState::Infected_V5I1, InfectionState::Infected_V4I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I1];
        flows[get_flat_flow_index<InfectionState::Infected_V5I1, InfectionState::Infected_V6I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I1];
        flows[get_flat_flow_index<InfectionState::Infected_V5I2, InfectionState::Infected_V4I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I2];
        flows[get_flat_flow_index<InfectionState::Infected_V5I2, InfectionState::Infected_V6I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I2];
        flows[get_flat_flow_index<InfectionState::Infected_V5I3, InfectionState::Infected_V4I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I3];
        flows[get_flat_flow_index<InfectionState::Infected_V5I3, InfectionState::Infected_V6I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I3];
        flows[get_flat_flow_index<InfectionState::Infected_V5I4, InfectionState::Infected_V4I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I4];
        flows[get_flat_flow_index<InfectionState::Infected_V5I4, InfectionState::Infected_V6I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I4];
        flows[get_flat_flow_index<InfectionState::Infected_V5I5, InfectionState::Infected_V4I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I5];
        flows[get_flat_flow_index<InfectionState::Infected_V5I5, InfectionState::Infected_V6I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I5];
        flows[get_flat_flow_index<InfectionState::Infected_V5I6, InfectionState::Infected_V4I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I6];
        flows[get_flat_flow_index<InfectionState::Infected_V5I6, InfectionState::Infected_V6I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I6];
        flows[get_flat_flow_index<InfectionState::Infected_V5I7, InfectionState::Infected_V4I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I7];
        flows[get_flat_flow_index<InfectionState::Infected_V5I7, InfectionState::Infected_V6I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I7];
        flows[get_flat_flow_index<InfectionState::Infected_V5I8, InfectionState::Infected_V4I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I8];
        flows[get_flat_flow_index<InfectionState::Infected_V5I8, InfectionState::Infected_V6I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I8];
        flows[get_flat_flow_index<InfectionState::Infected_V5I9, InfectionState::Infected_V4I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I9];
        flows[get_flat_flow_index<InfectionState::Infected_V5I9, InfectionState::Infected_V6I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I9];
        flows[get_flat_flow_index<InfectionState::Infected_V5I10, InfectionState::Infected_V4I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I10];
        flows[get_flat_flow_index<InfectionState::Infected_V5I10, InfectionState::Infected_V6I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V6I0, InfectionState::Infected_V5I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I0];
        flows[get_flat_flow_index<InfectionState::Infected_V6I0, InfectionState::Infected_V7I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I0];
        flows[get_flat_flow_index<InfectionState::Infected_V6I1, InfectionState::Infected_V5I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I1];
        flows[get_flat_flow_index<InfectionState::Infected_V6I1, InfectionState::Infected_V7I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I1];
        flows[get_flat_flow_index<InfectionState::Infected_V6I2, InfectionState::Infected_V5I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I2];
        flows[get_flat_flow_index<InfectionState::Infected_V6I2, InfectionState::Infected_V7I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I2];
        flows[get_flat_flow_index<InfectionState::Infected_V6I3, InfectionState::Infected_V5I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I3];
        flows[get_flat_flow_index<InfectionState::Infected_V6I3, InfectionState::Infected_V7I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I3];
        flows[get_flat_flow_index<InfectionState::Infected_V6I4, InfectionState::Infected_V5I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I4];
        flows[get_flat_flow_index<InfectionState::Infected_V6I4, InfectionState::Infected_V7I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I4];
        flows[get_flat_flow_index<InfectionState::Infected_V6I5, InfectionState::Infected_V5I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I5];
        flows[get_flat_flow_index<InfectionState::Infected_V6I5, InfectionState::Infected_V7I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I5];
        flows[get_flat_flow_index<InfectionState::Infected_V6I6, InfectionState::Infected_V5I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I6];
        flows[get_flat_flow_index<InfectionState::Infected_V6I6, InfectionState::Infected_V7I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I6];
        flows[get_flat_flow_index<InfectionState::Infected_V6I7, InfectionState::Infected_V5I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I7];
        flows[get_flat_flow_index<InfectionState::Infected_V6I7, InfectionState::Infected_V7I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I7];
        flows[get_flat_flow_index<InfectionState::Infected_V6I8, InfectionState::Infected_V5I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I8];
        flows[get_flat_flow_index<InfectionState::Infected_V6I8, InfectionState::Infected_V7I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I8];
        flows[get_flat_flow_index<InfectionState::Infected_V6I9, InfectionState::Infected_V5I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I9];
        flows[get_flat_flow_index<InfectionState::Infected_V6I9, InfectionState::Infected_V7I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I9];
        flows[get_flat_flow_index<InfectionState::Infected_V6I10, InfectionState::Infected_V5I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I10];
        flows[get_flat_flow_index<InfectionState::Infected_V6I10, InfectionState::Infected_V7I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I10];

        flows[get_flat_flow_index<InfectionState::Infected_V7I0, InfectionState::Infected_V6I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I0];
        flows[get_flat_flow_index<InfectionState::Infected_V7I0, InfectionState::Infected_V8I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I0];
        flows[get_flat_flow_index<InfectionState::Infected_V7I1, InfectionState::Infected_V6I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I1];
        flows[get_flat_flow_index<InfectionState::Infected_V7I1, InfectionState::Infected_V8I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I1];
        flows[get_flat_flow_index<InfectionState::Infected_V7I2, InfectionState::Infected_V6I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I2];
        flows[get_flat_flow_index<InfectionState::Infected_V7I2, InfectionState::Infected_V8I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I2];
        flows[get_flat_flow_index<InfectionState::Infected_V7I3, InfectionState::Infected_V6I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I3];
        flows[get_flat_flow_index<InfectionState::Infected_V7I3, InfectionState::Infected_V8I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I3];
        flows[get_flat_flow_index<InfectionState::Infected_V7I4, InfectionState::Infected_V6I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I4];
        flows[get_flat_flow_index<InfectionState::Infected_V7I4, InfectionState::Infected_V8I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I4];
        flows[get_flat_flow_index<InfectionState::Infected_V7I5, InfectionState::Infected_V6I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I5];
        flows[get_flat_flow_index<InfectionState::Infected_V7I5, InfectionState::Infected_V8I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I5];
        flows[get_flat_flow_index<InfectionState::Infected_V7I6, InfectionState::Infected_V6I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I6];
        flows[get_flat_flow_index<InfectionState::Infected_V7I6, InfectionState::Infected_V8I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I6];
        flows[get_flat_flow_index<InfectionState::Infected_V7I7, InfectionState::Infected_V6I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I7];
        flows[get_flat_flow_index<InfectionState::Infected_V7I7, InfectionState::Infected_V8I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I7];
        flows[get_flat_flow_index<InfectionState::Infected_V7I8, InfectionState::Infected_V6I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I8];
        flows[get_flat_flow_index<InfectionState::Infected_V7I8, InfectionState::Infected_V8I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I8];
        flows[get_flat_flow_index<InfectionState::Infected_V7I9, InfectionState::Infected_V6I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I9];
        flows[get_flat_flow_index<InfectionState::Infected_V7I9, InfectionState::Infected_V8I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I9];
        flows[get_flat_flow_index<InfectionState::Infected_V7I10, InfectionState::Infected_V6I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I10];
        flows[get_flat_flow_index<InfectionState::Infected_V7I10, InfectionState::Infected_V8I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V8I0, InfectionState::Infected_V7I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I0];
        flows[get_flat_flow_index<InfectionState::Infected_V8I0, InfectionState::Infected_V9I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I0];
        flows[get_flat_flow_index<InfectionState::Infected_V8I1, InfectionState::Infected_V7I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I1];
        flows[get_flat_flow_index<InfectionState::Infected_V8I1, InfectionState::Infected_V9I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I1];
        flows[get_flat_flow_index<InfectionState::Infected_V8I2, InfectionState::Infected_V7I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I2];
        flows[get_flat_flow_index<InfectionState::Infected_V8I2, InfectionState::Infected_V9I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I2];
        flows[get_flat_flow_index<InfectionState::Infected_V8I3, InfectionState::Infected_V7I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I3];
        flows[get_flat_flow_index<InfectionState::Infected_V8I3, InfectionState::Infected_V9I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I3];
        flows[get_flat_flow_index<InfectionState::Infected_V8I4, InfectionState::Infected_V7I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I4];
        flows[get_flat_flow_index<InfectionState::Infected_V8I4, InfectionState::Infected_V9I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I4];
        flows[get_flat_flow_index<InfectionState::Infected_V8I5, InfectionState::Infected_V7I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I5];
        flows[get_flat_flow_index<InfectionState::Infected_V8I5, InfectionState::Infected_V9I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I5];
        flows[get_flat_flow_index<InfectionState::Infected_V8I6, InfectionState::Infected_V7I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I6];
        flows[get_flat_flow_index<InfectionState::Infected_V8I6, InfectionState::Infected_V9I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I6];
        flows[get_flat_flow_index<InfectionState::Infected_V8I7, InfectionState::Infected_V7I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I7];
        flows[get_flat_flow_index<InfectionState::Infected_V8I7, InfectionState::Infected_V9I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I7];
        flows[get_flat_flow_index<InfectionState::Infected_V8I8, InfectionState::Infected_V7I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I8];
        flows[get_flat_flow_index<InfectionState::Infected_V8I8, InfectionState::Infected_V9I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I8];
        flows[get_flat_flow_index<InfectionState::Infected_V8I9, InfectionState::Infected_V7I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I9];
        flows[get_flat_flow_index<InfectionState::Infected_V8I9, InfectionState::Infected_V9I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I9];
        flows[get_flat_flow_index<InfectionState::Infected_V8I10, InfectionState::Infected_V7I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I10];
        flows[get_flat_flow_index<InfectionState::Infected_V8I10, InfectionState::Infected_V9I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V9I0, InfectionState::Infected_V8I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I0];
        flows[get_flat_flow_index<InfectionState::Infected_V9I0, InfectionState::Infected_V10I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I0];
        flows[get_flat_flow_index<InfectionState::Infected_V9I1, InfectionState::Infected_V8I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I1];
        flows[get_flat_flow_index<InfectionState::Infected_V9I1, InfectionState::Infected_V10I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I1];
        flows[get_flat_flow_index<InfectionState::Infected_V9I2, InfectionState::Infected_V8I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I2];
        flows[get_flat_flow_index<InfectionState::Infected_V9I2, InfectionState::Infected_V10I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I2];
        flows[get_flat_flow_index<InfectionState::Infected_V9I3, InfectionState::Infected_V8I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I3];
        flows[get_flat_flow_index<InfectionState::Infected_V9I3, InfectionState::Infected_V10I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I3];
        flows[get_flat_flow_index<InfectionState::Infected_V9I4, InfectionState::Infected_V8I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I4];
        flows[get_flat_flow_index<InfectionState::Infected_V9I4, InfectionState::Infected_V10I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I4];
        flows[get_flat_flow_index<InfectionState::Infected_V9I5, InfectionState::Infected_V8I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I5];
        flows[get_flat_flow_index<InfectionState::Infected_V9I5, InfectionState::Infected_V10I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I5];
        flows[get_flat_flow_index<InfectionState::Infected_V9I6, InfectionState::Infected_V8I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I6];
        flows[get_flat_flow_index<InfectionState::Infected_V9I6, InfectionState::Infected_V10I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I6];
        flows[get_flat_flow_index<InfectionState::Infected_V9I7, InfectionState::Infected_V8I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I7];
        flows[get_flat_flow_index<InfectionState::Infected_V9I7, InfectionState::Infected_V10I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I7];
        flows[get_flat_flow_index<InfectionState::Infected_V9I8, InfectionState::Infected_V8I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I8];
        flows[get_flat_flow_index<InfectionState::Infected_V9I8, InfectionState::Infected_V10I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I8];
        flows[get_flat_flow_index<InfectionState::Infected_V9I9, InfectionState::Infected_V8I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I9];
        flows[get_flat_flow_index<InfectionState::Infected_V9I9, InfectionState::Infected_V10I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I9];
        flows[get_flat_flow_index<InfectionState::Infected_V9I10, InfectionState::Infected_V8I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I10];
        flows[get_flat_flow_index<InfectionState::Infected_V9I10, InfectionState::Infected_V10I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V10I0, InfectionState::Infected_V9I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I0];
        flows[get_flat_flow_index<InfectionState::Infected_V10I1, InfectionState::Infected_V9I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I1];
        flows[get_flat_flow_index<InfectionState::Infected_V10I2, InfectionState::Infected_V9I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I2];
        flows[get_flat_flow_index<InfectionState::Infected_V10I3, InfectionState::Infected_V9I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I3];
        flows[get_flat_flow_index<InfectionState::Infected_V10I4, InfectionState::Infected_V9I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I4];
        flows[get_flat_flow_index<InfectionState::Infected_V10I5, InfectionState::Infected_V9I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I5];
        flows[get_flat_flow_index<InfectionState::Infected_V10I6, InfectionState::Infected_V9I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I6];
        flows[get_flat_flow_index<InfectionState::Infected_V10I7, InfectionState::Infected_V9I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I7];
        flows[get_flat_flow_index<InfectionState::Infected_V10I8, InfectionState::Infected_V9I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I8];
        flows[get_flat_flow_index<InfectionState::Infected_V10I9, InfectionState::Infected_V9I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I9];
        flows[get_flat_flow_index<InfectionState::Infected_V10I10, InfectionState::Infected_V9I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I10];

        flows[get_flat_flow_index<InfectionState::Infected_V1I0, InfectionState::Uninfected_I1>()] = (1.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I0];
        flows[get_flat_flow_index<InfectionState::Infected_V1I1, InfectionState::Uninfected_I1>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I1];
        flows[get_flat_flow_index<InfectionState::Infected_V1I2, InfectionState::Uninfected_I2>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I2];
        flows[get_flat_flow_index<InfectionState::Infected_V1I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I3];
        flows[get_flat_flow_index<InfectionState::Infected_V1I4, InfectionState::Uninfected_I4>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I4];
        flows[get_flat_flow_index<InfectionState::Infected_V1I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I5];
        flows[get_flat_flow_index<InfectionState::Infected_V1I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I6];
        flows[get_flat_flow_index<InfectionState::Infected_V1I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I6];
        flows[get_flat_flow_index<InfectionState::Infected_V1I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I8];
        flows[get_flat_flow_index<InfectionState::Infected_V1I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I9];
        flows[get_flat_flow_index<InfectionState::Infected_V1I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I10];

        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Uninfected_I2>()] = (1.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Uninfected_I2>()] = (1.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Uninfected_I2>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I4, InfectionState::Uninfected_I4>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I4];
        flows[get_flat_flow_index<InfectionState::Infected_V2I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I5];
        flows[get_flat_flow_index<InfectionState::Infected_V2I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I6];
        flows[get_flat_flow_index<InfectionState::Infected_V2I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I6];
        flows[get_flat_flow_index<InfectionState::Infected_V2I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I8];
        flows[get_flat_flow_index<InfectionState::Infected_V2I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I9];
        flows[get_flat_flow_index<InfectionState::Infected_V2I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V3I0, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V3I0];
        flows[get_flat_flow_index<InfectionState::Infected_V3I1, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V3I1];
        flows[get_flat_flow_index<InfectionState::Infected_V3I2, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I2];
        flows[get_flat_flow_index<InfectionState::Infected_V3I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I3];
        flows[get_flat_flow_index<InfectionState::Infected_V3I4, InfectionState::Uninfected_I4>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I4];
        flows[get_flat_flow_index<InfectionState::Infected_V3I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I5];
        flows[get_flat_flow_index<InfectionState::Infected_V3I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I6];
        flows[get_flat_flow_index<InfectionState::Infected_V3I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I6];
        flows[get_flat_flow_index<InfectionState::Infected_V3I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I8];
        flows[get_flat_flow_index<InfectionState::Infected_V3I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I9];
        flows[get_flat_flow_index<InfectionState::Infected_V3I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V4I0, InfectionState::Uninfected_I4>()] = (1.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I0];
        flows[get_flat_flow_index<InfectionState::Infected_V4I1, InfectionState::Uninfected_I4>()] = (1.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I1];
        flows[get_flat_flow_index<InfectionState::Infected_V4I2, InfectionState::Uninfected_I4>()] = (1.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I2];
        flows[get_flat_flow_index<InfectionState::Infected_V4I3, InfectionState::Uninfected_I4>()] = (1.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I3];
        flows[get_flat_flow_index<InfectionState::Infected_V4I4, InfectionState::Uninfected_I4>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I4];
        flows[get_flat_flow_index<InfectionState::Infected_V4I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I5];
        flows[get_flat_flow_index<InfectionState::Infected_V4I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I6];
        flows[get_flat_flow_index<InfectionState::Infected_V4I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I6];
        flows[get_flat_flow_index<InfectionState::Infected_V4I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I8];
        flows[get_flat_flow_index<InfectionState::Infected_V4I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I9];
        flows[get_flat_flow_index<InfectionState::Infected_V4I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V5I0, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I0];
        flows[get_flat_flow_index<InfectionState::Infected_V5I1, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I1];
        flows[get_flat_flow_index<InfectionState::Infected_V5I2, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I2];
        flows[get_flat_flow_index<InfectionState::Infected_V5I3, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I3];
        flows[get_flat_flow_index<InfectionState::Infected_V5I4, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I4];
        flows[get_flat_flow_index<InfectionState::Infected_V5I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I5];
        flows[get_flat_flow_index<InfectionState::Infected_V5I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I6];
        flows[get_flat_flow_index<InfectionState::Infected_V5I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I6];
        flows[get_flat_flow_index<InfectionState::Infected_V5I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I8];
        flows[get_flat_flow_index<InfectionState::Infected_V5I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I9];
        flows[get_flat_flow_index<InfectionState::Infected_V5I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I10];
    
        flows[get_flat_flow_index<InfectionState::Infected_V6I0, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I0];
        flows[get_flat_flow_index<InfectionState::Infected_V6I1, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I1];
        flows[get_flat_flow_index<InfectionState::Infected_V6I2, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I2];
        flows[get_flat_flow_index<InfectionState::Infected_V6I3, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I3];
        flows[get_flat_flow_index<InfectionState::Infected_V6I4, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I4];
        flows[get_flat_flow_index<InfectionState::Infected_V6I5, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I5];
        flows[get_flat_flow_index<InfectionState::Infected_V6I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I6];
        flows[get_flat_flow_index<InfectionState::Infected_V6I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I6];
        flows[get_flat_flow_index<InfectionState::Infected_V6I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I8];
        flows[get_flat_flow_index<InfectionState::Infected_V6I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I9];
        flows[get_flat_flow_index<InfectionState::Infected_V6I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I10];
    
        flows[get_flat_flow_index<InfectionState::Infected_V7I0, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I0];
        flows[get_flat_flow_index<InfectionState::Infected_V7I1, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I1];
        flows[get_flat_flow_index<InfectionState::Infected_V7I2, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I2];
        flows[get_flat_flow_index<InfectionState::Infected_V7I3, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I3];
        flows[get_flat_flow_index<InfectionState::Infected_V7I4, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I4];
        flows[get_flat_flow_index<InfectionState::Infected_V7I5, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I5];
        flows[get_flat_flow_index<InfectionState::Infected_V7I6, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I6];
        flows[get_flat_flow_index<InfectionState::Infected_V7I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I6];
        flows[get_flat_flow_index<InfectionState::Infected_V7I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I8];
        flows[get_flat_flow_index<InfectionState::Infected_V7I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I9];
        flows[get_flat_flow_index<InfectionState::Infected_V7I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V8I0, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I0];
        flows[get_flat_flow_index<InfectionState::Infected_V8I1, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I1];
        flows[get_flat_flow_index<InfectionState::Infected_V8I2, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I2];
        flows[get_flat_flow_index<InfectionState::Infected_V8I3, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I3];
        flows[get_flat_flow_index<InfectionState::Infected_V8I4, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I4];
        flows[get_flat_flow_index<InfectionState::Infected_V8I5, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I5];
        flows[get_flat_flow_index<InfectionState::Infected_V8I6, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I6];
        flows[get_flat_flow_index<InfectionState::Infected_V8I7, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I6];
        flows[get_flat_flow_index<InfectionState::Infected_V8I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I8];
        flows[get_flat_flow_index<InfectionState::Infected_V8I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I9];
        flows[get_flat_flow_index<InfectionState::Infected_V8I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V9I0, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I0];
        flows[get_flat_flow_index<InfectionState::Infected_V9I1, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I1];
        flows[get_flat_flow_index<InfectionState::Infected_V9I2, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I2];
        flows[get_flat_flow_index<InfectionState::Infected_V9I3, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I3];
        flows[get_flat_flow_index<InfectionState::Infected_V9I4, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I4];
        flows[get_flat_flow_index<InfectionState::Infected_V9I5, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I5];
        flows[get_flat_flow_index<InfectionState::Infected_V9I6, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I6];
        flows[get_flat_flow_index<InfectionState::Infected_V9I7, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I6];
        flows[get_flat_flow_index<InfectionState::Infected_V9I8, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I8];
        flows[get_flat_flow_index<InfectionState::Infected_V9I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I9];
        flows[get_flat_flow_index<InfectionState::Infected_V9I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V10I0, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I0];
        flows[get_flat_flow_index<InfectionState::Infected_V10I1, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I1];
        flows[get_flat_flow_index<InfectionState::Infected_V10I2, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I2];
        flows[get_flat_flow_index<InfectionState::Infected_V10I3, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I3];
        flows[get_flat_flow_index<InfectionState::Infected_V10I4, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I4];
        flows[get_flat_flow_index<InfectionState::Infected_V10I5, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I5];
        flows[get_flat_flow_index<InfectionState::Infected_V10I6, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I6];
        flows[get_flat_flow_index<InfectionState::Infected_V10I7, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I6];
        flows[get_flat_flow_index<InfectionState::Infected_V10I8, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I8];
        flows[get_flat_flow_index<InfectionState::Infected_V10I9, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I9];
        flows[get_flat_flow_index<InfectionState::Infected_V10I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I10];
       }

    void get_flows_denoised(Eigen::Ref<const Vector<>> pop,
                   Eigen::Ref<const Vector<>> y, ScalarType t,
                   Eigen::Ref<Vector<>> flows) const
    {
        auto& params         = this->parameters;
        ScalarType coeffStoI_V1 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV1>() / populations.get_total();
        ScalarType coeffStoI_V2 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV2>() / populations.get_total();
        ScalarType coeffStoI_V3 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV3>() / populations.get_total();
        ScalarType coeffStoI_V4 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV4>() / populations.get_total();
        ScalarType coeffStoI_V5 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV5>() / populations.get_total();
        ScalarType coeffStoI_V6 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV6>() / populations.get_total();
        ScalarType coeffStoI_V7 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV7>() / populations.get_total();
        ScalarType coeffStoI_V8 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV8>() / populations.get_total();
        ScalarType coeffStoI_V9 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV9>() / populations.get_total();
        ScalarType coeffStoI_V10 = params.get<ContactPatterns>().get_matrix_at(t)(0, 0) *
                               params.get<TransmissionProbabilityOnContactV10>() / populations.get_total();

        ScalarType sum_infected_v1 = pop[(size_t)InfectionState::Infected_V1I0] + pop[(size_t)InfectionState::Infected_V1I1] 
            + pop[(size_t)InfectionState::Infected_V1I2] + pop[(size_t)InfectionState::Infected_V1I3] 
            + pop[(size_t)InfectionState::Infected_V1I4] + pop[(size_t)InfectionState::Infected_V1I5]
            + pop[(size_t)InfectionState::Infected_V1I6] + pop[(size_t)InfectionState::Infected_V1I7]
            + pop[(size_t)InfectionState::Infected_V1I8] + pop[(size_t)InfectionState::Infected_V1I9]
            + pop[(size_t)InfectionState::Infected_V1I10];
        ScalarType sum_infected_v2 = pop[(size_t)InfectionState::Infected_V2I0] + pop[(size_t)InfectionState::Infected_V2I1] 
            + pop[(size_t)InfectionState::Infected_V2I2] + pop[(size_t)InfectionState::Infected_V2I3] 
            + pop[(size_t)InfectionState::Infected_V2I4] + pop[(size_t)InfectionState::Infected_V2I5]
            + pop[(size_t)InfectionState::Infected_V2I6] + pop[(size_t)InfectionState::Infected_V2I7]
            + pop[(size_t)InfectionState::Infected_V2I8] + pop[(size_t)InfectionState::Infected_V2I9]
            + pop[(size_t)InfectionState::Infected_V2I10];
        ScalarType sum_infected_v3 = pop[(size_t)InfectionState::Infected_V3I0] + pop[(size_t)InfectionState::Infected_V3I1] 
            + pop[(size_t)InfectionState::Infected_V3I2] + pop[(size_t)InfectionState::Infected_V3I3] 
            + pop[(size_t)InfectionState::Infected_V3I4] + pop[(size_t)InfectionState::Infected_V3I5]
            + pop[(size_t)InfectionState::Infected_V3I6] + pop[(size_t)InfectionState::Infected_V3I7]
            + pop[(size_t)InfectionState::Infected_V3I8] + pop[(size_t)InfectionState::Infected_V3I9]
            + pop[(size_t)InfectionState::Infected_V3I10];
        ScalarType sum_infected_v4 = pop[(size_t)InfectionState::Infected_V4I0] + pop[(size_t)InfectionState::Infected_V4I1] 
            + pop[(size_t)InfectionState::Infected_V4I2] + pop[(size_t)InfectionState::Infected_V4I3] 
            + pop[(size_t)InfectionState::Infected_V4I4] + pop[(size_t)InfectionState::Infected_V4I5]
            + pop[(size_t)InfectionState::Infected_V4I6] + pop[(size_t)InfectionState::Infected_V4I7]
            + pop[(size_t)InfectionState::Infected_V4I8] + pop[(size_t)InfectionState::Infected_V4I9]
            + pop[(size_t)InfectionState::Infected_V4I10];
        ScalarType sum_infected_v5 = pop[(size_t)InfectionState::Infected_V5I0] + pop[(size_t)InfectionState::Infected_V5I1] 
            + pop[(size_t)InfectionState::Infected_V5I2] + pop[(size_t)InfectionState::Infected_V5I3] 
            + pop[(size_t)InfectionState::Infected_V5I4] + pop[(size_t)InfectionState::Infected_V5I5]
            + pop[(size_t)InfectionState::Infected_V5I6] + pop[(size_t)InfectionState::Infected_V5I7]
            + pop[(size_t)InfectionState::Infected_V5I8] + pop[(size_t)InfectionState::Infected_V5I9]
            + pop[(size_t)InfectionState::Infected_V5I10];
        ScalarType sum_infected_v6 = pop[(size_t)InfectionState::Infected_V6I0] + pop[(size_t)InfectionState::Infected_V6I1] 
            + pop[(size_t)InfectionState::Infected_V6I2] + pop[(size_t)InfectionState::Infected_V6I3] 
            + pop[(size_t)InfectionState::Infected_V6I4] + pop[(size_t)InfectionState::Infected_V6I5]
            + pop[(size_t)InfectionState::Infected_V6I6] + pop[(size_t)InfectionState::Infected_V6I7]
            + pop[(size_t)InfectionState::Infected_V6I8] + pop[(size_t)InfectionState::Infected_V6I9]
            + pop[(size_t)InfectionState::Infected_V6I10];
        ScalarType sum_infected_v7 = pop[(size_t)InfectionState::Infected_V7I0] + pop[(size_t)InfectionState::Infected_V7I1] 
            + pop[(size_t)InfectionState::Infected_V7I2] + pop[(size_t)InfectionState::Infected_V7I3] 
            + pop[(size_t)InfectionState::Infected_V7I4] + pop[(size_t)InfectionState::Infected_V7I5]
            + pop[(size_t)InfectionState::Infected_V7I6] + pop[(size_t)InfectionState::Infected_V7I7]
            + pop[(size_t)InfectionState::Infected_V7I8] + pop[(size_t)InfectionState::Infected_V7I9]
            + pop[(size_t)InfectionState::Infected_V7I10];
        ScalarType sum_infected_v8 = pop[(size_t)InfectionState::Infected_V8I0] + pop[(size_t)InfectionState::Infected_V8I1] 
            + pop[(size_t)InfectionState::Infected_V8I2] + pop[(size_t)InfectionState::Infected_V8I3] 
            + pop[(size_t)InfectionState::Infected_V8I4] + pop[(size_t)InfectionState::Infected_V8I5]
            + pop[(size_t)InfectionState::Infected_V8I6] + pop[(size_t)InfectionState::Infected_V8I7]
            + pop[(size_t)InfectionState::Infected_V8I8] + pop[(size_t)InfectionState::Infected_V8I9]
            + pop[(size_t)InfectionState::Infected_V8I10];
        ScalarType sum_infected_v9 = pop[(size_t)InfectionState::Infected_V9I0] + pop[(size_t)InfectionState::Infected_V9I1] 
            + pop[(size_t)InfectionState::Infected_V9I2] + pop[(size_t)InfectionState::Infected_V9I3] 
            + pop[(size_t)InfectionState::Infected_V9I4] + pop[(size_t)InfectionState::Infected_V9I5]
            + pop[(size_t)InfectionState::Infected_V9I6] + pop[(size_t)InfectionState::Infected_V9I7]
            + pop[(size_t)InfectionState::Infected_V9I8] + pop[(size_t)InfectionState::Infected_V9I9]
            + pop[(size_t)InfectionState::Infected_V9I10];
        ScalarType sum_infected_v10 = pop[(size_t)InfectionState::Infected_V10I0] + pop[(size_t)InfectionState::Infected_V10I1] 
            + pop[(size_t)InfectionState::Infected_V10I2] + pop[(size_t)InfectionState::Infected_V10I3] 
            + pop[(size_t)InfectionState::Infected_V10I4] + pop[(size_t)InfectionState::Infected_V10I5]
            + pop[(size_t)InfectionState::Infected_V10I6] + pop[(size_t)InfectionState::Infected_V10I7]
            + pop[(size_t)InfectionState::Infected_V10I8] + pop[(size_t)InfectionState::Infected_V10I9]
            + pop[(size_t)InfectionState::Infected_V10I10];      


        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V1I0>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V2I0>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V3I0>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V4I0>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V5I0>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V6I0>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V7I0>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V8I0>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V9I0>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V10I0>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V1I1>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V2I1>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V3I1>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V4I1>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V5I1>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V6I1>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V7I1>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V8I1>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V9I1>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V10I1>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v10;
        
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V1I2>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V2I2>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V3I2>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V4I2>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V5I2>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V6I2>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V7I2>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V8I2>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V9I2>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V10I2>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v10;
        
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V1I3>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V2I3>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V3I3>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V4I3>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V5I3>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V6I3>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V7I3>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V8I3>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V9I3>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V10I3>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V1I4>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V2I4>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V3I4>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V4I4>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V5I4>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V6I4>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V7I4>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V8I4>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V9I4>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I4, InfectionState::Infected_V10I4>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I4] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V1I5>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V2I5>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V3I5>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V4I5>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V5I5>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V6I5>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V7I5>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V8I5>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V9I5>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I5, InfectionState::Infected_V10I5>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I5] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V1I6>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V2I6>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V3I6>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V4I6>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V5I6>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V6I6>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V7I6>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V8I6>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V9I6>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I6, InfectionState::Infected_V10I6>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I6] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V1I7>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V2I7>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V3I7>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V4I7>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V5I7>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V6I7>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V7I7>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V8I7>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V9I7>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I7, InfectionState::Infected_V10I7>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I7] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V1I8>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V2I8>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V3I8>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V4I8>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V5I8>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V6I8>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V7I8>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V8I8>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V9I8>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I8, InfectionState::Infected_V10I8>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I8] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V1I9>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V2I9>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V3I9>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V4I9>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V5I9>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V6I9>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V7I9>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V8I9>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V9I9>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I9, InfectionState::Infected_V10I9>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I9] * sum_infected_v10;

        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V1I10>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V2I10>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V3I10>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V4I10>()] = coeffStoI_V4 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v4;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V5I10>()] = coeffStoI_V5 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v5;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V6I10>()] = coeffStoI_V6 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v6;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V7I10>()] = coeffStoI_V7 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v7;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V8I10>()] = coeffStoI_V8 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v8;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V9I10>()] = coeffStoI_V9 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v9;
        flows[get_flat_flow_index<InfectionState::Uninfected_I10, InfectionState::Infected_V10I10>()] = coeffStoI_V10 * y[(size_t)InfectionState::Uninfected_I10] * sum_infected_v10;
        
        flows[get_flat_flow_index<InfectionState::Infected_V1I0, InfectionState::Infected_V2I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I0];
        flows[get_flat_flow_index<InfectionState::Infected_V1I1, InfectionState::Infected_V2I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I1];
        flows[get_flat_flow_index<InfectionState::Infected_V1I2, InfectionState::Infected_V2I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I2];
        flows[get_flat_flow_index<InfectionState::Infected_V1I3, InfectionState::Infected_V2I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I3];
        flows[get_flat_flow_index<InfectionState::Infected_V1I4, InfectionState::Infected_V2I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I4];
        flows[get_flat_flow_index<InfectionState::Infected_V1I5, InfectionState::Infected_V2I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I5];
        flows[get_flat_flow_index<InfectionState::Infected_V1I6, InfectionState::Infected_V2I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I6];
        flows[get_flat_flow_index<InfectionState::Infected_V1I7, InfectionState::Infected_V2I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I7];
        flows[get_flat_flow_index<InfectionState::Infected_V1I8, InfectionState::Infected_V2I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I8];
        flows[get_flat_flow_index<InfectionState::Infected_V1I9, InfectionState::Infected_V2I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I9];
        flows[get_flat_flow_index<InfectionState::Infected_V1I10, InfectionState::Infected_V2I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Infected_V1I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Infected_V3I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Infected_V1I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Infected_V3I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Infected_V1I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Infected_V3I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Infected_V1I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Infected_V3I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I4, InfectionState::Infected_V1I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I4];
        flows[get_flat_flow_index<InfectionState::Infected_V2I4, InfectionState::Infected_V3I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I4];
        flows[get_flat_flow_index<InfectionState::Infected_V2I5, InfectionState::Infected_V1I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I5];
        flows[get_flat_flow_index<InfectionState::Infected_V2I5, InfectionState::Infected_V3I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I5];
        flows[get_flat_flow_index<InfectionState::Infected_V2I6, InfectionState::Infected_V1I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I6];
        flows[get_flat_flow_index<InfectionState::Infected_V2I6, InfectionState::Infected_V3I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I6];
        flows[get_flat_flow_index<InfectionState::Infected_V2I7, InfectionState::Infected_V1I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I7];
        flows[get_flat_flow_index<InfectionState::Infected_V2I7, InfectionState::Infected_V3I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I7];
        flows[get_flat_flow_index<InfectionState::Infected_V2I8, InfectionState::Infected_V1I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I8];
        flows[get_flat_flow_index<InfectionState::Infected_V2I8, InfectionState::Infected_V3I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I8];
        flows[get_flat_flow_index<InfectionState::Infected_V2I9, InfectionState::Infected_V1I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I9];
        flows[get_flat_flow_index<InfectionState::Infected_V2I9, InfectionState::Infected_V3I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I9];
        flows[get_flat_flow_index<InfectionState::Infected_V2I10, InfectionState::Infected_V1I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I10];
        flows[get_flat_flow_index<InfectionState::Infected_V2I10, InfectionState::Infected_V3I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V3I0, InfectionState::Infected_V2I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I0];
        flows[get_flat_flow_index<InfectionState::Infected_V3I0, InfectionState::Infected_V4I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I0];
        flows[get_flat_flow_index<InfectionState::Infected_V3I1, InfectionState::Infected_V2I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I1];
        flows[get_flat_flow_index<InfectionState::Infected_V3I1, InfectionState::Infected_V4I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I1];
        flows[get_flat_flow_index<InfectionState::Infected_V3I2, InfectionState::Infected_V2I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I2];
        flows[get_flat_flow_index<InfectionState::Infected_V3I2, InfectionState::Infected_V4I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I2];
        flows[get_flat_flow_index<InfectionState::Infected_V3I3, InfectionState::Infected_V2I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I3];
        flows[get_flat_flow_index<InfectionState::Infected_V3I3, InfectionState::Infected_V4I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I3];
        flows[get_flat_flow_index<InfectionState::Infected_V3I4, InfectionState::Infected_V2I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I4];
        flows[get_flat_flow_index<InfectionState::Infected_V3I4, InfectionState::Infected_V4I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I4];
        flows[get_flat_flow_index<InfectionState::Infected_V3I5, InfectionState::Infected_V2I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I5];
        flows[get_flat_flow_index<InfectionState::Infected_V3I5, InfectionState::Infected_V4I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I5];
        flows[get_flat_flow_index<InfectionState::Infected_V3I6, InfectionState::Infected_V2I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I6];
        flows[get_flat_flow_index<InfectionState::Infected_V3I6, InfectionState::Infected_V4I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I6];
        flows[get_flat_flow_index<InfectionState::Infected_V3I7, InfectionState::Infected_V2I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I7];
        flows[get_flat_flow_index<InfectionState::Infected_V3I7, InfectionState::Infected_V4I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I7];
        flows[get_flat_flow_index<InfectionState::Infected_V3I8, InfectionState::Infected_V2I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I8];
        flows[get_flat_flow_index<InfectionState::Infected_V3I8, InfectionState::Infected_V4I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I8];
        flows[get_flat_flow_index<InfectionState::Infected_V3I9, InfectionState::Infected_V2I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I9];
        flows[get_flat_flow_index<InfectionState::Infected_V3I9, InfectionState::Infected_V4I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I9];
        flows[get_flat_flow_index<InfectionState::Infected_V3I10, InfectionState::Infected_V2I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I10];
        flows[get_flat_flow_index<InfectionState::Infected_V3I10, InfectionState::Infected_V4I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V4I0, InfectionState::Infected_V3I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I0];
        flows[get_flat_flow_index<InfectionState::Infected_V4I0, InfectionState::Infected_V5I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I0];
        flows[get_flat_flow_index<InfectionState::Infected_V4I1, InfectionState::Infected_V3I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I1];
        flows[get_flat_flow_index<InfectionState::Infected_V4I1, InfectionState::Infected_V5I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I1];
        flows[get_flat_flow_index<InfectionState::Infected_V4I2, InfectionState::Infected_V3I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I2];
        flows[get_flat_flow_index<InfectionState::Infected_V4I2, InfectionState::Infected_V5I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I2];
        flows[get_flat_flow_index<InfectionState::Infected_V4I3, InfectionState::Infected_V3I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I3];
        flows[get_flat_flow_index<InfectionState::Infected_V4I3, InfectionState::Infected_V5I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I3];
        flows[get_flat_flow_index<InfectionState::Infected_V4I4, InfectionState::Infected_V3I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I4];
        flows[get_flat_flow_index<InfectionState::Infected_V4I4, InfectionState::Infected_V5I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I4];
        flows[get_flat_flow_index<InfectionState::Infected_V4I5, InfectionState::Infected_V3I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I5];
        flows[get_flat_flow_index<InfectionState::Infected_V4I5, InfectionState::Infected_V5I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I5];
        flows[get_flat_flow_index<InfectionState::Infected_V4I6, InfectionState::Infected_V3I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I6];
        flows[get_flat_flow_index<InfectionState::Infected_V4I6, InfectionState::Infected_V5I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I6];
        flows[get_flat_flow_index<InfectionState::Infected_V4I7, InfectionState::Infected_V3I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I7];
        flows[get_flat_flow_index<InfectionState::Infected_V4I7, InfectionState::Infected_V5I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I7];
        flows[get_flat_flow_index<InfectionState::Infected_V4I8, InfectionState::Infected_V3I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I8];
        flows[get_flat_flow_index<InfectionState::Infected_V4I8, InfectionState::Infected_V5I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I8];
        flows[get_flat_flow_index<InfectionState::Infected_V4I9, InfectionState::Infected_V3I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I9];
        flows[get_flat_flow_index<InfectionState::Infected_V4I9, InfectionState::Infected_V5I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I9];
        flows[get_flat_flow_index<InfectionState::Infected_V4I10, InfectionState::Infected_V3I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I10];
        flows[get_flat_flow_index<InfectionState::Infected_V4I10, InfectionState::Infected_V5I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V4I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V5I0, InfectionState::Infected_V4I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I0];
        flows[get_flat_flow_index<InfectionState::Infected_V5I0, InfectionState::Infected_V6I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I0];
        flows[get_flat_flow_index<InfectionState::Infected_V5I1, InfectionState::Infected_V4I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I1];
        flows[get_flat_flow_index<InfectionState::Infected_V5I1, InfectionState::Infected_V6I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I1];
        flows[get_flat_flow_index<InfectionState::Infected_V5I2, InfectionState::Infected_V4I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I2];
        flows[get_flat_flow_index<InfectionState::Infected_V5I2, InfectionState::Infected_V6I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I2];
        flows[get_flat_flow_index<InfectionState::Infected_V5I3, InfectionState::Infected_V4I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I3];
        flows[get_flat_flow_index<InfectionState::Infected_V5I3, InfectionState::Infected_V6I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I3];
        flows[get_flat_flow_index<InfectionState::Infected_V5I4, InfectionState::Infected_V4I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I4];
        flows[get_flat_flow_index<InfectionState::Infected_V5I4, InfectionState::Infected_V6I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I4];
        flows[get_flat_flow_index<InfectionState::Infected_V5I5, InfectionState::Infected_V4I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I5];
        flows[get_flat_flow_index<InfectionState::Infected_V5I5, InfectionState::Infected_V6I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I5];
        flows[get_flat_flow_index<InfectionState::Infected_V5I6, InfectionState::Infected_V4I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I6];
        flows[get_flat_flow_index<InfectionState::Infected_V5I6, InfectionState::Infected_V6I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I6];
        flows[get_flat_flow_index<InfectionState::Infected_V5I7, InfectionState::Infected_V4I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I7];
        flows[get_flat_flow_index<InfectionState::Infected_V5I7, InfectionState::Infected_V6I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I7];
        flows[get_flat_flow_index<InfectionState::Infected_V5I8, InfectionState::Infected_V4I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I8];
        flows[get_flat_flow_index<InfectionState::Infected_V5I8, InfectionState::Infected_V6I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I8];
        flows[get_flat_flow_index<InfectionState::Infected_V5I9, InfectionState::Infected_V4I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I9];
        flows[get_flat_flow_index<InfectionState::Infected_V5I9, InfectionState::Infected_V6I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I9];
        flows[get_flat_flow_index<InfectionState::Infected_V5I10, InfectionState::Infected_V4I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I10];
        flows[get_flat_flow_index<InfectionState::Infected_V5I10, InfectionState::Infected_V6I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V5I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V6I0, InfectionState::Infected_V5I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I0];
        flows[get_flat_flow_index<InfectionState::Infected_V6I0, InfectionState::Infected_V7I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I0];
        flows[get_flat_flow_index<InfectionState::Infected_V6I1, InfectionState::Infected_V5I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I1];
        flows[get_flat_flow_index<InfectionState::Infected_V6I1, InfectionState::Infected_V7I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I1];
        flows[get_flat_flow_index<InfectionState::Infected_V6I2, InfectionState::Infected_V5I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I2];
        flows[get_flat_flow_index<InfectionState::Infected_V6I2, InfectionState::Infected_V7I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I2];
        flows[get_flat_flow_index<InfectionState::Infected_V6I3, InfectionState::Infected_V5I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I3];
        flows[get_flat_flow_index<InfectionState::Infected_V6I3, InfectionState::Infected_V7I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I3];
        flows[get_flat_flow_index<InfectionState::Infected_V6I4, InfectionState::Infected_V5I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I4];
        flows[get_flat_flow_index<InfectionState::Infected_V6I4, InfectionState::Infected_V7I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I4];
        flows[get_flat_flow_index<InfectionState::Infected_V6I5, InfectionState::Infected_V5I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I5];
        flows[get_flat_flow_index<InfectionState::Infected_V6I5, InfectionState::Infected_V7I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I5];
        flows[get_flat_flow_index<InfectionState::Infected_V6I6, InfectionState::Infected_V5I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I6];
        flows[get_flat_flow_index<InfectionState::Infected_V6I6, InfectionState::Infected_V7I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I6];
        flows[get_flat_flow_index<InfectionState::Infected_V6I7, InfectionState::Infected_V5I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I7];
        flows[get_flat_flow_index<InfectionState::Infected_V6I7, InfectionState::Infected_V7I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I7];
        flows[get_flat_flow_index<InfectionState::Infected_V6I8, InfectionState::Infected_V5I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I8];
        flows[get_flat_flow_index<InfectionState::Infected_V6I8, InfectionState::Infected_V7I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I8];
        flows[get_flat_flow_index<InfectionState::Infected_V6I9, InfectionState::Infected_V5I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I9];
        flows[get_flat_flow_index<InfectionState::Infected_V6I9, InfectionState::Infected_V7I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I9];
        flows[get_flat_flow_index<InfectionState::Infected_V6I10, InfectionState::Infected_V5I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I10];
        flows[get_flat_flow_index<InfectionState::Infected_V6I10, InfectionState::Infected_V7I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V6I10];

        flows[get_flat_flow_index<InfectionState::Infected_V7I0, InfectionState::Infected_V6I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I0];
        flows[get_flat_flow_index<InfectionState::Infected_V7I0, InfectionState::Infected_V8I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I0];
        flows[get_flat_flow_index<InfectionState::Infected_V7I1, InfectionState::Infected_V6I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I1];
        flows[get_flat_flow_index<InfectionState::Infected_V7I1, InfectionState::Infected_V8I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I1];
        flows[get_flat_flow_index<InfectionState::Infected_V7I2, InfectionState::Infected_V6I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I2];
        flows[get_flat_flow_index<InfectionState::Infected_V7I2, InfectionState::Infected_V8I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I2];
        flows[get_flat_flow_index<InfectionState::Infected_V7I3, InfectionState::Infected_V6I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I3];
        flows[get_flat_flow_index<InfectionState::Infected_V7I3, InfectionState::Infected_V8I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I3];
        flows[get_flat_flow_index<InfectionState::Infected_V7I4, InfectionState::Infected_V6I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I4];
        flows[get_flat_flow_index<InfectionState::Infected_V7I4, InfectionState::Infected_V8I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I4];
        flows[get_flat_flow_index<InfectionState::Infected_V7I5, InfectionState::Infected_V6I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I5];
        flows[get_flat_flow_index<InfectionState::Infected_V7I5, InfectionState::Infected_V8I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I5];
        flows[get_flat_flow_index<InfectionState::Infected_V7I6, InfectionState::Infected_V6I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I6];
        flows[get_flat_flow_index<InfectionState::Infected_V7I6, InfectionState::Infected_V8I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I6];
        flows[get_flat_flow_index<InfectionState::Infected_V7I7, InfectionState::Infected_V6I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I7];
        flows[get_flat_flow_index<InfectionState::Infected_V7I7, InfectionState::Infected_V8I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I7];
        flows[get_flat_flow_index<InfectionState::Infected_V7I8, InfectionState::Infected_V6I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I8];
        flows[get_flat_flow_index<InfectionState::Infected_V7I8, InfectionState::Infected_V8I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I8];
        flows[get_flat_flow_index<InfectionState::Infected_V7I9, InfectionState::Infected_V6I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I9];
        flows[get_flat_flow_index<InfectionState::Infected_V7I9, InfectionState::Infected_V8I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I9];
        flows[get_flat_flow_index<InfectionState::Infected_V7I10, InfectionState::Infected_V6I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I10];
        flows[get_flat_flow_index<InfectionState::Infected_V7I10, InfectionState::Infected_V8I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V7I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V8I0, InfectionState::Infected_V7I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I0];
        flows[get_flat_flow_index<InfectionState::Infected_V8I0, InfectionState::Infected_V9I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I0];
        flows[get_flat_flow_index<InfectionState::Infected_V8I1, InfectionState::Infected_V7I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I1];
        flows[get_flat_flow_index<InfectionState::Infected_V8I1, InfectionState::Infected_V9I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I1];
        flows[get_flat_flow_index<InfectionState::Infected_V8I2, InfectionState::Infected_V7I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I2];
        flows[get_flat_flow_index<InfectionState::Infected_V8I2, InfectionState::Infected_V9I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I2];
        flows[get_flat_flow_index<InfectionState::Infected_V8I3, InfectionState::Infected_V7I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I3];
        flows[get_flat_flow_index<InfectionState::Infected_V8I3, InfectionState::Infected_V9I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I3];
        flows[get_flat_flow_index<InfectionState::Infected_V8I4, InfectionState::Infected_V7I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I4];
        flows[get_flat_flow_index<InfectionState::Infected_V8I4, InfectionState::Infected_V9I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I4];
        flows[get_flat_flow_index<InfectionState::Infected_V8I5, InfectionState::Infected_V7I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I5];
        flows[get_flat_flow_index<InfectionState::Infected_V8I5, InfectionState::Infected_V9I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I5];
        flows[get_flat_flow_index<InfectionState::Infected_V8I6, InfectionState::Infected_V7I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I6];
        flows[get_flat_flow_index<InfectionState::Infected_V8I6, InfectionState::Infected_V9I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I6];
        flows[get_flat_flow_index<InfectionState::Infected_V8I7, InfectionState::Infected_V7I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I7];
        flows[get_flat_flow_index<InfectionState::Infected_V8I7, InfectionState::Infected_V9I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I7];
        flows[get_flat_flow_index<InfectionState::Infected_V8I8, InfectionState::Infected_V7I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I8];
        flows[get_flat_flow_index<InfectionState::Infected_V8I8, InfectionState::Infected_V9I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I8];
        flows[get_flat_flow_index<InfectionState::Infected_V8I9, InfectionState::Infected_V7I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I9];
        flows[get_flat_flow_index<InfectionState::Infected_V8I9, InfectionState::Infected_V9I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I9];
        flows[get_flat_flow_index<InfectionState::Infected_V8I10, InfectionState::Infected_V7I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I10];
        flows[get_flat_flow_index<InfectionState::Infected_V8I10, InfectionState::Infected_V9I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V8I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V9I0, InfectionState::Infected_V8I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I0];
        flows[get_flat_flow_index<InfectionState::Infected_V9I0, InfectionState::Infected_V10I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I0];
        flows[get_flat_flow_index<InfectionState::Infected_V9I1, InfectionState::Infected_V8I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I1];
        flows[get_flat_flow_index<InfectionState::Infected_V9I1, InfectionState::Infected_V10I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I1];
        flows[get_flat_flow_index<InfectionState::Infected_V9I2, InfectionState::Infected_V8I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I2];
        flows[get_flat_flow_index<InfectionState::Infected_V9I2, InfectionState::Infected_V10I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I2];
        flows[get_flat_flow_index<InfectionState::Infected_V9I3, InfectionState::Infected_V8I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I3];
        flows[get_flat_flow_index<InfectionState::Infected_V9I3, InfectionState::Infected_V10I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I3];
        flows[get_flat_flow_index<InfectionState::Infected_V9I4, InfectionState::Infected_V8I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I4];
        flows[get_flat_flow_index<InfectionState::Infected_V9I4, InfectionState::Infected_V10I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I4];
        flows[get_flat_flow_index<InfectionState::Infected_V9I5, InfectionState::Infected_V8I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I5];
        flows[get_flat_flow_index<InfectionState::Infected_V9I5, InfectionState::Infected_V10I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I5];
        flows[get_flat_flow_index<InfectionState::Infected_V9I6, InfectionState::Infected_V8I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I6];
        flows[get_flat_flow_index<InfectionState::Infected_V9I6, InfectionState::Infected_V10I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I6];
        flows[get_flat_flow_index<InfectionState::Infected_V9I7, InfectionState::Infected_V8I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I7];
        flows[get_flat_flow_index<InfectionState::Infected_V9I7, InfectionState::Infected_V10I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I7];
        flows[get_flat_flow_index<InfectionState::Infected_V9I8, InfectionState::Infected_V8I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I8];
        flows[get_flat_flow_index<InfectionState::Infected_V9I8, InfectionState::Infected_V10I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I8];
        flows[get_flat_flow_index<InfectionState::Infected_V9I9, InfectionState::Infected_V8I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I9];
        flows[get_flat_flow_index<InfectionState::Infected_V9I9, InfectionState::Infected_V10I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I9];
        flows[get_flat_flow_index<InfectionState::Infected_V9I10, InfectionState::Infected_V8I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I10];
        flows[get_flat_flow_index<InfectionState::Infected_V9I10, InfectionState::Infected_V10I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V9I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V10I0, InfectionState::Infected_V9I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I0];
        flows[get_flat_flow_index<InfectionState::Infected_V10I1, InfectionState::Infected_V9I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I1];
        flows[get_flat_flow_index<InfectionState::Infected_V10I2, InfectionState::Infected_V9I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I2];
        flows[get_flat_flow_index<InfectionState::Infected_V10I3, InfectionState::Infected_V9I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I3];
        flows[get_flat_flow_index<InfectionState::Infected_V10I4, InfectionState::Infected_V9I4>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I4];
        flows[get_flat_flow_index<InfectionState::Infected_V10I5, InfectionState::Infected_V9I5>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I5];
        flows[get_flat_flow_index<InfectionState::Infected_V10I6, InfectionState::Infected_V9I6>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I6];
        flows[get_flat_flow_index<InfectionState::Infected_V10I7, InfectionState::Infected_V9I7>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I7];
        flows[get_flat_flow_index<InfectionState::Infected_V10I8, InfectionState::Infected_V9I8>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I8];
        flows[get_flat_flow_index<InfectionState::Infected_V10I9, InfectionState::Infected_V9I9>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I9];
        flows[get_flat_flow_index<InfectionState::Infected_V10I10, InfectionState::Infected_V9I10>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V10I10];

        flows[get_flat_flow_index<InfectionState::Infected_V1I0, InfectionState::Uninfected_I1>()] = (1.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I0];
        flows[get_flat_flow_index<InfectionState::Infected_V1I1, InfectionState::Uninfected_I1>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I1];
        flows[get_flat_flow_index<InfectionState::Infected_V1I2, InfectionState::Uninfected_I2>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I2];
        flows[get_flat_flow_index<InfectionState::Infected_V1I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I3];
        flows[get_flat_flow_index<InfectionState::Infected_V1I4, InfectionState::Uninfected_I4>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I4];
        flows[get_flat_flow_index<InfectionState::Infected_V1I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I5];
        flows[get_flat_flow_index<InfectionState::Infected_V1I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I6];
        flows[get_flat_flow_index<InfectionState::Infected_V1I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I6];
        flows[get_flat_flow_index<InfectionState::Infected_V1I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I8];
        flows[get_flat_flow_index<InfectionState::Infected_V1I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I9];
        flows[get_flat_flow_index<InfectionState::Infected_V1I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I10];

        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Uninfected_I2>()] = (1.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Uninfected_I2>()] = (1.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Uninfected_I2>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I4, InfectionState::Uninfected_I4>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I4];
        flows[get_flat_flow_index<InfectionState::Infected_V2I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I5];
        flows[get_flat_flow_index<InfectionState::Infected_V2I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I6];
        flows[get_flat_flow_index<InfectionState::Infected_V2I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I6];
        flows[get_flat_flow_index<InfectionState::Infected_V2I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I8];
        flows[get_flat_flow_index<InfectionState::Infected_V2I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I9];
        flows[get_flat_flow_index<InfectionState::Infected_V2I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V3I0, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V3I0];
        flows[get_flat_flow_index<InfectionState::Infected_V3I1, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V3I1];
        flows[get_flat_flow_index<InfectionState::Infected_V3I2, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I2];
        flows[get_flat_flow_index<InfectionState::Infected_V3I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I3];
        flows[get_flat_flow_index<InfectionState::Infected_V3I4, InfectionState::Uninfected_I4>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I4];
        flows[get_flat_flow_index<InfectionState::Infected_V3I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I5];
        flows[get_flat_flow_index<InfectionState::Infected_V3I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I6];
        flows[get_flat_flow_index<InfectionState::Infected_V3I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I6];
        flows[get_flat_flow_index<InfectionState::Infected_V3I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I8];
        flows[get_flat_flow_index<InfectionState::Infected_V3I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I9];
        flows[get_flat_flow_index<InfectionState::Infected_V3I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V4I0, InfectionState::Uninfected_I4>()] = (1.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I0];
        flows[get_flat_flow_index<InfectionState::Infected_V4I1, InfectionState::Uninfected_I4>()] = (1.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I1];
        flows[get_flat_flow_index<InfectionState::Infected_V4I2, InfectionState::Uninfected_I4>()] = (1.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I2];
        flows[get_flat_flow_index<InfectionState::Infected_V4I3, InfectionState::Uninfected_I4>()] = (1.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I3];
        flows[get_flat_flow_index<InfectionState::Infected_V4I4, InfectionState::Uninfected_I4>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I4];
        flows[get_flat_flow_index<InfectionState::Infected_V4I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I5];
        flows[get_flat_flow_index<InfectionState::Infected_V4I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I6];
        flows[get_flat_flow_index<InfectionState::Infected_V4I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I6];
        flows[get_flat_flow_index<InfectionState::Infected_V4I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I8];
        flows[get_flat_flow_index<InfectionState::Infected_V4I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I9];
        flows[get_flat_flow_index<InfectionState::Infected_V4I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV4>()) * y[(size_t)InfectionState::Infected_V4I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V5I0, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I0];
        flows[get_flat_flow_index<InfectionState::Infected_V5I1, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I1];
        flows[get_flat_flow_index<InfectionState::Infected_V5I2, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I2];
        flows[get_flat_flow_index<InfectionState::Infected_V5I3, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I3];
        flows[get_flat_flow_index<InfectionState::Infected_V5I4, InfectionState::Uninfected_I5>()] = (1.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I4];
        flows[get_flat_flow_index<InfectionState::Infected_V5I5, InfectionState::Uninfected_I5>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I5];
        flows[get_flat_flow_index<InfectionState::Infected_V5I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I6];
        flows[get_flat_flow_index<InfectionState::Infected_V5I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I6];
        flows[get_flat_flow_index<InfectionState::Infected_V5I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I8];
        flows[get_flat_flow_index<InfectionState::Infected_V5I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I9];
        flows[get_flat_flow_index<InfectionState::Infected_V5I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV5>()) * y[(size_t)InfectionState::Infected_V5I10];
    
        flows[get_flat_flow_index<InfectionState::Infected_V6I0, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I0];
        flows[get_flat_flow_index<InfectionState::Infected_V6I1, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I1];
        flows[get_flat_flow_index<InfectionState::Infected_V6I2, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I2];
        flows[get_flat_flow_index<InfectionState::Infected_V6I3, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I3];
        flows[get_flat_flow_index<InfectionState::Infected_V6I4, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I4];
        flows[get_flat_flow_index<InfectionState::Infected_V6I5, InfectionState::Uninfected_I6>()] = (1.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I5];
        flows[get_flat_flow_index<InfectionState::Infected_V6I6, InfectionState::Uninfected_I6>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I6];
        flows[get_flat_flow_index<InfectionState::Infected_V6I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I6];
        flows[get_flat_flow_index<InfectionState::Infected_V6I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I8];
        flows[get_flat_flow_index<InfectionState::Infected_V6I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I9];
        flows[get_flat_flow_index<InfectionState::Infected_V6I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV6>()) * y[(size_t)InfectionState::Infected_V6I10];
    
        flows[get_flat_flow_index<InfectionState::Infected_V7I0, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I0];
        flows[get_flat_flow_index<InfectionState::Infected_V7I1, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I1];
        flows[get_flat_flow_index<InfectionState::Infected_V7I2, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I2];
        flows[get_flat_flow_index<InfectionState::Infected_V7I3, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I3];
        flows[get_flat_flow_index<InfectionState::Infected_V7I4, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I4];
        flows[get_flat_flow_index<InfectionState::Infected_V7I5, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I5];
        flows[get_flat_flow_index<InfectionState::Infected_V7I6, InfectionState::Uninfected_I7>()] = (1.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I6];
        flows[get_flat_flow_index<InfectionState::Infected_V7I7, InfectionState::Uninfected_I7>()] = (100.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I6];
        flows[get_flat_flow_index<InfectionState::Infected_V7I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I8];
        flows[get_flat_flow_index<InfectionState::Infected_V7I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I9];
        flows[get_flat_flow_index<InfectionState::Infected_V7I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV7>()) * y[(size_t)InfectionState::Infected_V7I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V8I0, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I0];
        flows[get_flat_flow_index<InfectionState::Infected_V8I1, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I1];
        flows[get_flat_flow_index<InfectionState::Infected_V8I2, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I2];
        flows[get_flat_flow_index<InfectionState::Infected_V8I3, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I3];
        flows[get_flat_flow_index<InfectionState::Infected_V8I4, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I4];
        flows[get_flat_flow_index<InfectionState::Infected_V8I5, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I5];
        flows[get_flat_flow_index<InfectionState::Infected_V8I6, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I6];
        flows[get_flat_flow_index<InfectionState::Infected_V8I7, InfectionState::Uninfected_I8>()] = (1.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I6];
        flows[get_flat_flow_index<InfectionState::Infected_V8I8, InfectionState::Uninfected_I8>()] = (100.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I8];
        flows[get_flat_flow_index<InfectionState::Infected_V8I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I9];
        flows[get_flat_flow_index<InfectionState::Infected_V8I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV8>()) * y[(size_t)InfectionState::Infected_V8I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V9I0, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I0];
        flows[get_flat_flow_index<InfectionState::Infected_V9I1, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I1];
        flows[get_flat_flow_index<InfectionState::Infected_V9I2, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I2];
        flows[get_flat_flow_index<InfectionState::Infected_V9I3, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I3];
        flows[get_flat_flow_index<InfectionState::Infected_V9I4, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I4];
        flows[get_flat_flow_index<InfectionState::Infected_V9I5, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I5];
        flows[get_flat_flow_index<InfectionState::Infected_V9I6, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I6];
        flows[get_flat_flow_index<InfectionState::Infected_V9I7, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I6];
        flows[get_flat_flow_index<InfectionState::Infected_V9I8, InfectionState::Uninfected_I9>()] = (1.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I8];
        flows[get_flat_flow_index<InfectionState::Infected_V9I9, InfectionState::Uninfected_I9>()] = (100.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I9];
        flows[get_flat_flow_index<InfectionState::Infected_V9I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV9>()) * y[(size_t)InfectionState::Infected_V9I10];
        
        flows[get_flat_flow_index<InfectionState::Infected_V10I0, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I0];
        flows[get_flat_flow_index<InfectionState::Infected_V10I1, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I1];
        flows[get_flat_flow_index<InfectionState::Infected_V10I2, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I2];
        flows[get_flat_flow_index<InfectionState::Infected_V10I3, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I3];
        flows[get_flat_flow_index<InfectionState::Infected_V10I4, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I4];
        flows[get_flat_flow_index<InfectionState::Infected_V10I5, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I5];
        flows[get_flat_flow_index<InfectionState::Infected_V10I6, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I6];
        flows[get_flat_flow_index<InfectionState::Infected_V10I7, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I6];
        flows[get_flat_flow_index<InfectionState::Infected_V10I8, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I8];
        flows[get_flat_flow_index<InfectionState::Infected_V10I9, InfectionState::Uninfected_I10>()] = (1.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I9];
        flows[get_flat_flow_index<InfectionState::Infected_V10I10, InfectionState::Uninfected_I10>()] = (100.0 / params.get<TimeInfectedV10>()) * y[(size_t)InfectionState::Infected_V10I10];
       }

    ScalarType step_size; ///< A step size of the model with which the stochastic process is realized.
    mutable RandomNumberGenerator rng;

private:
};

} // namespace sui3
} // namespace mio

#endif // MIO_SDE_SIR_MODEL_H

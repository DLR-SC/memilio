/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*
* Authors: Daniel Abele, Martin J. Kuehn
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
#ifndef PARAMETER_SPACE_H
#define PARAMETER_SPACE_H

#include "memilio/mobility/mobility.h"
#include "memilio/utils/memory.h"
#include "memilio/utils/logging.h"
#include "memilio/utils/parameter_distributions.h"
#include "secir_vaccine/model.h"

#include <assert.h>
#include <string>
#include <vector>
#include <random>
#include <memory>

namespace mio
{
namespace secirv
{
    /**
     * draws a sample from the specified distributions for all parameters related to the demographics, e.g. population.
     * @param[inout] model SecirModel including contact patterns for alle age groups
     */
    void draw_sample_demographics(SecirModel& model);

    /**
     * draws a sample from the specified distributions for all parameters related to the infection.
     * @param[inout] model SecirModel including contact patterns for alle age groups
     */
    void draw_sample_infection(SecirModel& model);

    /** Draws a sample from SecirModel parameter distributions and stores sample values
    * as SecirParams parameter values (cf. UncertainValue and SecirParams classes)
    * @param[inout] model SecirModel including contact patterns for alle age groups
    */
    void draw_sample(SecirModel& model);

    Graph<SecirModel, MigrationParameters> draw_sample(Graph<SecirModel, MigrationParameters>& graph, double tmax, bool high);

} // namespace secirv
} // namespace mio

#endif // PARAMETER_SPACE_H

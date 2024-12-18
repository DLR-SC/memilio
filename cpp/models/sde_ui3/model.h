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

#ifndef MIO_SDE_UI3_MODEL_H
#define MIO_SDE_UI3_MODEL_H

#include "memilio/compartments/flow_model.h"
#include "memilio/epidemiology/populations.h"
#include "memilio/utils/random_number_generator.h"
#include "sde_ui3/infection_state.h"
#include "sde_ui3/parameters.h"

namespace mio
{
namespace sui3
{

/********************
 * define the model *
 ********************/

using Flows = TypeList<Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V1I0>, //Infection
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V2I0>,
                       Flow<InfectionState::Uninfected_I0, InfectionState::Infected_V3I0>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V1I1>, 
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V2I1>,
                       Flow<InfectionState::Uninfected_I1, InfectionState::Infected_V3I1>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V1I2>, 
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V2I2>,
                       Flow<InfectionState::Uninfected_I2, InfectionState::Infected_V3I2>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V1I3>, 
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V2I3>,
                       Flow<InfectionState::Uninfected_I3, InfectionState::Infected_V3I3>,

                       Flow<InfectionState::Infected_V1I0, InfectionState::Infected_V2I0>, //mutation
                       Flow<InfectionState::Infected_V1I1, InfectionState::Infected_V2I1>,
                       Flow<InfectionState::Infected_V1I2, InfectionState::Infected_V2I2>,
                       Flow<InfectionState::Infected_V1I3, InfectionState::Infected_V2I3>,
                       Flow<InfectionState::Infected_V2I0, InfectionState::Infected_V1I0>,
                       Flow<InfectionState::Infected_V2I0, InfectionState::Infected_V3I0>,
                       Flow<InfectionState::Infected_V2I1, InfectionState::Infected_V1I1>,
                       Flow<InfectionState::Infected_V2I1, InfectionState::Infected_V3I1>,
                       Flow<InfectionState::Infected_V2I2, InfectionState::Infected_V1I2>,
                       Flow<InfectionState::Infected_V2I2, InfectionState::Infected_V3I2>,
                       Flow<InfectionState::Infected_V2I3, InfectionState::Infected_V1I3>,
                       Flow<InfectionState::Infected_V2I3, InfectionState::Infected_V3I3>,
                       Flow<InfectionState::Infected_V3I0, InfectionState::Infected_V2I0>,
                       Flow<InfectionState::Infected_V3I1, InfectionState::Infected_V2I1>,
                       Flow<InfectionState::Infected_V3I2, InfectionState::Infected_V2I2>,
                       Flow<InfectionState::Infected_V3I3, InfectionState::Infected_V2I3>,

                       Flow<InfectionState::Infected_V1I0, InfectionState::Uninfected_I1>, //recovery
                       Flow<InfectionState::Infected_V1I1, InfectionState::Uninfected_I1>,
                       Flow<InfectionState::Infected_V1I2, InfectionState::Uninfected_I2>,
                       Flow<InfectionState::Infected_V1I3, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V2I0, InfectionState::Uninfected_I2>,
                       Flow<InfectionState::Infected_V2I1, InfectionState::Uninfected_I2>,
                       Flow<InfectionState::Infected_V2I2, InfectionState::Uninfected_I2>,
                       Flow<InfectionState::Infected_V2I3, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V3I0, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V3I1, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V3I2, InfectionState::Uninfected_I3>,
                       Flow<InfectionState::Infected_V3I3, InfectionState::Uninfected_I3>>;

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


        ScalarType sum_infected_v1 = pop[(size_t)InfectionState::Infected_V1I0] + pop[(size_t)InfectionState::Infected_V1I1] 
            + pop[(size_t)InfectionState::Infected_V1I2] + pop[(size_t)InfectionState::Infected_V1I3];
        ScalarType sum_infected_v2 = pop[(size_t)InfectionState::Infected_V2I0] + pop[(size_t)InfectionState::Infected_V2I1] 
            + pop[(size_t)InfectionState::Infected_V2I2] + pop[(size_t)InfectionState::Infected_V2I3];
        ScalarType sum_infected_v3 = pop[(size_t)InfectionState::Infected_V3I0] + pop[(size_t)InfectionState::Infected_V3I1] 
            + pop[(size_t)InfectionState::Infected_V3I2] + pop[(size_t)InfectionState::Infected_V3I3];

        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V1I0>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V2I0>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I0, InfectionState::Infected_V3I0>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I0] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V1I1>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V2I1>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I1, InfectionState::Infected_V3I1>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I1] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V1I2>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V2I2>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I2, InfectionState::Infected_V3I2>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I2] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V1I3>()] = coeffStoI_V1 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v1;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V2I3>()] = coeffStoI_V2 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v2;
        flows[get_flat_flow_index<InfectionState::Uninfected_I3, InfectionState::Infected_V3I3>()] = coeffStoI_V3 * y[(size_t)InfectionState::Uninfected_I3] * sum_infected_v3;
        flows[get_flat_flow_index<InfectionState::Infected_V1I0, InfectionState::Infected_V2I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I0];
        flows[get_flat_flow_index<InfectionState::Infected_V1I1, InfectionState::Infected_V2I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I1];
        flows[get_flat_flow_index<InfectionState::Infected_V1I2, InfectionState::Infected_V2I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I2];
        flows[get_flat_flow_index<InfectionState::Infected_V1I3, InfectionState::Infected_V2I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V1I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Infected_V1I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Infected_V3I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Infected_V1I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Infected_V3I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Infected_V1I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Infected_V3I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Infected_V1I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Infected_V3I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V3I0, InfectionState::Infected_V2I0>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I0];
        flows[get_flat_flow_index<InfectionState::Infected_V3I1, InfectionState::Infected_V2I1>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I1];
        flows[get_flat_flow_index<InfectionState::Infected_V3I2, InfectionState::Infected_V2I2>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I2];
        flows[get_flat_flow_index<InfectionState::Infected_V3I3, InfectionState::Infected_V2I3>()] = params.get<ProbMutation>() * y[(size_t)InfectionState::Infected_V3I3];
        flows[get_flat_flow_index<InfectionState::Infected_V1I0, InfectionState::Uninfected_I1>()] = (1.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I0];
        flows[get_flat_flow_index<InfectionState::Infected_V1I1, InfectionState::Uninfected_I1>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I1];
        flows[get_flat_flow_index<InfectionState::Infected_V1I2, InfectionState::Uninfected_I2>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I2];
        flows[get_flat_flow_index<InfectionState::Infected_V1I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV1>()) * y[(size_t)InfectionState::Infected_V1I3];
        flows[get_flat_flow_index<InfectionState::Infected_V2I0, InfectionState::Uninfected_I2>()] = (1.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I0];
        flows[get_flat_flow_index<InfectionState::Infected_V2I1, InfectionState::Uninfected_I2>()] = (1.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I1];
        flows[get_flat_flow_index<InfectionState::Infected_V2I2, InfectionState::Uninfected_I2>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I2];
        flows[get_flat_flow_index<InfectionState::Infected_V2I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV2>()) * y[(size_t)InfectionState::Infected_V2I3];
        flows[get_flat_flow_index<InfectionState::Infected_V3I0, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I0];
        flows[get_flat_flow_index<InfectionState::Infected_V3I1, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I1];
        flows[get_flat_flow_index<InfectionState::Infected_V3I2, InfectionState::Uninfected_I3>()] = (1.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I2];
        flows[get_flat_flow_index<InfectionState::Infected_V3I3, InfectionState::Uninfected_I3>()] = (100.0 / params.get<TimeInfectedV3>()) * y[(size_t)InfectionState::Infected_V3I3];
    }

    ScalarType step_size; ///< A step size of the model with which the stochastic process is realized.
    mutable RandomNumberGenerator rng;

private:
};

} // namespace sui3
} // namespace mio

#endif // MIO_SDE_SIR_MODEL_H

/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Lena Ploetzke
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

#ifndef LCT_SECIR_MODEL_H
#define LCT_SECIR_MODEL_H

#include "lct_secir/parameters.h"
#include "lct_secir/infection_state.h"
#include "memilio/config.h"
#include "memilio/utils/time_series.h"
#include "memilio/math/eigen.h"
#include <string>

namespace mio
{
namespace lsecir
{
class Model
{

public:
    /**
     * @brief Constructor to create an LCT SECIR Model.
     *
     * @param[in] init Vector with initial values for all infection states inclusive subcompartments.
     * @param[in, out] infectionState_init infectionState for the Model, specifies number of subcompartments for each infection state.
     * @param[in, out] parameters_init Specifies Parameters necessary for the Model. 
     */
    Model(Eigen::VectorXd init, InfectionState infectionState_init = InfectionState(),
          Parameters&& parameters_init = Parameters());

    /**
     * @brief Checks constraints of the model inclusive check for model parameters.
     */
    bool check_constraints() const;

    /**
     * @brief Evaulates the right-hand-side f of the LCT dydt = f(y, t).
     *
     * The LCT-SECIR model is defined through ordinary differential equations of the form dydt = f(y, t). 
     * y is a vector containing number of individuals for each (sub-) compartment.
     * This function evaluates the right-hand-side f of the ODE and can be used in an ODE solver.
     * @param[in] y the current state of the model
     * @param[in] t the current time
     * @param[out] dydt a reference to the calculated output
     */
    void eval_right_hand_side(Eigen::Ref<const Eigen::VectorXd> y, ScalarType t,
                              Eigen::Ref<Eigen::VectorXd> dydt) const;

    /**
     * @brief Cumulates a simulation result with subcompartments to produce a result that divides the population only into the infection states defined in InfectionStateBase.
     *
     * If the model is used for simulation, we will get a result in form of a TimeSeries with infection states divided in subcompartments.
     * Function transforms this TimeSeries in another TimeSeries with just the Basic infectionState. 
     * This is done by summing up the numbers in the subcompartments.
     * @param[in] result result of a simulation with the model.
     * @return result of the simulation divided in the Base infection states. 
     *  Returns TimeSeries with values -1 if calculation is not possible.
     */
    TimeSeries<ScalarType> calculate_populations(const TimeSeries<ScalarType>& result) const;

    /**
     * @brief Returns the initial values for the model.
     *
     * This can be used as initial conditions in an ODE solver.
     * @return Vector with initial values for all (sub-)compartments.
     */
    Eigen::VectorXd get_initial_values()
    {
        return m_initial_values;
    }

    Parameters parameters{}; ///< Parameters of the model.
    InfectionState infectionState; ///< InfectionState specifies number of subcompartments.

private:
    Eigen::VectorXd m_initial_values; ///< Initial values of the model.
    ScalarType m_N0{
        0}; ///< Total population size at time t_0 for the considered region (inclusive initial value for Dead).
};

} // namespace lsecir
} // namespace mio

#endif // LCTSECIR_MODEL_H
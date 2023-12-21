/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Daniel Abele
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
#ifndef METAPOPULATION_MOBILITY_STOCHASTIC_H
#define METAPOPULATION_MOBILITY_STOCHASTIC_H

#include "memilio/compartments/simulation.h"
#include "memilio/utils/time_series.h"
#include "memilio/epidemiology/contact_matrix.h"
#include "memilio/epidemiology/age_group.h"
#include "memilio/mobility/graph_simulation.h"
#include "memilio/mobility/metapopulation_mobility_instant.h"

#include "boost/filesystem.hpp"

#include <cassert>

namespace mio
{

/**
 * status and age dependent movement coefficients.
 */
using MovementCoefficients = DampingMatrixExpression<VectorDampings>;

/**
 * parameters that influence movement.
 */
class MovementParametersStochastic
{
public:
    /**
     * constructor from movement coefficients.
     * @param coeffs movement coefficients
     */
    MovementParametersStochastic(const MovementCoefficients& coeffs)
        : m_coefficients(coeffs)
    {
    }

    /**
     * constructor from movement coefficients.
     * @param coeffs movement coefficients
     */
    MovementParametersStochastic(const Eigen::VectorXd& coeffs)
        : m_coefficients(MovementCoefficients(coeffs))
    {
    }

    /** 
     * equality comparison operators
     */
    //@{
    bool operator==(const MovementParametersStochastic& other) const
    {
        return m_coefficients == other.m_coefficients;
    }
    bool operator!=(const MovementParametersStochastic& other) const
    {
        return m_coefficients != other.m_coefficients;
    }
    //@}

    /**
     * Get/Set the movement coefficients.
     * The coefficients represent the rates for migrating 
     * from one node to another by age and infection compartment. 
     * @{
     */
    /**
     * @return the movement coefficients.
     */
    const MovementCoefficients& get_coefficients() const
    {
        return m_coefficients;
    }
    MovementCoefficients& get_coefficients()
    {
        return m_coefficients;
    }
    /**
     * @param coeffs the movement coefficients.
     */
    void set_coefficients(const MovementCoefficients& coeffs)
    {
        m_coefficients = coeffs;
    }

    /**
     * serialize this. 
     * @see mio::serialize
     */
    template <class IOContext>
    void serialize(IOContext& io) const
    {
        auto obj = io.create_object("MovementParameters");
        obj.add_element("Coefficients", m_coefficients);
    }

    /**
     * deserialize an object of this class.
     * @see mio::deserialize
     */
    template <class IOContext>
    static IOResult<MovementParametersStochastic> deserialize(IOContext& io)
    {
        auto obj = io.expect_object("MovementParameters");
        auto c   = obj.expect_element("Coefficients", Tag<MovementCoefficients>{});
        return apply(
            io,
            [](auto&& c_) {
                MovementParametersStochastic params(c_);
                return params;
            },
            c);
    }

private:
    MovementCoefficients m_coefficients; //one per group and compartment
};

/** 
 * represents the movement between two nodes.
 */
class MovementEdgeStochastic
{
public:
    /**
     * create edge with coefficients.
     * @param coeffs movement rate for each group and compartment
     */
    MovementEdgeStochastic(const MovementParametersStochastic& params)
        : m_parameters(params)
    {
    }

    /**
     * create edge with coefficients.
     * @param coeffs movement rate for each group and compartment
     */
    MovementEdgeStochastic(const Eigen::VectorXd& coeffs)
        : m_parameters(coeffs)
    {
    }

    /**
     * get the movement parameters.
     */
    const MovementParametersStochastic& get_parameters() const
    {
        return m_parameters;
    }

    /**
     * get the cumulative transition rate of the edge.
    */
    template <class Sim>
    Eigen::VectorXd get_transition_rates(SimulationNode<Sim>& node_from)
    {
        Eigen::VectorXd transitionRates(node_from.get_last_state().size());
        for (Eigen::Index i = 0; i < node_from.get_last_state().size(); ++i) {
            transitionRates[i] =
                node_from.get_last_state()(i) * m_parameters.get_coefficients().get_baseline()[(size_t)i];
        }
        return transitionRates;
    }

    /**
     * compute movement from node_from to node_to for a given event
     * @param[in] event index specifying which compartment and age group migrates
     * @param node_from node that people migrated from
     * @param node_to node that people migrated to
     */
    template <class Sim>
    void apply_movement(size_t event, SimulationNode<Sim>& node_from, SimulationNode<Sim>& node_to);

private:
    MovementParametersStochastic m_parameters;
};

template <class Sim>
void MovementEdgeStochastic::apply_movement(size_t event, SimulationNode<Sim>& node_from,
                                              SimulationNode<Sim>& node_to)
{
    node_from.get_result().get_last_value()[event] -= 1;
    node_to.get_result().get_last_value()[event] += 1;
}

/**
 * edge functor for movement simulation.
 * @see MovementEdgeStochastic::apply_movement
 */
template <class Sim, class StochasticEdge>
void apply_movement(StochasticEdge& movementEdge, size_t event, SimulationNode<Sim>& node_from,
                     SimulationNode<Sim>& node_to)
{
    movementEdge.apply_movement(event, node_from, node_to);
}

/**
 * create a movement simulation.
 * After every second time step, for each edge a portion of the population corresponding to the coefficients of the edge
 * moves from one node to the other. In the next timestep, the migrated population return to their "home" node. 
 * Returns are adjusted based on the development in the target node. 
 * @param t0 start time of the simulation
 * @param dt time step between movements
 * @param graph set up for movement simulation
 * @{
 */
template <class Sim>
GraphSimulationStochastic<Graph<SimulationNode<Sim>, MovementEdgeStochastic>>
make_movement_sim(double t0, double dt, const Graph<SimulationNode<Sim>, MovementEdgeStochastic>& graph)
{
    return make_graph_sim_stochastic(t0, dt, graph, &evolve_model<Sim>, &apply_movement<Sim, MovementEdgeStochastic>);
}

template <class Sim>
GraphSimulationStochastic<Graph<SimulationNode<Sim>, MovementEdgeStochastic>>
make_movement_sim(double t0, double dt, Graph<SimulationNode<Sim>, MovementEdgeStochastic>&& graph)
{
    return make_graph_sim_stochastic(t0, dt, std::move(graph), &evolve_model<Sim>,
                                     &apply_movement<Sim, MovementEdgeStochastic>);
}

/** @} */

} // namespace mio

#endif //METAPOPULATION_MOBILITY_STOCHASTIC_H

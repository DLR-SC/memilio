/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
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
#ifndef MOBILITY_H
#define MOBILITY_H

#include "memilio/compartments/simulation.h"
#include "memilio/utils/time_series.h"
#include "memilio/epidemiology/contact_matrix.h"
#include "memilio/epidemiology/age_group.h"
#include "memilio/mobility/graph_simulation.h"

//rausnehmen
#include "memilio/utils/compiler_diagnostics.h"

#include "boost/filesystem.hpp"

#include <cassert>

namespace mio
{

/**
 * represents the simulation in one node of the graph.
 */
template <class Sim>
class SimulationNode
{
public:
    template <class... Args, typename = std::enable_if_t<std::is_constructible<Sim, Args...>::value, void>>
    SimulationNode(Args&&... args)
        : m_simulation(std::forward<Args>(args)...)
        , m_last_state(m_simulation.get_result().get_last_value())
        , m_t0(m_simulation.get_result().get_last_time())
    {
    }

    /**
     * get the result of the simulation in this node.
     * @{
     */
    decltype(auto) get_result() const
    {
        return m_simulation.get_result();
    }
    decltype(auto) get_result()
    {
        return m_simulation.get_result();
    }
    /**@}*/

    /**
     * get the the simulation in this node.
     * @{
     */
    Sim& get_simulation()
    {
        return m_simulation;
    }
    const Sim& get_simulation() const
    {
        return m_simulation;
    }
    /**@}*/

    Eigen::Ref<const Eigen::VectorXd> get_last_state() const
    {
        return m_last_state;
    }

    double get_t0() const
    {
        return m_t0;
    }

    void evolve(double t, double dt)
    {
        m_simulation.advance(t + dt);
        m_last_state = m_simulation.get_result().get_last_value();
    }

private:
    Sim m_simulation;
    Eigen::VectorXd m_last_state;
    double m_t0;
};

/**
 * status and age dependent migration coefficients.
 */
using MigrationCoefficients = DampingMatrixExpression<VectorDampings>;

/**
 * parameters that influence migration.
 */
class MigrationParametersStochastic
{
public:
    /**
     * constructor from migration coefficients.
     * @param coeffs migration coefficients
     */
    MigrationParametersStochastic(const MigrationCoefficients& coeffs)
        : m_coefficients(coeffs)
    {
    }

    /**
     * constructor from migration coefficients.
     * @param coeffs migration coefficients
     */
    MigrationParametersStochastic(const Eigen::VectorXd& coeffs)
        : m_coefficients(MigrationCoefficients(coeffs))
    {
    }

    /** 
     * equality comparison operators
     */
    //@{
    bool operator==(const MigrationParametersStochastic& other) const
    {
        return m_coefficients == other.m_coefficients;
    }
    bool operator!=(const MigrationParametersStochastic& other) const
    {
        return m_coefficients != other.m_coefficients;
    }
    //@}

    /**
     * Get/Set the migration coefficients.
     * The coefficients represent the rates for migrating 
     * from one node to another by age and infection compartment. 
     * @{
     */
    /**
     * @return the migration coefficients.
     */
    const MigrationCoefficients& get_coefficients() const
    {
        return m_coefficients;
    }
    MigrationCoefficients& get_coefficients()
    {
        return m_coefficients;
    }
    /**
     * @param coeffs the migration coefficients.
     */
    void set_coefficients(const MigrationCoefficients& coeffs)
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
        auto obj = io.create_object("MigrationParameters");
        obj.add_element("Coefficients", m_coefficients);
    }

    /**
     * deserialize an object of this class.
     * @see mio::deserialize
     */
    template <class IOContext>
    static IOResult<MigrationParametersStochastic> deserialize(IOContext& io)
    {
        auto obj = io.expect_object("MigrationParameters");
        auto c   = obj.expect_element("Coefficients", Tag<MigrationCoefficients>{});
        return apply(
            io,
            [](auto&& c_) {
                MigrationParametersStochastic params(c_);
                return params;
            },
            c);
    }

private:
    MigrationCoefficients m_coefficients; //one per group and compartment
};

/** 
 * represents the migration between two nodes.
 */
class MigrationEdgeStochastic
{
public:
    /**
     * create edge with coefficients.
     * @param coeffs migration rate for each group and compartment
     */
    MigrationEdgeStochastic(const MigrationParametersStochastic& params)
        : m_parameters(params)
    {
    }

    /**
     * create edge with coefficients.
     * @param coeffs migration rate for each group and compartment
     */
    MigrationEdgeStochastic(const Eigen::VectorXd& coeffs)
        : m_parameters(coeffs)
    {
    }

    /**
     * get the migration parameters.
     */
    const MigrationParametersStochastic& get_parameters() const
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
        //std::cout << "node from last state" << node_from.get_last_state() << std::endl;
        //std::cout << "transition coefficients" << m_parameters.get_coefficients().get_baseline() << std::endl;
        for (Eigen::Index i = 0; i < node_from.get_last_state().size(); ++i) {
            transitionRates[i] =
                node_from.get_last_state()(i) * m_parameters.get_coefficients().get_baseline()[(size_t)i];
        }
        //std::cout << "transition rates \n" << transitionRates << std::endl;
        //(node_from.get_last_state().array() * m_parameters.get_coefficients().get_baseline().array()).matrix();
        return transitionRates;
    }

    /**
     * compute migration from node_from to node_to for a given event
     * @param[in] event index specifying which compartment and age group migrates
     * @param node_from node that people migrated from
     * @param node_to node that people migrated to
     */
    template <class Sim>
    void apply_migration(size_t event, SimulationNode<Sim>& node_from, SimulationNode<Sim>& node_to);

private:
    MigrationParametersStochastic m_parameters;
};

template <class Sim>
void MigrationEdgeStochastic::apply_migration(size_t event, SimulationNode<Sim>& node_from,
                                              SimulationNode<Sim>& node_to)
{
    //std::cout << "compartments before transition: " << node_from.get_result().get_last_value()[event] << ", "
    //<< node_to.get_result().get_last_value()[event] << std::endl;
    node_from.get_result().get_last_value()[event] -= 1;
    node_to.get_result().get_last_value()[event] += 1;
    //std::cout << "compartments after transition: " << node_from.get_result().get_last_value()[event] << ", "
    //<< node_to.get_result().get_last_value()[event] << std::endl;
}

/**
 * node functor to evolve model.
 * @see SimulationNode::evolve
 */
template <class Sim>
void evolve_model(double t, double dt, SimulationNode<Sim>& node)
{
    node.evolve(t, dt);
}

/**
 * edge functor for migration simulation.
 * @see MigrationEdge::apply_migration
 */
template <class Sim>
void apply_migration(MigrationEdgeStochastic& migrationEdge, size_t event, SimulationNode<Sim>& node_from,
                     SimulationNode<Sim>& node_to)
{
    migrationEdge.apply_migration(event, node_from, node_to);
}

/**
 * create a migration simulation.
 * After every second time step, for each edge a portion of the population corresponding to the coefficients of the edge
 * moves from one node to the other. In the next timestep, the migrated population return to their "home" node. 
 * Returns are adjusted based on the development in the target node. 
 * @param t0 start time of the simulation
 * @param dt time step between migrations
 * @param graph set up for migration simulation
 * @{
 */
template <class Sim>
GraphSimulationStochastic<Graph<SimulationNode<Sim>, MigrationEdgeStochastic>>
make_migration_sim(double t0, double dt, const Graph<SimulationNode<Sim>, MigrationEdgeStochastic>& graph)
{
    return make_graph_sim_stochastic(t0, dt, graph, &evolve_model<Sim>, &apply_migration<Sim>);
}

template <class Sim>
GraphSimulationStochastic<Graph<SimulationNode<Sim>, MigrationEdgeStochastic>>
make_migration_sim(double t0, double dt, Graph<SimulationNode<Sim>, MigrationEdgeStochastic>&& graph)
{
    return make_graph_sim_stochastic(t0, dt, std::move(graph), &evolve_model<Sim>, &apply_migration<Sim>);
}

/** @} */

} // namespace mio

#endif //MOBILITY_H

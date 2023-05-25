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
#ifndef MIO_MOBILITY_GRAPH_SIMULATION_H
#define MIO_MOBILITY_GRAPH_SIMULATION_H

#include "memilio/mobility/graph.h"
#include "memilio/utils/compiler_diagnostics.h"
#include "memilio/utils/random_number_generator.h"

namespace mio
{

/**
 * @brief abstract simulation on a graph with alternating node and edge actions
 */
template <class Graph, class edge_f = std::function<void(double, double, typename Graph::EdgeProperty&,
                                                         typename Graph::NodeProperty&, typename Graph::NodeProperty&,
                                                         typename Graph::NodeProperty&, typename Graph::NodeProperty&)>>
class GraphSimulationBase
{
public:
    using node_function = std::function<void(double, double, typename Graph::NodeProperty&)>;

    using edge_function = edge_f;

    GraphSimulationBase(double t0, double dt, const Graph& g, const node_function& node_func,
                        const edge_function&& edge_func)
        : m_t(t0)
        , m_dt(dt)
        , m_graph(g)
        , m_node_func(node_func)
        , m_edge_func(edge_func)
    {
    }

    GraphSimulationBase(double t0, double dt, Graph&& g, const node_function& node_func,
                        const edge_function&& edge_func)
        : m_t(t0)
        , m_dt(dt)
        , m_graph(std::move(g))
        , m_node_func(node_func)
        , m_edge_func(edge_func)
    {
    }

    void advance(double t_max = 1.0)
    {
        mio::unused(t_max);
        // // pseudocode
        // // 1. kleinstes dt berechnen
        // // 2. eigen matrix mit zeitpunkten?
        // // 3. Woher weiß ich wie vielte Reise? Evtl aus t ziehen
        // auto dt = m_dt;

        // // Idee: 2 Modelle erstellen. 1x PT 1x MIT. Dann Landkreisweise durch iterieren und immer vorher alle Personen die drin sind berechnen.
        // // Das wäre dann node_curr.
        // // Problem ist, dass ich den aktuellen Stand der m_migrated nirgendswo abspeichern kann. Müsste entweder deren Position berechnen oder iwo sammeln
        // // Idee 2: Modelle dann in Matrix packen und vervielfältigen. Bei ersten Aufruf initialisieren. Mehr Speicher, aber deutlich schnellerer Aufruf,
        // // da ich direkt weiß, wer im LK ist.

        // // create node for PT and MIT
        // // auto& node    = m_graph.nodes()[0];
        // // auto node_pt  = node.property.get_simulation().get_model();
        // // auto node_mit = node.property.get_simulation().get_model();

        // // node_pt.populations.set_total(0);
        // // node_mit.populations.set_total(0);

        // auto nodes_mit = m_graph.nodes();

        // for (auto& n : nodes_mit) {
        //     n.property.get_simulation().get_model().populations.set_total(0);
        // }
        // auto nodes_pt = nodes_mit;

        // // start mobility activity with edge-wise different travel times and a own duration time for each county.
        // while (m_t < t_max) {
        //     double t_begin = m_t;
        //     if (m_t + dt > t_max) {
        //         dt = t_max - m_t;
        //         for (auto& n : m_graph.nodes()) {
        //             m_node_func(m_t, dt, n.property);
        //         }
        //         break;
        //     }

        //     // length of stay until first people start their mobility
        //     for (auto& n : m_graph.nodes()) {
        //         m_node_func(m_t, dt, n.property);
        //     }
        //     m_t += dt;

        //     // start mobility activity with edge-wise different travel times and a own duration time for each county.
        //     double dt_traveltimes = 0.01;
        //     while (std::abs(t_begin + 1 - m_t) > 1e-10) {

        //         for (auto& e : m_graph.edges()) {
        //             // start trip
        //             if (std::abs(t_begin + 1 - 2 * e.traveltime - m_graph.nodes()[e.end_node_idx].stay_duration - m_t) <
        //                 1e-10) {
        //                 m_edge_func(m_t, e.traveltime, e.property, m_graph.nodes()[e.start_node_idx].property,
        //                             m_graph.nodes()[e.end_node_idx].property);
        //             }

        //             // arrival travelers
        //             if (std::abs(t_begin + 1 - e.traveltime - m_graph.nodes()[e.end_node_idx].stay_duration - m_t) <
        //                 1e-10) {
        //                 m_edge_func(m_t, e.traveltime, e.property, m_graph.nodes()[e.start_node_idx].property,
        //                             m_graph.nodes()[e.end_node_idx].property);
        //             }

        //             // start return trip
        //             if (std::abs(t_begin + 1 - e.traveltime - m_t) < 1e-10) {
        //                 m_edge_func(m_t, e.traveltime, e.property, m_graph.nodes()[e.start_node_idx].property,
        //                             m_graph.nodes()[e.end_node_idx].property);
        //             }
        //         }

        //         // integrate nodes after mobility updates
        //         for (auto& n : m_graph.nodes()) {
        //             m_node_func(m_t, dt_traveltimes, n.property);
        //         }

        //         m_t += dt_traveltimes;
        //     }

        //     // at the end of the day, we want all travelers to return to their home region
        //     for (auto& e : m_graph.edges()) {
        //         m_edge_func(m_t, dt, e.property, m_graph.nodes()[e.start_node_idx].property,
        //                     m_graph.nodes()[e.end_node_idx].property);
        //     }
        // }
    }

    double get_t() const
    {
        return m_t;
    }

    Graph& get_graph() &
    {
        return m_graph;
    }

    const Graph& get_graph() const&
    {
        return m_graph;
    }

    Graph&& get_graph() &&
    {
        return std::move(m_graph);
    }

protected:
    double m_t;
    double m_dt;
    Graph m_graph;
    node_function m_node_func;
    edge_function m_edge_func;
};

template <class Graph>
class GraphSimulation : public GraphSimulationBase<Graph>
{
    using GraphSimulationBase<Graph>::GraphSimulationBase;
};

template <class Graph>
class GraphSimulationStochastic
    : public GraphSimulationBase<Graph,
                                 std::function<void(typename Graph::EdgeProperty&, size_t,
                                                    typename Graph::NodeProperty&, typename Graph::NodeProperty&)>>
{
    using Base =
        GraphSimulationBase<Graph, std::function<void(typename Graph::EdgeProperty&, size_t,
                                                      typename Graph::NodeProperty&, typename Graph::NodeProperty&)>>;
    using node_function = typename Base::node_function;
    using edge_function = typename Base::edge_function;

public:
    GraphSimulationStochastic(double t0, double dt, const Graph& g, const node_function& node_func,
                              const edge_function&& edge_func)
        : Base(t0, dt, g, node_func, std::move(edge_func))
        , m_rates(Base::m_graph.edges().size() *
                  Base::m_graph.edges()[0].property.get_parameters().get_coefficients().get_shape().rows())
    {
    }

    GraphSimulationStochastic(double t0, double dt, Graph&& g, const node_function& node_func,
                              const edge_function&& edge_func)
        : Base(t0, dt, std::forward<Graph>(g), node_func, std::move(edge_func))
        , m_rates(Base::m_graph.edges().size() *
                  Base::m_graph.edges()[0].property.get_parameters().get_coefficients().get_shape().rows())
    {
    }

    void advance(double t_max)
    {
        //draw normalized waiting time
        ScalarType normalized_waiting_time = ExponentialDistribution<ScalarType>::get_instance()(1.0);
        std::vector<ScalarType> dt_cand(Base::m_graph.nodes().size());
        ScalarType cumulative_rate = 0; //cumulative transition rate
        size_t parameters_per_edge =
            size_t(Base::m_graph.edges()[0].property.get_parameters().get_coefficients().get_shape().rows());
        std::vector<ScalarType> transition_rates(parameters_per_edge * Base::m_graph.edges().size());
        while (Base::m_t < t_max) {
            Base::m_dt = std::min({Base::m_dt, t_max - Base::m_t});
            //calculate current transition rates and cumulative rate
            cumulative_rate = get_cumulative_transition_rate();
            if (cumulative_rate * Base::m_dt >
                normalized_waiting_time) { //at least one transition event during current time step
                do {
                    //evaluate rates
                    get_rates(m_rates);
                    //draw transition event
                    size_t event = mio::DiscreteDistribution<size_t>::get_instance()(m_rates);
                    //edge that performs transition event
                    auto& event_edge = Base::m_graph.edges()[event / parameters_per_edge];
                    //index for compartment and age group migrating
                    auto flat_index = event % parameters_per_edge;

                    //advance nodes until t + (waiting_time / cumulative_rate)
                    for (size_t node_iter = 0; node_iter < Base::m_graph.nodes().size(); ++node_iter) {
                        auto& node = Base::m_graph.nodes()[node_iter];
                        Base::m_node_func(Base::m_t, normalized_waiting_time / cumulative_rate, node.property);
                    }

                    //advance time
                    Base::m_t += normalized_waiting_time / cumulative_rate;

                    //reduce remaining time of current time step
                    Base::m_dt -= normalized_waiting_time / cumulative_rate;

                    //perform transition
                    Base::m_edge_func(event_edge.property, flat_index,
                                      Base::m_graph.nodes()[event_edge.start_node_idx].property,
                                      Base::m_graph.nodes()[event_edge.end_node_idx].property);

                    //calculate new cumulative rate
                    cumulative_rate = get_cumulative_transition_rate();

                    //draw new normalized waiting time
                    normalized_waiting_time = ExponentialDistribution<ScalarType>::get_instance()(1.0);

                } while (cumulative_rate * Base::m_dt > normalized_waiting_time);
            }
            else { //no transition event in current time step
                normalized_waiting_time -= cumulative_rate * Base::m_dt; //reduce waiting time by current time step
            }

            //advance nodes until t+dt
            for (size_t node_iter = 0; node_iter < Base::m_graph.nodes().size(); ++node_iter) {
                auto& node = Base::m_graph.nodes()[node_iter];
                Base::m_node_func(Base::m_t, Base::m_dt, node.property);
                //get new dt of each node
                dt_cand[node_iter] = node.property.get_simulation().get_dt();
            }

            //advance time
            Base::m_t += Base::m_dt;
            //new dt ist the minimal dt of all nodes
            Base::m_dt = *std::min_element(dt_cand.begin(), dt_cand.end());
        }
    }

private:
    ScalarType get_cumulative_transition_rate()
    {
        //compute current cumulative transition rate
        ScalarType cumulative_transition_rate = 0;
        for (auto& e : Base::m_graph.edges()) {
            cumulative_transition_rate +=
                e.property.get_transition_rates(Base::m_graph.nodes()[e.start_node_idx].property).sum();
        }
        return cumulative_transition_rate;
    }

    void get_rates(std::vector<ScalarType>& rates)
    {
        size_t j = 0;
        for (auto& e : Base::m_graph.edges()) {
            auto edge_rates = e.property.get_transition_rates(Base::m_graph.nodes()[e.start_node_idx].property);
            for (Eigen::Index i = 0; i < edge_rates.size(); ++i) {
                const auto compartment_value =
                    Base::m_graph.nodes()[e.start_node_idx].property.get_result().get_last_value()[i];
                rates[j] = (compartment_value < 1.) ? 0. : edge_rates(i);

                j++;
            }
        }
    }

    std::vector<ScalarType> m_rates;
};

template <class Graph, class NodeF, class EdgeF>
auto make_graph_sim(double t0, double dt, Graph&& g, NodeF&& node_func, EdgeF&& edge_func)
{
    return GraphSimulation<std::decay_t<Graph>>(t0, dt, std::forward<Graph>(g), std::forward<NodeF>(node_func),
                                                std::forward<EdgeF>(edge_func));
}

template <class Graph, class NodeF, class EdgeF>
auto make_graph_sim_stochastic(double t0, double dt, Graph&& g, NodeF&& node_func, EdgeF&& edge_func)
{
    return GraphSimulationStochastic<std::decay_t<Graph>>(
        t0, dt, std::forward<Graph>(g), std::forward<NodeF>(node_func), std::forward<EdgeF>(edge_func));
}

} // namespace mio
#endif //MIO_MOBILITY_GRAPH_SIMULATION_H

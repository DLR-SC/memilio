/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Jan Kleinert, Daniel Abele
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
#ifndef SIMULATION_H
#define SIMULATION_H

#include "memilio/config.h"
#include "memilio/compartments/compartmentalmodel.h"
#include "memilio/utils/metaprogramming.h"
#include "memilio/math/stepper_wrapper.h"
#include "memilio/utils/time_series.h"
#include "memilio/math/euler.h"

namespace mio
{

using DefaultIntegratorCore = mio::ControlledStepperWrapper<boost::numeric::odeint::runge_kutta_cash_karp54>;

/**
 * @brief A class for the simulation of a compartment model.
 * @tparam M a CompartmentModel type
 */
template <class M>
class Simulation
{
    static_assert(is_compartment_model<M>::value, "Template parameter must be a compartment model.");

public:
    using Model = M;

    /**
     * @brief setup the simulation with an ODE solver
     * @param[in] model: An instance of a compartmental model
     * @param[in] t0 start time
     * @param[in] dt initial step size of integration
     */
    Simulation(Model const& model, double t0 = 0., double dt = 0.1)
        : m_integratorCore(std::make_shared<DefaultIntegratorCore>())
        , m_model(std::make_unique<Model>(model))
        , m_integrator(
              [&model = *m_model](auto&& y, auto&& t, auto&& dydt) {
                  model.eval_right_hand_side(y, y, t, dydt);
              },
              t0, m_model->get_initial_values(), dt, m_integratorCore)
    {
    }

    /**
     * @brief set the core integrator used in the simulation
     */
    void set_integrator(std::shared_ptr<IntegratorCore> integrator)
    {
        m_integratorCore = std::move(integrator);
        m_integrator.set_integrator(m_integratorCore);
    }

    /**
     * @brief get_integrator
     * @return reference to the core integrator used in the simulation
     */
    IntegratorCore& get_integrator()
    {
        return *m_integratorCore;
    }

    /**
     * @brief get_integrator
     * @return reference to the core integrator used in the simulation
     */
    IntegratorCore const& get_integrator() const
    {
        return *m_integratorCore;
    }

    /**
     * @brief advance simulation to tmax
     * tmax must be greater than get_result().get_last_time_point()
     * @param tmax next stopping point of simulation
     */
    Eigen::Ref<Eigen::VectorXd> advance(double tmax)
    {
        return m_integrator.advance(tmax);
    }

    /**
     * @brief Get the result of the simulation.
     * Return the number of persons in all InfectionState%s.
     * @return The result of the simulation.
     */
    TimeSeries<ScalarType>& get_result()
    {
        return m_integrator.get_result();
    }

    /**
     * @brief get_result returns the final simulation result
     * @return a TimeSeries to represent the final simulation result
     */
    const TimeSeries<ScalarType>& get_result() const
    {
        return m_integrator.get_result();
    }

    /**
     * @brief returns the simulation model used in simulation
     */
    const Model& get_model() const
    {
        return *m_model;
    }

    /**
     * @brief returns the simulation model used in simulation
     */
    Model& get_model()
    {
        return *m_model;
    }

    /**
     * @brief returns the next time step chosen by integrator
    */
    double get_dt() const
    {
        return m_integrator.get_dt();
    }

protected:
    /**
     * @brief Set up a simulation for the given model and ODE system.
     * The ODESystem defines both the initial values and right hand side of the ODE system. The class is required
     * to have the two member functions `Eigen::VectorXd initial_values(const Model&)` and
     * `void right_hand_side(const Model&, Eigen::Ref<const Eigen::VectorXd>, Eigen::Ref<const Eigen::VectorXd>,
     * double, Eigen::Ref<Eigen::VectorXd>)`. They are expected to behave similiar to
     * CompartmentalModel::get_initial_values and CompartmentalModel::eval_right_hand_side, respectively, with an
     * additional first argument that recieves a const& to the model of the Simulation.
     * @param[in] model An instance of a compartmental model.
     * @param[in] t0 Start time for the simulation.
     * @param[in] dt Initial step size of integration.
     * @param[in] sys An instance of ODESystem (by reference).
     * @tparam ODESystem Type defining the initial values and right hand side of the ODE system.
     */
    template <class ODESystem>
    Simulation(Model const& model, double t0, double dt, ODESystem& sys)
        : m_integratorCore(std::make_shared<DefaultIntegratorCore>())
        , m_model(std::make_unique<Model>(model))
        , m_integrator(
              [&model = *m_model, &sys](auto&& y, auto&& t, auto&& dydt) {
                  sys.right_hand_side(model, y, y, t, dydt);
              },
              t0, sys.initial_values(model), dt, m_integratorCore)
    {
    }

private:
    std::shared_ptr<IntegratorCore> m_integratorCore;
    std::unique_ptr<Model> m_model;
    OdeIntegrator m_integrator;
};

template <class M>
class SimulationFlows : public Simulation<M>
{
public:
    using Model = M;
    using Base  = Simulation<M>;

    /**
     * @brief Set up the simulation with an ODE solver.
     * @param[in] model An instance of a compartmental model.
     * @param[in] t0 Start time.
     * @param[in] dt Initial step size of integration.
     */
    SimulationFlows(Model const& model, double t0 = 0., double dt = 0.1)
        : SimulationFlows(model, t0, dt, std::make_unique<ODESystem>(model.get_initial_values(), t0))
    {
    }

    /**
     * @brief Set up the simulation with an ODE solver.
     * @param[in] other A SimulationFlows instance.
     */
    SimulationFlows(const SimulationFlows& other)
        : SimulationFlows(other.get_model(), other.get_result().get_time(0), other.get_dt(),
                          std::make_unique<ODESystem>(other.m_system->pop_result))
    {
    }

    // Default constructors may be unsafe. Implement using private constructor as needed.
    SimulationFlows(SimulationFlows&&)                 = delete;
    SimulationFlows& operator=(const SimulationFlows&) = delete;
    SimulationFlows& operator=(SimulationFlows&&)      = delete;

    /**
     * @brief get_result returns the values for all compartments within the
     * simulated model for each time step.
     * @return a TimeSeries to represent a numerical solution for the model. 
     * For each simulated time step, the TimeSeries containts the population  
     * size for each compartment. 
     */
    TimeSeries<ScalarType>& get_result()
    {
        // overwrite get_result from the base class, so that it also returns compartments (instead of flows)
        compute_population_results();
        return m_system->pop_result;
    }

    /**
     * @brief get_result returns the values for all compartments within the
     * simulated model for each time step.
     * @return a TimeSeries to represent a numerical solution for the model. 
     * For each simulated time step, the TimeSeries containts the population  
     * size for each compartment. 
     */
    const TimeSeries<ScalarType>& get_result() const
    {
        // overwrite get_result from the base class, so that it also returns compartments (instead of flows)
        compute_population_results();
        return m_system->pop_result;
    }

    /**
     * @brief get_flows returns the values describing the transition between 
     * compartments for each time step.
     *
     * Which flows are used by the model is defined by the Flows template 
     * argument for the CompartmentalModel using an explicit FlowChart.
     * To get the correct index for the flow between two compartments use 
     * CompartmentalModel::get_flow_index.
     *
     * @return a TimeSeries to represent a numerical solution for the 
     * flows in the model. 
     * For each simulated time step, the TimeSeries containts the value for 
     * each flow. 
     */
    TimeSeries<ScalarType>& get_flows()
    {
        // use get_result from the base class
        return Base::get_result();
    }

    /**
     * @brief get_flows returns the values describing the transition between 
     * compartments for each time step.
     *
     * Which flows are used by the model is defined by the Flows template 
     * argument for the CompartmentalModel using an explicit FlowChart.
     * To get the correct index for the flow between two compartments use 
     * CompartmentalModel::get_flow_index.
     *
     * @return a TimeSeries to represent a numerical solution for the 
     * flows in the model. 
     * For each simulated time step, the TimeSeries containts the value for 
     * each flow. 
     */
    const TimeSeries<ScalarType>& get_flows() const
    {
        // use get_result from the base class
        return Base::get_result();
    }

private:
    struct ODESystem {
        /// @brief Set up ODESystem with initial conditions of the simulation.
        ODESystem(Eigen::Ref<const Eigen::VectorXd> population0, ScalarType t0)
            : pop(population0.size())
            , pop_result(t0, population0)

        {
        }

        /// @brief Set up ODESystem with results from another simulation.
        ODESystem(const TimeSeries<ScalarType>& results)
            : pop(results.get_num_elements())
            , pop_result(results)

        {
        }

        /// @brief Stand-in for model.eval_right_hand_side(...), that calculates the derivative of flows.
        inline void right_hand_side(const Model& model, Eigen::Ref<const Eigen::VectorXd>,
                                    Eigen::Ref<const Eigen::VectorXd> flows, double t,
                                    Eigen::Ref<Eigen::VectorXd> dflows_dt)
        {
            // compute current population
            //   flows contains the accumulated outflows of each compartment for each target compartment at time t.
            //   Using that the ODEs are linear expressions of the flows, get_derivatives can compute the total change
            //   in population from t0 to t.
            //   To incorporate external changes to the last values of pop_result (e.g. by applying mobility), we only
            //   calculate the change in population starting from the last available time point in pop_result, instead
            //   of starting at t0. To do that, the following difference of flows is used.
            model.get_derivatives(flows - flow_result->get_value(pop_result.get_num_time_points() - 1),
                                  pop); // note: overwrites values in pop
            //   add the "initial" value of the ODEs (using last available time point in pop_result)
            //     If no changes were made to the last value in pop_result outside of SimulationFlows, the following
            //     line computes the same as `model.get_derivatives(flows, x); x += model.get_initial_values();`.
            pop += pop_result.get_last_value();
            // compute the current change in flows with respect to the current population
            dflows_dt.setZero();
            model.get_flows(pop, pop, t, dflows_dt); // this result is used by the integrator
        }

        /// @brief Stand-in for model.get_initial_values(), that returns the initial values for the flows.
        inline Eigen::VectorXd initial_values(const Model& model)
        {
            return model.get_initial_flows();
        }

        Eigen::VectorXd pop; ///< pre-allocated temporary, used in right_hand_side()
        mio::TimeSeries<ScalarType> pop_result; ///< population result of the simulation
        mio::TimeSeries<ScalarType>* flow_result; ///< pointer to the flow result of the simulation (computed by Base)
    };

    /// @brief Internal constructor needed for correct initialization order (ODESystem -> Base -> SimulationFlows).
    SimulationFlows(Model const& model, double t0, double dt, std::unique_ptr<ODESystem> sys)
        : Base(model, t0, dt, *sys)
        , m_system(std::move(sys)) // take ownership of *sys
    {
        // this pointer allows ODESystem::right_hand_side to depend on the (potentially modified) last result value
        m_system->flow_result = &get_flows();
    }

    /**
     * @brief Computes the population based on the simulated flows.
     * Uses the same method as ODESystem::right_hand_side to compute the population given the flows and initil values.
     * Adds time points to m_system->pop_result until it has the same number of time points as flow result
     * (get_flows()). Does not recalculate older values.
     */
    void compute_population_results() const
    {
        const auto& flows = get_flows();
        const auto& model = this->get_model();
        auto& result      = m_system->pop_result;
        // calculate new time points
        for (Eigen::Index i = result.get_num_time_points(); i < flows.get_num_time_points(); i++) {
            result.add_time_point(flows.get_time(i));
            model.get_derivatives(flows.get_value(i), result.get_value(i));
            result.get_value(i) += result.get_value(0); // TODO: is "... += model.get_initial_values();" better here?
        }
    }

    std::unique_ptr<ODESystem> m_system; ///< instance of ODESystem supplied to Base constructor
};

/**
 * Defines the return type of the `advance` member function of a type.
 * Template is invalid if this member function does not exist.
 * @tparam Sim a compartment model simulation type.
 */
template <class Sim>
using advance_expr_t = decltype(std::declval<Sim>().advance(std::declval<double>()));

/**
 * Template meta function to check if a type is a compartment model simulation. 
 * Defines a static constant of name `value`. 
 * The constant `value` will be equal to true if Sim is a valid compartment simulation type.
 * Otherwise, `value` will be equal to false.
 * @tparam Sim a type that may or may not be a compartment model simulation.
 */
template <class Sim>
using is_compartment_model_simulation =
    std::integral_constant<bool, (is_expression_valid<advance_expr_t, Sim>::value &&
                                  is_compartment_model<typename Sim::Model>::value)>;

/**
 * @brief simulate simulates a compartmental model
 * @param[in] t0 start time
 * @param[in] tmax end time
 * @param[in] dt initial step size of integration
 * @param[in] model: An instance of a compartmental model
 * @return a TimeSeries to represent the final simulation result
 * @tparam Model a compartment model type
 * @tparam Sim a simulation type that can simulate the model.
 */
template <class Model, class Sim = Simulation<Model>>
TimeSeries<ScalarType> simulate(double t0, double tmax, double dt, Model const& model,
                                std::shared_ptr<IntegratorCore> integrator = nullptr)
{
    model.check_constraints();
    Sim sim(model, t0, dt);
    if (integrator) {
        sim.set_integrator(integrator);
    }
    sim.advance(tmax);
    return sim.get_result();
}

/**
 * @brief simulate simulates a compartmental model and returns the same results as simulate and also the flows.
 * @param[in] t0 start time
 * @param[in] tmax end time
 * @param[in] dt initial step size of integration
 * @param[in] model: An instance of a compartmental model
 * @return a Tuple of two TimeSeries to represent the final simulation result and flows
 * @tparam Model a compartment model type
 * @tparam Sim a simulation type that can simulate the model.
 */
template <class Model, class Sim = SimulationFlows<Model>>
std::vector<TimeSeries<ScalarType>> simulate_flows(double t0, double tmax, double dt, Model const& model,
                                                   std::shared_ptr<IntegratorCore> integrator = nullptr)
{
    model.check_constraints();
    Sim sim(model, t0, dt);
    if (integrator) {
        sim.set_integrator(integrator);
    }
    sim.advance(tmax);
    return {sim.get_result(), sim.get_flows()};
}
} // namespace mio

#endif // SIMULATION_H

/*
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Rene Schmieding
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
#ifndef STEPPER_WRAPPER_H_
#define STEPPER_WRAPPER_H_

#include "memilio/utils/compiler_diagnostics.h"
#include "memilio/math/integrator.h"

GCC_CLANG_DIAGNOSTIC(push)
GCC_CLANG_DIAGNOSTIC(ignored "-Wshadow")
GCC_CLANG_DIAGNOSTIC(ignored "-Wlanguage-extension-token")
MSVC_WARNING_DISABLE_PUSH(4127)
#include "boost/numeric/odeint/external/eigen/eigen_algebra.hpp"
#include "boost/numeric/odeint/stepper/controlled_runge_kutta.hpp"
#include "boost/numeric/odeint/stepper/runge_kutta4.hpp"
#include "boost/numeric/odeint/stepper/runge_kutta_fehlberg78.hpp"
#include "boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp"
// #include "boost/numeric/odeint/stepper/runge_kutta_dopri5.hpp" // TODO: reenable once boost bug is fixed
MSVC_WARNING_POP()
GCC_CLANG_DIAGNOSTIC(pop)

namespace mio
{

/**
 * @brief Creates and manages an instance of a boost::numeric::odeint::controlled_runge_kutta
 * integrator, wrapped as mio::IntegratorCore.
 */
template <template <class State = Eigen::VectorXd, class Value = double, class Deriv = State, class Time = double,
                    class Algebra    = boost::numeric::odeint::vector_space_algebra,
                    class Operations = typename boost::numeric::odeint::operations_dispatcher<State>::operations_type,
                    class Resizer    = boost::numeric::odeint::never_resizer>
          class ControlledStepper>
class ControlledStepperWrapper : public mio::IntegratorCore
{
    using Stepper                         = boost::numeric::odeint::controlled_runge_kutta<ControlledStepper<>>;
    static constexpr bool is_fsal_stepper = std::is_same_v<typename Stepper::stepper_type::stepper_category,
                                                           boost::numeric::odeint::explicit_error_stepper_fsal_tag>;
    static_assert(!is_fsal_stepper,
                  "FSAL steppers cannot be used unitl https://github.com/boostorg/odeint/issues/72 is resolved.");

public:
    /**
     * @brief Set up the integrator
     * @param abs_tol absolute tolerance
     * @param rel_tol relative tolerance 
     * @param dt_min lower bound for time step dt
     * @param dt_max upper bound for time step dt
     */
    ControlledStepperWrapper(double abs_tol = 1e-10, double rel_tol = 1e-5,
                             double dt_min = std::numeric_limits<double>::min(),
                             double dt_max = std::numeric_limits<double>::max())
        : m_abs_tol(abs_tol)
        , m_rel_tol(rel_tol)
        , m_dt_min(dt_min)
        , m_dt_max(dt_max)
        , m_stepper(create_stepper())
    {
    }

    /**
    * @brief Make a single integration step of a system of ODEs and adapt the step size dt.
    * @param[in] yt Value of y at t_{k}, y(t_{k}).
    * @param[in,out] t Current time step t_{k} for some k. Will be set to t_{k+1} in [t_{k} + dt_min, t_{k} + dt].
    * @param[in,out] dt Current time step size h=dt. Overwritten by an estimated optimal step size for the next step.
    * @param[out] ytp1 The approximated value of y(t_{k+1}).
    */
    bool step(const mio::DerivFunction& f, Eigen::Ref<Eigen::VectorXd const> yt, double& t, double& dt,
              Eigen::Ref<Eigen::VectorXd> ytp1) const override
    {
        using boost::numeric::odeint::fail;
        assert(0 <= m_dt_min);
        assert(m_dt_min <= m_dt_max);
        // set initial values for exit conditions
        auto step_result = fail;
        bool is_dt_valid = true;
        // copy vectors from the references, since the stepper cannot (trivially) handle Eigen::Ref
        m_ytp1 = ytp1;
        m_yt   = yt;
        // make a integration step, adapting dt to a possibly larger value on success,
        // or a strictly smaller value on fail.
        // stop only on a successful step or a failed step size adaption (w.r.t. the minimal step size m_dt_min)
        while (step_result == fail && is_dt_valid) {
            if (dt < m_dt_min) {
                is_dt_valid = false;
                dt          = m_dt_min;
            }
            // we use the scheme try_step(sys, in, t, out, dt) with sys=f, in=y(t_{k}), out=y(t_{k+1}).
            // this is similiar to do_step, but it can adapt the step size dt. if successfull, it also updates t.
            if constexpr (!is_fsal_stepper) { // prevent compile time errors with fsal steppers
                step_result = m_stepper.try_step(
                    // reorder arguments of the DerivFunction f for the stepper
                    [&](const Eigen::VectorXd& x, Eigen::VectorXd& dxds, double s) {
                        dxds.resizeLike(x); // boost resizers cannot resize Eigen::Vector, hence we need to do that here
                        f(x, s, dxds);
                    },
                    m_yt, t, m_ytp1, dt);
            }
        }
        // output the new value by copying it back to the reference
        ytp1 = m_ytp1;
        // bound dt from below
        // the last adaptive step (successfull or not) may have calculated a new step size smaller than m_dt_min
        dt = std::max(dt, m_dt_min);
        // check whether the last step failed (which means that m_dt_min was still too large to suffice tolerances)
        if (step_result == fail) {
            // adaptive stepping failed, but we still return the result of the last attempt
            t += m_dt_min;
            return false;
        }
        else {
            // successfully made an integration step
            return true;
        }
    }

    /// @param tol the required absolute tolerance for comparison of the iterative approximation
    void set_abs_tolerance(double abs_tol)
    {
        m_abs_tol = abs_tol;
        m_stepper = create_stepper();
    }

    /// @param tol the required relative tolerance for comparison of the iterative approximation
    void set_rel_tolerance(double rel_tol)
    {
        m_rel_tol = rel_tol;
        m_stepper = create_stepper();
    }

    /// @param dt_min sets the minimum step size
    void set_dt_min(double dt_min)
    {
        m_dt_min = dt_min;
    }

    /// @param dt_max sets the maximum step size
    void set_dt_max(double dt_max)
    {
        m_dt_max  = dt_max;
        m_stepper = create_stepper();
    }

private:
    /// @brief (Re)initialize the internal stepper.
    Stepper create_stepper()
    {
        // for more options see: boost/boost/numeric/odeint/stepper/controlled_runge_kutta.hpp
        return Stepper(typename Stepper::error_checker_type(m_abs_tol, m_rel_tol),
                       typename Stepper::step_adjuster_type(m_dt_max));
    }

    double m_abs_tol, m_rel_tol; ///< Absolute and relative tolerances for integration.
    double m_dt_min, m_dt_max; ///< Lower and upper bound to the step size dt.
    mutable Eigen::VectorXd m_ytp1, m_yt; ///< Temporary storage to avoid allocations in step function.
    mutable Stepper m_stepper; ///< A stepper instance used for integration.
};

} // namespace mio

#endif

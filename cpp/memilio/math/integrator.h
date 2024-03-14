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
#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "memilio/utils/time_series.h"
#include "memilio/utils/logging.h"
#include <memory>
#include <functional>

namespace mio
{

/**
 * Function template to be integrated
 */
template <typename FP = double>
using DerivFunction = std::function<void(Eigen::Ref<const typename Eigen::Matrix<FP, Eigen::Dynamic, 1>> y, FP t,
                                         Eigen::Ref<typename Eigen::Matrix<FP, Eigen::Dynamic, 1>> dydt)>;

template <typename FP = double>
class IntegratorCore
{
public:
    virtual ~IntegratorCore(){};

    /**
     * @brief Make a single integration step.
     *
     * The behaviour of this method changes when the integration scheme has adaptive step sizing. 
     * These changes are noted in the parentheses (...) below.
     * Adaptive integrators must have bounds dt_min and dt_max for dt.
     * The adaptive step sizing is considered to be successful, if a step of at least size dt_min sufficed tolerances.
     * Tolerances are defined in each implementation, usually using a criterion with absolute and relative tolerances.
     * Even if the step sizing failed, the integrator will make a step of at least size dt_min.
     *
     * @param[in] f Right hand side of the ODE. May be called multiple times with different arguments.
     * @param[in] yt The known value of y at time t.
     * @param[in,out] t The current time. It will be increased by dt.
     *     (If adaptive, the increment is instead within [dt_min, dt].)
     * @param[in,out] dt The current step size h=dt. Will not be changed.
     *     (If adaptive, the given dt is used as the maximum step size, and must be within [dt_min, dt_max].
     *      During integration, dt is adjusted in [dt_min, dt_max] to have an optimal size for the next step.)
     * @param[out] ytp1 Set to the approximated value of y at time t + dt.
     *     (If adaptive, this time may be smaller, but it is at least t + dt_min, at most t + dt_max.
     *      Note that the increment on t may be different from the returned value of dt.)
     * @return Always true for nonadaptive methods.
     *     (If adaptive, returns whether the adaptive step sizing was successful.)
     */
    virtual bool step(const DerivFunction<FP>& f, Eigen::Ref<const Eigen::Matrix<FP, Eigen::Dynamic, 1>> yt, FP& t,
                      FP& dt, Eigen::Ref<Eigen::Matrix<FP, Eigen::Dynamic, 1>> ytp1) const = 0;
};

/**
 * Integrate initial value problems (IVP) of ordinary differential equations (ODE) of the form y' = f(y, t), y(t0) = y0.
 * tparam FP a floating point type accepted by Eigen
 */
template <typename FP = double>
class OdeIntegrator
{
public:
    /**
     * @brief create an integrator for a specific IVP
     * @param[in] core implements the solution method
     */
    OdeIntegrator(std::shared_ptr<IntegratorCore<FP>> core)
        : m_core(core)
    {
    }

    /**
     * @brief Advance the integrator.
     * @param[in] f The rhs of the ODE.
     * @param[in] tmax Time end point. Must be greater than results.get_last_time().
     * @param[in, out] dt Initial integration step size. May be changed by the IntegratorCore.
     * @param[in, out] results List of results. Must contain at least one time point. The last entry is used as
     * intitial time and value. A new entry is added for each integration step.
     * @return A reference to the last value in the results time series.
     */

    Eigen::Ref<Eigen::Matrix<FP, Eigen::Dynamic, 1>> advance(const DerivFunction<FP>& f, const FP tmax, FP& dt,
                                                             TimeSeries<FP>& results)
    {
        using std::fabs;
        using std::min;
        const auto t0 = results.get_last_time();
        assert(tmax > t0);
        assert(dt > 0);

        const size_t nb_steps = (int)(ceil((tmax - t0) / dt)); // estimated number of time steps (if equidistant)

        results.reserve(results.get_num_time_points() + nb_steps);

        bool step_okay = true;

        auto t   = t0;
        size_t i = results.get_num_time_points() - 1;
        while (fabs((tmax - t) / (tmax - t0)) > 1e-10) {
            //we don't make timesteps too small as the error estimator of an adaptive integrator
            //may not be able to handle it. this is very conservative and maybe unnecessary,
            //but also unlikely to happen. may need to be reevaluated

            auto dt_eff = min(dt, tmax - t);
            results.add_time_point();
            step_okay &= m_core->step(f, results[i], t, dt_eff, results[i + 1]);
            results.get_last_time() = t;

            ++i;

            if (fabs((tmax - t) / (tmax - t0)) > 1e-10 || dt_eff > dt) {
                //store dt only if it's not the last step as it is probably smaller than required for tolerances
                //except if the step function returns a bigger step size so as to not lose efficiency
                dt = dt_eff;
            }
        }

        if (!step_okay) {
            log_warning("Adaptive step sizing failed. Forcing an integration step of size dt_min.");
        }
        else if (fabs((tmax - t) / (tmax - t0)) > 1e-14) {
            log_warning("Last time step too small. Could not reach tmax exactly.");
        }
        else {
            log_info("Adaptive step sizing successful to tolerances.");
        }

        return results.get_last_value();
    }

    void set_integrator(std::shared_ptr<IntegratorCore<FP>> integrator)
    {
        m_core = integrator;
    }

private:
    std::shared_ptr<IntegratorCore<FP>> m_core;
};

} // namespace mio

#endif // INTEGRATOR_H

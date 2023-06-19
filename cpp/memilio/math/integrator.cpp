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
#include "memilio/math/integrator.h"
#include "memilio/utils/logging.h"

namespace mio
{

Eigen::Ref<Eigen::VectorXd> OdeIntegrator::advance(double tmax)
{
    const double t0 = m_result.get_time(m_result.get_num_time_points() - 1);
    assert(tmax > t0);

    const size_t nb_steps = (int)(ceil((tmax - t0) / m_dt)); // estimated number of time steps (if equidistant)

    m_result.reserve(m_result.get_num_time_points() + nb_steps);

    bool step_okay = true;

    double t = t0;
    size_t i = m_result.get_num_time_points() - 1;
    while (std::abs((tmax - t) / (tmax - t0)) > 1e-10) {
        //we don't make timesteps too small as the error estimator of an adaptive integrator
        //may not be able to handle it. this is very conservative and maybe unnecessary,
        //but also unlikely to happen. may need to be reevaluated

        auto dt_eff = std::min(m_dt, tmax - t);
        m_result.add_time_point();
        step_okay &= m_core->step(m_f, m_result[i], t, dt_eff, m_result[i + 1]);
        m_result.get_last_time() = t;
        m_next_dt                = dt_eff;

        ++i;

        if (std::abs((tmax - t) / (tmax - t0)) > 1e-10 || dt_eff > m_dt) {
            //store dt only if it's not the last step as it is probably smaller than required for tolerances
            //except if the step function returns a bigger step size so as to not lose efficiency
            m_dt = dt_eff;
        }
    }

    if (!step_okay) {
        log_warning("Adaptive step sizing failed.");
    }
    else if (std::abs((tmax - t) / (tmax - t0)) > 1e-15) {
        log_warning("Last time step too small. Could not reach tmax exactly.");
    }
    else {
        log_info("Adaptive step sizing successful to tolerances.");
    }

    return m_result.get_last_value();
}

Eigen::Ref<Eigen::VectorXd> SplitOdeIntegrator::advance(double tmax)
{
    assert(m_result_head.get_num_time_points() == m_result_tail.get_num_time_points());
    const double t0 = m_result_head.get_time(m_result_head.get_num_time_points() - 1);
    assert(tmax > t0);

    const size_t nb_steps = (int)(ceil((tmax - t0) / m_dt)); // estimated number of time steps (if equidistant)

    m_result_head.reserve(m_result_head.get_num_time_points() + nb_steps);
    m_result_tail.reserve(m_result_tail.get_num_time_points() + nb_steps);

    bool step_okay = true;

    double t = t0;
    size_t i = m_result_head.get_num_time_points() - 1;
    while (std::abs((tmax - t) / (tmax - t0)) > 1e-10) {
        //we don't make timesteps too small as the error estimator of an adaptive integrator
        //may not be able to handle it. this is very conservative and maybe unnecessary,
        //but also unlikely to happen. may need to be reevaluated

        auto dt_eff = std::min(m_dt, tmax - t);
        m_result_head.add_time_point();
        m_result_tail.add_time_point();

        // concat m_result_heads and m_result_tail for time integration
        m_yt << m_result_head[i], m_result_tail[i];

        // integrate both systems in one call
        step_okay &= m_core->step(m_f, m_yt, t, dt_eff, m_ytp1);

        // split m_ytp1 back in m_result_heads and m_result_tail
        m_result_head[i + 1] = m_ytp1.head(m_result_head.get_num_elements());
        m_result_tail[i + 1] = m_ytp1.tail(m_result_tail.get_num_elements());

        m_result_head.get_last_time() = t;
        m_result_tail.get_last_time() = t;

        ++i;

        if (std::abs((tmax - t) / (tmax - t0)) > 1e-10 || dt_eff > m_dt) {
            //store dt only if it's not the last step as it is probably smaller than required for tolerances
            //except if the step function returns a bigger step size so as to not lose efficiency
            m_dt = dt_eff;
        }
    }

    if (!step_okay) {
        log_warning("Adaptive step sizing failed.");
    }
    else if (std::abs((tmax - t) / (tmax - t0)) > 1e-15) {
        log_warning("Last time step too small. Could not reach tmax exactly.");
    }
    else {
        log_info("Adaptive step sizing successful to tolerances.");
    }

    return m_result_head.get_last_value();
}

} // namespace mio

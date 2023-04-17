/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*        & Helmholtz Centre for Infection Research (HZI)
*
* Authors: Khoa Nguyen
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
#ifndef ABM_ANALYZE_RESULT_H
#define ABM_ANALYZE_RESULT_H

#include "abm/simulation.h"
#include "abm/parameters.h"
#include "memilio/data/analyze_result.h"

#include <functional>
#include <vector>

namespace mio
{
namespace abm
{
/**
    * @brief computes the p percentile of the parameters for each node.
    * @param ensemble_result graph of multiple simulation runs
    * @param p percentile value in open interval (0, 1)
    * @return p percentile of the parameters over all runs
    */
template <class Model>
std::vector<Model> ensemble_params_percentile(const std::vector<std::vector<Model>>& ensemble_params, double p)
{
    assert(p > 0.0 && p < 1.0 && "Invalid percentile value.");

    auto num_runs  = ensemble_params.size();
    auto num_nodes = ensemble_params[0].size();
    std::vector<double> single_element_ensemble(num_runs);
    auto num_groups = (size_t)ensemble_params[0][0].parameters.get_num_groups();

    // lamda function that calculates the percentile of a single paramter
    std::vector<Model> percentile(num_nodes, Model((int)num_groups));
    auto param_percentil = [&ensemble_params, p, num_runs, &percentile](auto n, auto get_param) mutable {
        std::vector<double> single_element(num_runs);
        for (size_t run = 0; run < num_runs; run++) {
            auto const& params  = ensemble_params[run][n];
            single_element[run] = get_param(params);
        }
        std::sort(single_element.begin(), single_element.end());
        auto& new_params = get_param(percentile[n]);
        new_params       = single_element[static_cast<size_t>(num_runs * p)];
    };

    for (size_t node = 0; node < num_nodes; node++) {
        for (auto age_group = AgeGroup(0); age_group < AgeGroup(num_groups); age_group++) {
            // Global infection parameters
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<IncubationPeriod>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters
                        .template get<SusceptibleToExposedByCarrier>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters
                        .template get<SusceptibleToExposedByInfected>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<CarrierToInfected>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<CarrierToRecovered>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<InfectedToRecovered>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<InfectedToSevere>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<SevereToCritical>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<SevereToRecovered>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<CriticalToDead>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<CriticalToRecovered>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters
                        .template get<RecoveredToSusceptible>()[{age_group, VaccinationState::Unvaccinated}];
                });
            param_percentil(
                node, [age_group](auto&& model) -> auto& {
                    return model.parameters.template get<DetectInfection>()[{age_group, VaccinationState::Unvaccinated}];
                });
        }
        for (size_t run = 0; run < num_runs; run++) {
            auto const& params           = ensemble_params[run][node];
            single_element_ensemble[run] = static_cast<double>(params.parameters.size());
        }
        std::sort(single_element_ensemble.begin(), single_element_ensemble.end());
    }

    return percentile;
}

} // namespace abm
} // namespace mio

#endif
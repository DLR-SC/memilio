/* 
* * Copyright (C) 2020-2024 MEmilio
*
* Authors: Henrik Zunker, Wadim Koslow, Daniel Abele, Martin J. Kühn
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
#ifndef ODESECIRVVS_MODEL_H
#define ODESECIRVVS_MODEL_H

#include "memilio/compartments/flow_model.h"
#include "memilio/compartments/simulation.h"
#include "memilio/compartments/flow_simulation.h"
#include "memilio/epidemiology/populations.h"
#include "ode_secirvvs/infection_state.h"
#include "ode_secirvvs/parameters.h"
#include "memilio/math/smoother.h"
#include "memilio/math/eigen_util.h"

namespace mio
{
namespace osecirvvs
{
// clang-format off
using Flows = TypeList<
    //naive
    Flow<InfectionState::SusceptibleNaive,                            InfectionState::ExposedNaive>, 
    Flow<InfectionState::SusceptibleNaive,                            InfectionState::TemporaryImmunPartialImmunity>, 
    Flow<InfectionState::ExposedNaive,                                InfectionState::InfectedNoSymptomsNaive>,
    Flow<InfectionState::InfectedNoSymptomsNaive,                     InfectionState::InfectedSymptomsNaive>,
    Flow<InfectionState::InfectedNoSymptomsNaive,                     InfectionState::TemporaryImmunPartialImmunity>,
    Flow<InfectionState::InfectedNoSymptomsNaiveConfirmed,            InfectionState::InfectedSymptomsNaiveConfirmed>,
    Flow<InfectionState::InfectedNoSymptomsNaiveConfirmed,            InfectionState::TemporaryImmunPartialImmunity>,
    Flow<InfectionState::InfectedSymptomsNaive,                       InfectionState::InfectedSevereNaive>,
    Flow<InfectionState::InfectedSymptomsNaive,                       InfectionState::TemporaryImmunPartialImmunity>,
    Flow<InfectionState::InfectedSymptomsNaiveConfirmed,              InfectionState::InfectedSevereNaive>,
    Flow<InfectionState::InfectedSymptomsNaiveConfirmed,              InfectionState::TemporaryImmunPartialImmunity>,
    Flow<InfectionState::InfectedSevereNaive,                         InfectionState::InfectedCriticalNaive>,
    Flow<InfectionState::InfectedSevereNaive,                         InfectionState::TemporaryImmunPartialImmunity>, 
    Flow<InfectionState::InfectedSevereNaive,                         InfectionState::DeadNaive>,
    Flow<InfectionState::InfectedCriticalNaive,                       InfectionState::DeadNaive>,
    Flow<InfectionState::InfectedCriticalNaive,                       InfectionState::TemporaryImmunPartialImmunity>,
    Flow<InfectionState::TemporaryImmunPartialImmunity,                          InfectionState::SusceptiblePartialImmunity>,
    //partial immunity
    Flow<InfectionState::SusceptiblePartialImmunity,                  InfectionState::ExposedPartialImmunity>,
    Flow<InfectionState::SusceptiblePartialImmunity,                  InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::ExposedPartialImmunity,                      InfectionState::InfectedNoSymptomsPartialImmunity>,
    Flow<InfectionState::InfectedNoSymptomsPartialImmunity,           InfectionState::InfectedSymptomsPartialImmunity>,
    Flow<InfectionState::InfectedNoSymptomsPartialImmunity,           InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedNoSymptomsPartialImmunityConfirmed,  InfectionState::InfectedSymptomsPartialImmunityConfirmed>,
    Flow<InfectionState::InfectedNoSymptomsPartialImmunityConfirmed,  InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedSymptomsPartialImmunity,             InfectionState::InfectedSeverePartialImmunity>,
    Flow<InfectionState::InfectedSymptomsPartialImmunity,             InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedSymptomsPartialImmunityConfirmed,    InfectionState::InfectedSeverePartialImmunity>,
    Flow<InfectionState::InfectedSymptomsPartialImmunityConfirmed,    InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedSeverePartialImmunity,               InfectionState::InfectedCriticalPartialImmunity>,
    Flow<InfectionState::InfectedSeverePartialImmunity,               InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedSeverePartialImmunity,               InfectionState::DeadPartialImmunity>,
    Flow<InfectionState::InfectedCriticalPartialImmunity,             InfectionState::DeadPartialImmunity>,
    Flow<InfectionState::InfectedCriticalPartialImmunity,             InfectionState::TemporaryImmunImprovedImmunity>,
    //improved immunity
    Flow<InfectionState::SusceptibleImprovedImmunity,                 InfectionState::ExposedImprovedImmunity>,
    Flow<InfectionState::SusceptibleImprovedImmunity,                 InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::ExposedImprovedImmunity,                     InfectionState::InfectedNoSymptomsImprovedImmunity>,
    Flow<InfectionState::InfectedNoSymptomsImprovedImmunity,          InfectionState::InfectedSymptomsImprovedImmunity>,
    Flow<InfectionState::InfectedNoSymptomsImprovedImmunity,          InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedNoSymptomsImprovedImmunityConfirmed, InfectionState::InfectedSymptomsImprovedImmunityConfirmed>,
    Flow<InfectionState::InfectedNoSymptomsImprovedImmunityConfirmed, InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedSymptomsImprovedImmunity,            InfectionState::InfectedSevereImprovedImmunity>,
    Flow<InfectionState::InfectedSymptomsImprovedImmunity,            InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedSymptomsImprovedImmunityConfirmed,   InfectionState::InfectedSevereImprovedImmunity>,
    Flow<InfectionState::InfectedSymptomsImprovedImmunityConfirmed,   InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedSevereImprovedImmunity,              InfectionState::InfectedCriticalImprovedImmunity>,
    Flow<InfectionState::InfectedSevereImprovedImmunity,              InfectionState::TemporaryImmunImprovedImmunity>,
    Flow<InfectionState::InfectedSevereImprovedImmunity,              InfectionState::DeadImprovedImmunity>,
    Flow<InfectionState::InfectedCriticalImprovedImmunity,            InfectionState::DeadImprovedImmunity>,
    Flow<InfectionState::InfectedCriticalImprovedImmunity,            InfectionState::TemporaryImmunImprovedImmunity>,
    
    // waning
    Flow<InfectionState::TemporaryImmunPartialImmunity,                          InfectionState::SusceptiblePartialImmunity>,
    Flow<InfectionState::TemporaryImmunImprovedImmunity,                          InfectionState::SusceptibleImprovedImmunity>,
    Flow<InfectionState::SusceptibleImprovedImmunity,                 InfectionState::SusceptiblePartialImmunity>,
    Flow<InfectionState::SusceptiblePartialImmunity,                  InfectionState::SusceptibleNaive>>;

// clang-format on

class Model : public FlowModel<InfectionState, Populations<AgeGroup, InfectionState>, Parameters, Flows>
{
    using Base = FlowModel<InfectionState, mio::Populations<AgeGroup, InfectionState>, Parameters, Flows>;

public:
    Model(const Populations& pop, const ParameterSet& params)
        : Base(pop, params)
    {
    }

    Model(int num_agegroups)
        : Model(Populations({AgeGroup(num_agegroups), InfectionState::Count}), ParameterSet(AgeGroup(num_agegroups)))
    {
    }

    void get_flows(Eigen::Ref<const Eigen::VectorXd> pop, Eigen::Ref<const Eigen::VectorXd> y, double t,
                   Eigen::Ref<Eigen::VectorXd> flows) const override
    {
        auto const& params   = this->parameters;
        AgeGroup n_agegroups = params.get_num_groups();

        ContactMatrixGroup const& contact_matrix = params.get<ContactPatterns>();

        auto icu_occupancy           = 0.0;
        auto test_and_trace_required = 0.0;
        for (auto i = AgeGroup(0); i < n_agegroups; ++i) {
            test_and_trace_required +=
                (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i]) / params.get<TimeInfectedNoSymptoms>()[i] *
                (this->populations.get_from(pop, {i, InfectionState::InfectedNoSymptomsNaive}) +
                 this->populations.get_from(pop, {i, InfectionState::InfectedNoSymptomsPartialImmunity}) +
                 this->populations.get_from(pop, {i, InfectionState::InfectedNoSymptomsImprovedImmunity}) +
                 this->populations.get_from(pop, {i, InfectionState::InfectedNoSymptomsNaiveConfirmed}) +
                 this->populations.get_from(pop, {i, InfectionState::InfectedNoSymptomsPartialImmunityConfirmed}) +
                 this->populations.get_from(pop, {i, InfectionState::InfectedNoSymptomsImprovedImmunityConfirmed}));
            icu_occupancy += this->populations.get_from(pop, {i, InfectionState::InfectedCriticalNaive}) +
                             this->populations.get_from(pop, {i, InfectionState::InfectedCriticalPartialImmunity}) +
                             this->populations.get_from(pop, {i, InfectionState::InfectedCriticalImprovedImmunity});
        }

        // get vaccinations with
        // Eigen::VectorXd vaccinations_at(const CustomIndexArray<double, AgeGroup, SimulationDay>& daily_vaccinations,
        // const Scalartype t, const ScalarType eps = 0.15) const
        auto const& partial_vaccination = vaccinations_at(this->parameters.get<DailyPartialVaccination>(), t);

        mio::unused(partial_vaccination);

        for (auto i = AgeGroup(0); i < n_agegroups; i++) {

            size_t SNi    = this->populations.get_flat_index({i, InfectionState::SusceptibleNaive});
            size_t ENi    = this->populations.get_flat_index({i, InfectionState::ExposedNaive});
            size_t INSNi  = this->populations.get_flat_index({i, InfectionState::InfectedNoSymptomsNaive});
            size_t ISyNi  = this->populations.get_flat_index({i, InfectionState::InfectedSymptomsNaive});
            size_t ISevNi = this->populations.get_flat_index({i, InfectionState::InfectedSevereNaive});
            size_t ICrNi  = this->populations.get_flat_index({i, InfectionState::InfectedCriticalNaive});

            size_t INSNCi = this->populations.get_flat_index({i, InfectionState::InfectedNoSymptomsNaiveConfirmed});
            size_t ISyNCi = this->populations.get_flat_index({i, InfectionState::InfectedSymptomsNaiveConfirmed});

            size_t SPIi    = this->populations.get_flat_index({i, InfectionState::SusceptiblePartialImmunity});
            size_t EPIi    = this->populations.get_flat_index({i, InfectionState::ExposedPartialImmunity});
            size_t INSPIi  = this->populations.get_flat_index({i, InfectionState::InfectedNoSymptomsPartialImmunity});
            size_t ISyPIi  = this->populations.get_flat_index({i, InfectionState::InfectedSymptomsPartialImmunity});
            size_t ISevPIi = this->populations.get_flat_index({i, InfectionState::InfectedSeverePartialImmunity});
            size_t ICrPIi  = this->populations.get_flat_index({i, InfectionState::InfectedCriticalPartialImmunity});

            size_t INSPICi =
                this->populations.get_flat_index({i, InfectionState::InfectedNoSymptomsPartialImmunityConfirmed});
            size_t ISyPICi =
                this->populations.get_flat_index({i, InfectionState::InfectedSymptomsPartialImmunityConfirmed});

            size_t EIIi    = this->populations.get_flat_index({i, InfectionState::ExposedImprovedImmunity});
            size_t INSIIi  = this->populations.get_flat_index({i, InfectionState::InfectedNoSymptomsImprovedImmunity});
            size_t ISyIIi  = this->populations.get_flat_index({i, InfectionState::InfectedSymptomsImprovedImmunity});
            size_t ISevIIi = this->populations.get_flat_index({i, InfectionState::InfectedSevereImprovedImmunity});
            size_t ICrIIi  = this->populations.get_flat_index({i, InfectionState::InfectedCriticalImprovedImmunity});

            size_t INSIICi =
                this->populations.get_flat_index({i, InfectionState::InfectedNoSymptomsImprovedImmunityConfirmed});
            size_t ISyIICi =
                this->populations.get_flat_index({i, InfectionState::InfectedSymptomsImprovedImmunityConfirmed});
            size_t TImm1 = this->populations.get_flat_index({i, InfectionState::TemporaryImmunPartialImmunity});
            size_t TImm2 = this->populations.get_flat_index({i, InfectionState::TemporaryImmunImprovedImmunity});

            size_t SIIi = this->populations.get_flat_index({i, InfectionState::SusceptibleImprovedImmunity});

            double reducExposedPartialImmunity           = params.get<ReducExposedPartialImmunity>()[i];
            double reducExposedImprovedImmunity          = params.get<ReducExposedImprovedImmunity>()[i];
            double reducInfectedSymptomsPartialImmunity  = params.get<ReducInfectedSymptomsPartialImmunity>()[i];
            double reducInfectedSymptomsImprovedImmunity = params.get<ReducInfectedSymptomsImprovedImmunity>()[i];
            double reducInfectedSevereCriticalDeadPartialImmunity =
                params.get<ReducInfectedSevereCriticalDeadPartialImmunity>()[i];
            double reducInfectedSevereCriticalDeadImprovedImmunity =
                params.get<ReducInfectedSevereCriticalDeadImprovedImmunity>()[i];
            double reducTimeInfectedMild = params.get<ReducTimeInfectedMild>()[i];

            //symptomatic are less well quarantined when testing and tracing is overwhelmed so they infect more people
            auto riskFromInfectedSymptomatic = smoother_cosine(
                test_and_trace_required, params.get<TestAndTraceCapacity>(), params.get<TestAndTraceCapacity>() * 15,
                params.get<RiskOfInfectionFromSymptomatic>()[i], params.get<MaxRiskOfInfectionFromSymptomatic>()[i]);

            auto riskFromInfectedNoSymptoms = smoother_cosine(
                test_and_trace_required, params.get<TestAndTraceCapacity>(), params.get<TestAndTraceCapacity>() * 2,
                params.get<RelativeTransmissionNoSymptoms>()[i], 1.0);

            for (auto j = AgeGroup(0); j < n_agegroups; j++) {
                size_t SNj    = this->populations.get_flat_index({j, InfectionState::SusceptibleNaive});
                size_t ENj    = this->populations.get_flat_index({j, InfectionState::ExposedNaive});
                size_t INSNj  = this->populations.get_flat_index({j, InfectionState::InfectedNoSymptomsNaive});
                size_t ISyNj  = this->populations.get_flat_index({j, InfectionState::InfectedSymptomsNaive});
                size_t ISevNj = this->populations.get_flat_index({j, InfectionState::InfectedSevereNaive});
                size_t ICrNj  = this->populations.get_flat_index({j, InfectionState::InfectedCriticalNaive});
                size_t SIIj   = this->populations.get_flat_index({j, InfectionState::SusceptibleImprovedImmunity});

                size_t INSNCj = this->populations.get_flat_index({j, InfectionState::InfectedNoSymptomsNaiveConfirmed});
                size_t ISyNCj = this->populations.get_flat_index({j, InfectionState::InfectedSymptomsNaiveConfirmed});

                size_t SPIj = this->populations.get_flat_index({j, InfectionState::SusceptiblePartialImmunity});
                size_t EPIj = this->populations.get_flat_index({j, InfectionState::ExposedPartialImmunity});
                size_t INSPIj =
                    this->populations.get_flat_index({j, InfectionState::InfectedNoSymptomsPartialImmunity});
                size_t ISyPIj  = this->populations.get_flat_index({j, InfectionState::InfectedSymptomsPartialImmunity});
                size_t ISevPIj = this->populations.get_flat_index({j, InfectionState::InfectedSeverePartialImmunity});
                size_t ICrPIj  = this->populations.get_flat_index({j, InfectionState::InfectedCriticalPartialImmunity});

                size_t INSPICj =
                    this->populations.get_flat_index({j, InfectionState::InfectedNoSymptomsPartialImmunityConfirmed});
                size_t ISyPICj =
                    this->populations.get_flat_index({j, InfectionState::InfectedSymptomsPartialImmunityConfirmed});

                size_t EIIj = this->populations.get_flat_index({j, InfectionState::ExposedImprovedImmunity});
                size_t INSIIj =
                    this->populations.get_flat_index({j, InfectionState::InfectedNoSymptomsImprovedImmunity});
                size_t ISyIIj = this->populations.get_flat_index({j, InfectionState::InfectedSymptomsImprovedImmunity});
                size_t ISevIIj = this->populations.get_flat_index({j, InfectionState::InfectedSevereImprovedImmunity});
                size_t ICrIIj = this->populations.get_flat_index({j, InfectionState::InfectedCriticalImprovedImmunity});

                size_t INSIICj =
                    this->populations.get_flat_index({j, InfectionState::InfectedNoSymptomsImprovedImmunityConfirmed});
                size_t ISyIICj =
                    this->populations.get_flat_index({j, InfectionState::InfectedSymptomsImprovedImmunityConfirmed});

                // effective contact rate by contact rate between groups i and j and damping j
                double season_val =
                    (1 + params.get<Seasonality>() *
                             sin(3.141592653589793 * (std::fmod((params.get<StartDay>() + t), 365.0) / 182.5 + 0.5)));
                double cont_freq_eff =
                    season_val * contact_matrix.get_matrix_at(t)(static_cast<Eigen::Index>((size_t)i),
                                                                 static_cast<Eigen::Index>((size_t)j));
                // without died people
                double Nj = pop[SNj] + pop[ENj] + pop[INSNj] + pop[ISyNj] + pop[ISevNj] + pop[ICrNj] + pop[INSNCj] +
                            pop[ISyNCj] + pop[SPIj] + pop[EPIj] + pop[INSPIj] + pop[ISyPIj] + pop[ISevPIj] +
                            pop[ICrPIj] + pop[INSPICj] + pop[ISyPICj] + pop[SIIj] + pop[EIIj] + pop[INSIIj] +
                            pop[ISyIIj] + pop[ISevIIj] + pop[ICrIIj] + pop[INSIICj] + pop[ISyIICj];

                double divNj = (Nj > 0) ? 1.0 / Nj : 0;

                double ext_inf_force_dummy = cont_freq_eff * divNj *
                                             params.template get<TransmissionProbabilityOnContact>()[(AgeGroup)i] *
                                             (riskFromInfectedNoSymptoms * (pop[INSNj] + pop[INSPIj] + pop[INSIIj]) +
                                              riskFromInfectedSymptomatic * (pop[ISyNj] + pop[ISyPIj] + pop[ISyIIj]));

                double dummy_SN = y[SNi] * ext_inf_force_dummy;

                double dummy_SPI = y[SPIi] * reducExposedPartialImmunity * ext_inf_force_dummy;

                double dummy_SII = y[SIIi] * reducExposedImprovedImmunity * ext_inf_force_dummy;

                flows[get_flat_flow_index<InfectionState::SusceptibleNaive, InfectionState::ExposedNaive>({i})] +=
                    dummy_SN;
                flows[get_flat_flow_index<InfectionState::SusceptiblePartialImmunity,
                                          InfectionState::ExposedPartialImmunity>({i})] += dummy_SPI;
                flows[get_flat_flow_index<InfectionState::SusceptibleImprovedImmunity,
                                          InfectionState::ExposedImprovedImmunity>({i})] += dummy_SII;
            }

            // vaccinations
            const auto t_idx = SimulationDay((size_t)t);

            // Calculates the number of vaccinations per layer for a specific day and age group.
            // If the size of the `DailyPartialVaccination` array is smaller than the current day t then zero is returned.
            // Otherwise, we differentiate between greater and equal SimulationDay(0) since the numbers are accumulated.
            double first_vacc =
                static_cast<size_t>(params.get<DailyPartialVaccination>().size<SimulationDay>()) > (size_t)t
                    ? (t_idx > SimulationDay(0) ? params.template get<DailyPartialVaccination>()[{(AgeGroup)i, t_idx}] -
                                                      params.template get<DailyPartialVaccination>()[{
                                                          (AgeGroup)i, SimulationDay((size_t)t - 1)}]
                                                : params.template get<DailyPartialVaccination>()[{(AgeGroup)i, t_idx}])
                    : 0;

            double full_vacc =
                static_cast<size_t>(params.get<DailyFullVaccination>().size<SimulationDay>()) > (size_t)t
                    ? (t_idx > SimulationDay(0)
                           ? params.template get<DailyFullVaccination>()[{(AgeGroup)i, t_idx}] -
                                 params
                                     .template get<DailyFullVaccination>()[{(AgeGroup)i, SimulationDay((size_t)t - 1)}]
                           : params.template get<DailyFullVaccination>()[{(AgeGroup)i, t_idx}])
                    : 0;

            double booster_vacc =
                static_cast<size_t>(params.get<DailyBoosterVaccination>().size<SimulationDay>()) > (size_t)t
                    ? (t_idx > SimulationDay(0) ? params.template get<DailyBoosterVaccination>()[{(AgeGroup)i, t_idx}] -
                                                      params.template get<DailyBoosterVaccination>()[{
                                                          (AgeGroup)i, SimulationDay((size_t)t - 1)}]
                                                : params.template get<DailyBoosterVaccination>()[{(AgeGroup)i, t_idx}])
                    : 0;

            flows[get_flat_flow_index<InfectionState::SusceptibleNaive, InfectionState::TemporaryImmunPartialImmunity>(
                {i})] =
                std::min(
                    y[SNi] -
                        flows[get_flat_flow_index<InfectionState::SusceptibleNaive, InfectionState::ExposedNaive>({i})],
                    first_vacc);

            flows[get_flat_flow_index<InfectionState::SusceptiblePartialImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                std::min(y[SPIi] - flows[get_flat_flow_index<InfectionState::SusceptiblePartialImmunity,
                                                             InfectionState::ExposedPartialImmunity>({i})],
                         full_vacc);

            flows[get_flat_flow_index<InfectionState::SusceptibleImprovedImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                std::min(y[SIIi] - flows[get_flat_flow_index<InfectionState::SusceptibleImprovedImmunity,
                                                             InfectionState::ExposedImprovedImmunity>({i})],
                         booster_vacc);

            // ICU capacity shortage is close
            // TODO: if this is used with vaccination model, it has to be adapted if CriticalPerSevere
            // is different for different vaccination status. This is not the case here and in addition, ICUCapacity
            // is set to infinity and this functionality is deactivated, so this is OK for the moment.
            double criticalPerSevereAdjusted =
                smoother_cosine(icu_occupancy, 0.90 * params.get<ICUCapacity>(), params.get<ICUCapacity>(),
                                params.get<CriticalPerSevere>()[i], 0);

            double deathsPerSevereAdjusted = params.get<CriticalPerSevere>()[i] - criticalPerSevereAdjusted;

            /**** path of immune-naive ***/
            // Exposed
            flows[get_flat_flow_index<InfectionState::ExposedNaive, InfectionState::InfectedNoSymptomsNaive>({i})] +=
                y[ENi] / params.get<TimeExposed>()[i];

            // InfectedNoSymptoms
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsNaive,
                                      InfectionState::TemporaryImmunPartialImmunity>({i})] =
                params.get<RecoveredPerInfectedNoSymptoms>()[i] * (1 / params.get<TimeInfectedNoSymptoms>()[i]) *
                y[INSNi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsNaive, InfectionState::InfectedSymptomsNaive>(
                {i})] = (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i]) /
                        params.get<TimeInfectedNoSymptoms>()[i] * y[INSNi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsNaiveConfirmed,
                                      InfectionState::InfectedSymptomsNaiveConfirmed>({i})] =
                (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i]) / params.get<TimeInfectedNoSymptoms>()[i] *
                y[INSNCi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsNaiveConfirmed,
                                      InfectionState::TemporaryImmunPartialImmunity>({i})] =
                params.get<RecoveredPerInfectedNoSymptoms>()[i] * (1 / params.get<TimeInfectedNoSymptoms>()[i]) *
                y[INSNCi];

            // // InfectedSymptoms
            flows[get_flat_flow_index<InfectionState::InfectedSymptomsNaive, InfectionState::InfectedSevereNaive>(
                {i})] = params.get<SeverePerInfectedSymptoms>()[i] / params.get<TimeInfectedSymptoms>()[i] * y[ISyNi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsNaive,
                                      InfectionState::TemporaryImmunPartialImmunity>({i})] =
                (1 - params.get<SeverePerInfectedSymptoms>()[i]) / params.get<TimeInfectedSymptoms>()[i] * y[ISyNi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsNaiveConfirmed,
                                      InfectionState::InfectedSevereNaive>({i})] =
                params.get<SeverePerInfectedSymptoms>()[i] / params.get<TimeInfectedSymptoms>()[i] * y[ISyNCi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsNaiveConfirmed,
                                      InfectionState::TemporaryImmunPartialImmunity>({i})] =
                (1 - params.get<SeverePerInfectedSymptoms>()[i]) / params.get<TimeInfectedSymptoms>()[i] * y[ISyNCi];

            // InfectedSevere
            flows[get_flat_flow_index<InfectionState::InfectedSevereNaive, InfectionState::InfectedCriticalNaive>(
                {i})] = criticalPerSevereAdjusted / params.get<TimeInfectedSevere>()[i] * y[ISevNi];

            flows[get_flat_flow_index<InfectionState::InfectedSevereNaive,
                                      InfectionState::TemporaryImmunPartialImmunity>({i})] =
                (1 - params.get<CriticalPerSevere>()[i]) / params.get<TimeInfectedSevere>()[i] * y[ISevNi];

            flows[get_flat_flow_index<InfectionState::InfectedSevereNaive, InfectionState::DeadNaive>({i})] =
                deathsPerSevereAdjusted / params.get<TimeInfectedSevere>()[i] * y[ISevNi];
            // InfectedCritical
            flows[get_flat_flow_index<InfectionState::InfectedCriticalNaive, InfectionState::DeadNaive>({i})] =
                params.get<DeathsPerCritical>()[i] / params.get<TimeInfectedCritical>()[i] * y[ICrNi];

            flows[get_flat_flow_index<InfectionState::InfectedCriticalNaive,
                                      InfectionState::TemporaryImmunPartialImmunity>({i})] =
                (1 - params.get<DeathsPerCritical>()[i]) / params.get<TimeInfectedCritical>()[i] * y[ICrNi];

            // Waning immunity
            flows[get_flat_flow_index<InfectionState::SusceptiblePartialImmunity, InfectionState::SusceptibleNaive>(
                {i})] = 1 / params.get<TimeWaningPartialImmunity>()[i] * y[SPIi];
            flows[get_flat_flow_index<InfectionState::TemporaryImmunPartialImmunity,
                                      InfectionState::SusceptiblePartialImmunity>({i})] =
                1 / params.get<TimeTemporaryImmunityPI>()[i] * y[TImm1];

            // /**** path of partially immune ***/

            // Exposed
            flows[get_flat_flow_index<InfectionState::ExposedPartialImmunity,
                                      InfectionState::InfectedNoSymptomsPartialImmunity>({i})] +=
                y[EPIi] / params.get<TimeExposed>()[i];

            // InfectedNoSymptoms
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsPartialImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSymptomsPartialImmunity / reducExposedPartialImmunity) *
                         (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i])) /
                (params.get<TimeInfectedNoSymptoms>()[i] * reducTimeInfectedMild) * y[INSPIi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsPartialImmunity,
                                      InfectionState::InfectedSymptomsPartialImmunity>({i})] =
                (reducInfectedSymptomsPartialImmunity / reducExposedPartialImmunity) *
                (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i]) /
                (params.get<TimeInfectedNoSymptoms>()[i] * reducTimeInfectedMild) * y[INSPIi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsPartialImmunityConfirmed,
                                      InfectionState::InfectedSymptomsPartialImmunityConfirmed>({i})] =
                (reducInfectedSymptomsPartialImmunity / reducExposedPartialImmunity) *
                (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i]) /
                (params.get<TimeInfectedNoSymptoms>()[i] * reducTimeInfectedMild) * y[INSPICi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsPartialImmunityConfirmed,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSymptomsPartialImmunity / reducExposedPartialImmunity) *
                         (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i])) /
                (params.get<TimeInfectedNoSymptoms>()[i] * reducTimeInfectedMild) * y[INSPICi];

            // InfectedSymptoms
            flows[get_flat_flow_index<InfectionState::InfectedSymptomsPartialImmunity,
                                      InfectionState::InfectedSeverePartialImmunity>({i})] =
                reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSymptomsPartialImmunity *
                params.get<SeverePerInfectedSymptoms>()[i] /
                (params.get<TimeInfectedSymptoms>()[i] * reducTimeInfectedMild) * y[ISyPIi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsPartialImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSymptomsPartialImmunity) *
                         params.get<SeverePerInfectedSymptoms>()[i]) /
                (params.get<TimeInfectedSymptoms>()[i] * reducTimeInfectedMild) * y[ISyPIi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsPartialImmunityConfirmed,
                                      InfectionState::InfectedSeverePartialImmunity>({i})] =
                reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSymptomsPartialImmunity *
                params.get<SeverePerInfectedSymptoms>()[i] /
                (params.get<TimeInfectedSymptoms>()[i] * reducTimeInfectedMild) * y[ISyPICi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsPartialImmunityConfirmed,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSymptomsPartialImmunity) *
                         params.get<SeverePerInfectedSymptoms>()[i]) /
                (params.get<TimeInfectedSymptoms>()[i] * reducTimeInfectedMild) * y[ISyPICi];

            // InfectedSevere
            flows[get_flat_flow_index<InfectionState::InfectedSeverePartialImmunity,
                                      InfectionState::InfectedCriticalPartialImmunity>({i})] =
                reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSevereCriticalDeadPartialImmunity *
                criticalPerSevereAdjusted / params.get<TimeInfectedSevere>()[i] * y[ISevPIi];

            flows[get_flat_flow_index<InfectionState::InfectedSeverePartialImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSevereCriticalDeadPartialImmunity) *
                         params.get<CriticalPerSevere>()[i]) /
                params.get<TimeInfectedSevere>()[i] * y[ISevPIi];

            flows[get_flat_flow_index<InfectionState::InfectedSeverePartialImmunity,
                                      InfectionState::DeadPartialImmunity>({i})] =
                (reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSevereCriticalDeadPartialImmunity) *
                deathsPerSevereAdjusted / params.get<TimeInfectedSevere>()[i] * y[ISevPIi];
            // InfectedCritical
            flows[get_flat_flow_index<InfectionState::InfectedCriticalPartialImmunity,
                                      InfectionState::DeadPartialImmunity>({i})] =
                (reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSevereCriticalDeadPartialImmunity) *
                params.get<DeathsPerCritical>()[i] / params.get<TimeInfectedCritical>()[i] * y[ICrPIi];

            flows[get_flat_flow_index<InfectionState::InfectedCriticalPartialImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSevereCriticalDeadPartialImmunity / reducInfectedSevereCriticalDeadPartialImmunity) *
                         params.get<DeathsPerCritical>()[i]) /
                params.get<TimeInfectedCritical>()[i] * y[ICrPIi];

            // /**** path of improved immunity ***/
            // Exposed
            flows[get_flat_flow_index<InfectionState::ExposedImprovedImmunity,
                                      InfectionState::InfectedNoSymptomsImprovedImmunity>({i})] +=
                y[EIIi] / params.get<TimeExposed>()[i];

            // InfectedNoSymptoms
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsImprovedImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSymptomsImprovedImmunity / reducExposedImprovedImmunity) *
                         (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i])) /
                (params.get<TimeInfectedNoSymptoms>()[i] * reducTimeInfectedMild) * y[INSIIi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsImprovedImmunity,
                                      InfectionState::InfectedSymptomsImprovedImmunity>({i})] =
                (reducInfectedSymptomsImprovedImmunity / reducExposedImprovedImmunity) *
                (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i]) /
                (params.get<TimeInfectedNoSymptoms>()[i] * reducTimeInfectedMild) * y[INSIIi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsImprovedImmunityConfirmed,
                                      InfectionState::InfectedSymptomsImprovedImmunityConfirmed>({i})] =
                (reducInfectedSymptomsImprovedImmunity / reducExposedImprovedImmunity) *
                (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i]) /
                (params.get<TimeInfectedNoSymptoms>()[i] * reducTimeInfectedMild) * y[INSIICi];
            flows[get_flat_flow_index<InfectionState::InfectedNoSymptomsImprovedImmunityConfirmed,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSymptomsImprovedImmunity / reducExposedImprovedImmunity) *
                         (1 - params.get<RecoveredPerInfectedNoSymptoms>()[i])) /
                (params.get<TimeInfectedNoSymptoms>()[i] * reducTimeInfectedMild) * y[INSIICi];

            // InfectedSymptoms
            flows[get_flat_flow_index<InfectionState::InfectedSymptomsImprovedImmunity,
                                      InfectionState::InfectedSevereImprovedImmunity>({i})] =
                reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSymptomsImprovedImmunity *
                params.get<SeverePerInfectedSymptoms>()[i] /
                (params.get<TimeInfectedSymptoms>()[i] * reducTimeInfectedMild) * y[ISyIIi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsImprovedImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSymptomsImprovedImmunity) *
                         params.get<SeverePerInfectedSymptoms>()[i]) /
                (params.get<TimeInfectedSymptoms>()[i] * reducTimeInfectedMild) * y[ISyIIi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsImprovedImmunityConfirmed,
                                      InfectionState::InfectedSevereImprovedImmunity>({i})] =
                reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSymptomsImprovedImmunity *
                params.get<SeverePerInfectedSymptoms>()[i] /
                (params.get<TimeInfectedSymptoms>()[i] * reducTimeInfectedMild) * y[ISyIICi];

            flows[get_flat_flow_index<InfectionState::InfectedSymptomsImprovedImmunityConfirmed,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 - (reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSymptomsImprovedImmunity) *
                         params.get<SeverePerInfectedSymptoms>()[i]) /
                (params.get<TimeInfectedSymptoms>()[i] * reducTimeInfectedMild) * y[ISyIICi];

            // InfectedSevere
            flows[get_flat_flow_index<InfectionState::InfectedSevereImprovedImmunity,
                                      InfectionState::InfectedCriticalImprovedImmunity>({i})] =
                reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSevereCriticalDeadImprovedImmunity *
                criticalPerSevereAdjusted / params.get<TimeInfectedSevere>()[i] * y[ISevIIi];

            flows[get_flat_flow_index<InfectionState::InfectedSevereImprovedImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 -
                 (reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSevereCriticalDeadImprovedImmunity) *
                     params.get<CriticalPerSevere>()[i]) /
                params.get<TimeInfectedSevere>()[i] * y[ISevIIi];

            flows[get_flat_flow_index<InfectionState::InfectedSevereImprovedImmunity,
                                      InfectionState::DeadImprovedImmunity>({i})] =
                (reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSevereCriticalDeadImprovedImmunity) *
                deathsPerSevereAdjusted / params.get<TimeInfectedSevere>()[i] * y[ISevIIi];

            // InfectedCritical
            flows[get_flat_flow_index<InfectionState::InfectedCriticalImprovedImmunity,
                                      InfectionState::DeadImprovedImmunity>({i})] =
                (reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSevereCriticalDeadImprovedImmunity) *
                params.get<DeathsPerCritical>()[i] / params.get<TimeInfectedCritical>()[i] * y[ICrIIi];

            flows[get_flat_flow_index<InfectionState::InfectedCriticalImprovedImmunity,
                                      InfectionState::TemporaryImmunImprovedImmunity>({i})] =
                (1 -
                 (reducInfectedSevereCriticalDeadImprovedImmunity / reducInfectedSevereCriticalDeadImprovedImmunity) *
                     params.get<DeathsPerCritical>()[i]) /
                params.get<TimeInfectedCritical>()[i] * y[ICrIIi];

            // Waning immunity
            flows[get_flat_flow_index<InfectionState::TemporaryImmunImprovedImmunity,
                                      InfectionState::SusceptibleImprovedImmunity>({i})] =
                1 / params.get<TimeTemporaryImmunityII>()[i] * y[TImm2];

            flows[get_flat_flow_index<InfectionState::SusceptibleImprovedImmunity,
                                      InfectionState::SusceptiblePartialImmunity>({i})] =
                1 / params.get<TimeWaningImprovedImmunity>()[i] * y[SIIi];
        }
    }

    Eigen::VectorXd vaccinations_at(const CustomIndexArray<double, AgeGroup, SimulationDay>& daily_vaccinations,
                                    const ScalarType t, const ScalarType eps = 0.15) const
    {
        auto const& params  = this->parameters;
        const ScalarType ub = (size_t)t + 1.0;
        const ScalarType lb = ub - eps;

        Eigen::VectorXd smoothed_vaccinations((size_t)params.get_num_groups());

        if (t > ub) {
            mio::log_warning("Vaccination time is out of bounds");
        }
        // check if t is in the range of the interval [lb,ub]
        if (t >= lb) {
            // need a eigen vector of size params.get_num_groups() to store the number of vaccinations per age group

            // ToDo: Find a way to Iterate over all three vaccination types
            for (AgeGroup age = AgeGroup(0); age < params.get_num_groups(); age++) {
                const auto num_vaccinations = daily_vaccinations[{age, SimulationDay((size_t)t + 1)}] -
                                              daily_vaccinations[{age, SimulationDay((size_t)t)}];
                const auto num_vaccinations_eps = daily_vaccinations[{age, SimulationDay((size_t)t - eps + 1)}] -
                                                  daily_vaccinations[{age, SimulationDay((size_t)t - eps)}];
                smoothed_vaccinations[(size_t)age] = smoother_cosine(t, lb, ub, num_vaccinations_eps, num_vaccinations);
            }
        }
        else {
            for (auto age = AgeGroup(0); age < params.get_num_groups(); age++) {
                smoothed_vaccinations[(size_t)age] = daily_vaccinations[{age, SimulationDay((size_t)t + 1)}] -
                                                     daily_vaccinations[{age, SimulationDay((size_t)t)}];
            }
        }
        return smoothed_vaccinations;
    }

    /**
    * serialize this. 
    * @see mio::serialize
    */
    template <class IOContext>
    void serialize(IOContext& io) const
    {
        auto obj = io.create_object("Model");
        obj.add_element("Parameters", parameters);
        obj.add_element("Populations", populations);
    }

    /**
    * deserialize an object of this class.
    * @see mio::deserialize
    */
    template <class IOContext>
    static IOResult<Model> deserialize(IOContext& io)
    {
        auto obj = io.expect_object("Model");
        auto par = obj.expect_element("Parameters", Tag<ParameterSet>{});
        auto pop = obj.expect_element("Populations", Tag<Populations>{});
        return apply(
            io,
            [](auto&& par_, auto&& pop_) {
                return Model{pop_, par_};
            },
            par, pop);
    }
}; // namespace osecirvvs

//forward declaration, see below.
template <class BaseT = mio::Simulation<Model>>
class Simulation;

/**
* get percentage of infections per total population.
* @param model the compartment model with initial values.
* @param t current simulation time.
* @param y current value of compartments.
* @tparam Base simulation type that uses the SECIRS-type compartment model. see Simulation.
*/
template <class Base = mio::Simulation<Model>>
double get_infections_relative(const Simulation<Base>& model, double t, const Eigen::Ref<const Eigen::VectorXd>& y);

/**
 * specialization of compartment model simulation for the SECIRS-type model.
 * @tparam BaseT simulation type, default mio::Simulation. For testing purposes only!
 */
template <class BaseT>
class Simulation : public BaseT
{
public:
    /**
     * construct a simulation.
     * @param model the model to simulate.
     * @param t0 start time
     * @param dt time steps
     */
    Simulation(Model const& model, double t0 = 0., double dt = 0.1)
        : BaseT(model, t0, dt)
        , m_t_last_npi_check(t0)
    {
    }

    /**
    * @brief Applies the effect of a new variant of a disease to the transmission probability of the model.
    * 
    * This function adjusts the transmission probability of the disease for each age group based on the share of the new variant.
    * The share of the new variant is calculated based on the time `t` and the start day of the new variant.
    * The transmission probability is then updated for each age group in the model.
    * 
    * Based on Equation (35) and (36) in doi.org/10.1371/journal.pcbi.1010054
    * 
    * @param [in] t The current time.
    * @param [in] base_infectiousness The base infectiousness of the old variant for each age group.
    */

    void apply_variant(const double t, const CustomIndexArray<UncertainValue, AgeGroup> base_infectiousness)
    {
        auto start_day             = this->get_model().parameters.template get<StartDay>();
        auto start_day_new_variant = this->get_model().parameters.template get<StartDayNewVariant>();

        if (start_day + t >= start_day_new_variant - 1e-10) {
            const double days_variant      = t - (start_day_new_variant - start_day);
            const double share_new_variant = std::min(1.0, 0.01 * pow(2, (1. / 7) * days_variant));
            const auto num_groups          = this->get_model().parameters.get_num_groups();
            for (auto i = AgeGroup(0); i < num_groups; ++i) {
                double new_transmission = (1 - share_new_variant) * base_infectiousness[i] +
                                          share_new_variant * base_infectiousness[i] *
                                              this->get_model().parameters.template get<InfectiousnessNewVariant>()[i];
                this->get_model().parameters.template get<TransmissionProbabilityOnContact>()[i] = new_transmission;
            }
        }
    }

    /**
     * @brief advance simulation to tmax.
     * Overwrites Simulation::advance and includes a check for dynamic NPIs in regular intervals.
     * @see Simulation::advance
     * @param tmax next stopping point of simulation
     * @return value at tmax
     */
    Eigen::Ref<Eigen::VectorXd> advance(double tmax)
    {
        auto& t_end_dyn_npis   = this->get_model().parameters.get_end_dynamic_npis();
        auto& dyn_npis         = this->get_model().parameters.template get<DynamicNPIsInfectedSymptoms>();
        auto& contact_patterns = this->get_model().parameters.template get<ContactPatterns>();
        // const size_t num_groups = (size_t)this->get_model().parameters.get_num_groups();

        // in the apply_variant function, we adjust the TransmissionProbabilityOnContact parameter. We need to store
        // the base value to use it in the apply_variant function and also to reset the parameter after the simulation.
        auto base_infectiousness = this->get_model().parameters.template get<TransmissionProbabilityOnContact>();

        double delay_lockdown;
        auto t        = BaseT::get_result().get_last_time();
        const auto dt = dyn_npis.get_interval().get();
        while (t < tmax) {

            auto dt_eff = std::min({dt, tmax - t, m_t_last_npi_check + dt - t});
            if (dt_eff >= 1.0) {
                dt_eff = 1.0;
            }

            BaseT::advance(t + dt_eff);
            if (t + 0.5 + dt_eff - std::floor(t + 0.5) >= 1) {
                this->apply_variant(t, base_infectiousness);
            }

            if (t > 0) {
                delay_lockdown = 7;
            }
            else {
                delay_lockdown = 0;
            }
            t = t + dt_eff;

            if (dyn_npis.get_thresholds().size() > 0) {
                if (floating_point_greater_equal(t, m_t_last_npi_check + dt)) {
                    if (t < t_end_dyn_npis) {
                        auto inf_rel = get_infections_relative(*this, t, this->get_result().get_last_value()) *
                                       dyn_npis.get_base_value();
                        auto exceeded_threshold = dyn_npis.get_max_exceeded_threshold(inf_rel);
                        if (exceeded_threshold != dyn_npis.get_thresholds().end() &&
                            (exceeded_threshold->first > m_dynamic_npi.first ||
                             t > double(m_dynamic_npi.second))) { //old npi was weaker or is expired

                            auto t_start = SimulationTime(t + delay_lockdown);
                            auto t_end   = t_start + SimulationTime(dyn_npis.get_duration());
                            this->get_model().parameters.get_start_commuter_detection() = (double)t_start;
                            this->get_model().parameters.get_end_commuter_detection()   = (double)t_end;
                            m_dynamic_npi = std::make_pair(exceeded_threshold->first, t_end);
                            implement_dynamic_npis(contact_patterns.get_cont_freq_mat(), exceeded_threshold->second,
                                                   t_start, t_end, [](auto& g) {
                                                       return make_contact_damping_matrix(g);
                                                   });
                        }
                    }
                    m_t_last_npi_check = t;
                }
            }
            else {
                m_t_last_npi_check = t;
            }
        }
        // reset TransmissionProbabilityOnContact. This is important for the graph simulation where the advance
        // function is called multiple times for the same model.
        this->get_model().parameters.template get<TransmissionProbabilityOnContact>() = base_infectiousness;

        return this->get_result().get_last_value();
    }

private:
    double m_t_last_npi_check;
    std::pair<double, SimulationTime> m_dynamic_npi = {-std::numeric_limits<double>::max(), SimulationTime(0)};
};

/**
 * @brief Specialization of simulate for SECIRS-type models using Simulation.
 * 
 * @param[in] t0 start time.
 * @param[in] tmax end time.
 * @param[in] dt time step.
 * @param[in] model SECIRS-type model to simulate.
 * @param[in] integrator optional integrator, uses rk45 if nullptr.
 * 
 * @return Returns the result of the simulation.
 */
inline auto simulate(double t0, double tmax, double dt, const Model& model,
                     std::shared_ptr<IntegratorCore> integrator = nullptr)
{
    return mio::simulate<Model, Simulation<>>(t0, tmax, dt, model, integrator);
}

/**
 * @brief Specialization of simulate for SECIRS-type models using the FlowSimulation.
 * 
 * @param[in] t0 start time.
 * @param[in] tmax end time.
 * @param[in] dt time step.
 * @param[in] model SECIRS-type model to simulate.
 * @param[in] integrator optional integrator, uses rk45 if nullptr.
 * 
 * @return Returns the result of the Flowsimulation.
  */
inline auto simulate_flows(double t0, double tmax, double dt, const Model& model,
                           std::shared_ptr<IntegratorCore> integrator = nullptr)
{
    return mio::simulate_flows<Model, Simulation<mio::FlowSimulation<Model>>>(t0, tmax, dt, model, integrator);
}

//see declaration above.
template <class Base>
double get_infections_relative(const Simulation<Base>& sim, double /*t*/, const Eigen::Ref<const Eigen::VectorXd>& y)
{
    double sum_inf = 0;
    for (auto i = AgeGroup(0); i < sim.get_model().parameters.get_num_groups(); ++i) {
        sum_inf += sim.get_model().populations.get_from(y, {i, InfectionState::InfectedSymptomsNaive});
        sum_inf += sim.get_model().populations.get_from(y, {i, InfectionState::InfectedSymptomsNaiveConfirmed});
        sum_inf += sim.get_model().populations.get_from(y, {i, InfectionState::InfectedSymptomsPartialImmunity});
        sum_inf += sim.get_model().populations.get_from(y, {i, InfectionState::InfectedSymptomsImprovedImmunity});
        sum_inf +=
            sim.get_model().populations.get_from(y, {i, InfectionState::InfectedSymptomsPartialImmunityConfirmed});
        sum_inf +=
            sim.get_model().populations.get_from(y, {i, InfectionState::InfectedSymptomsImprovedImmunityConfirmed});
    }
    auto inf_rel = sum_inf / sim.get_model().populations.get_total();

    return inf_rel;
}

/**
 * Get migration factors.
 * Used by migration graph simulation.
 * Like infection risk, migration of infected individuals is reduced if they are well isolated.
 * @param model the compartment model with initial values.
 * @param t current simulation time.
 * @param y current value of compartments.
 * @return vector expression, same size as y, with migration factors per compartment.
 * @tparam Base simulation type that uses a SECIRS-type compartment model. see Simulation.
 */
template <class Base = mio::Simulation<Model>>
auto get_migration_factors(const Simulation<Base>& sim, double /*t*/, const Eigen::Ref<const Eigen::VectorXd>& y)
{
    auto& params = sim.get_model().parameters;
    //parameters as arrays
    auto& p_asymp   = params.template get<RecoveredPerInfectedNoSymptoms>().array().template cast<double>();
    auto& p_inf     = params.template get<RiskOfInfectionFromSymptomatic>().array().template cast<double>();
    auto& p_inf_max = params.template get<MaxRiskOfInfectionFromSymptomatic>().array().template cast<double>();
    //slice of InfectedNoSymptoms
    auto y_INS = slice(y, {Eigen::Index(InfectionState::InfectedNoSymptomsNaive),
                           Eigen::Index(size_t(params.get_num_groups())), Eigen::Index(InfectionState::Count)}) +
                 slice(y, {Eigen::Index(InfectionState::InfectedNoSymptomsPartialImmunity),
                           Eigen::Index(size_t(params.get_num_groups())), Eigen::Index(InfectionState::Count)}) +
                 slice(y, {Eigen::Index(InfectionState::InfectedNoSymptomsImprovedImmunity),
                           Eigen::Index(size_t(params.get_num_groups())), Eigen::Index(InfectionState::Count)});

    //compute isolation, same as infection risk from main model
    auto test_and_trace_required =
        ((1 - p_asymp) / params.template get<TimeInfectedNoSymptoms>().array().template cast<double>() * y_INS.array())
            .sum();
    auto riskFromInfectedSymptomatic =
        smoother_cosine(test_and_trace_required, double(params.template get<TestAndTraceCapacity>()),
                        params.template get<TestAndTraceCapacity>() * 5, p_inf.matrix(), p_inf_max.matrix());

    //set factor for infected
    auto factors = Eigen::VectorXd::Ones(y.rows()).eval();
    slice(factors, {Eigen::Index(InfectionState::InfectedSymptomsNaive), Eigen::Index(size_t(params.get_num_groups())),
                    Eigen::Index(InfectionState::Count)})
        .array() = riskFromInfectedSymptomatic;
    slice(factors, {Eigen::Index(InfectionState::InfectedSymptomsPartialImmunity),
                    Eigen::Index(size_t(params.get_num_groups())), Eigen::Index(InfectionState::Count)})
        .array() = riskFromInfectedSymptomatic;
    slice(factors, {Eigen::Index(InfectionState::InfectedSymptomsImprovedImmunity),
                    Eigen::Index(size_t(params.get_num_groups())), Eigen::Index(InfectionState::Count)})
        .array() = riskFromInfectedSymptomatic;
    return factors;
}

template <class Base = mio::Simulation<Model>>
auto test_commuters(Simulation<Base>& sim, Eigen::Ref<Eigen::VectorXd> migrated, double time)
{
    auto& model       = sim.get_model();
    auto nondetection = 1.0;
    if (time >= model.parameters.get_start_commuter_detection() &&
        time < model.parameters.get_end_commuter_detection()) {
        nondetection = (double)model.parameters.get_commuter_nondetection();
    }
    for (auto i = AgeGroup(0); i < model.parameters.get_num_groups(); ++i) {
        auto ISyNi  = model.populations.get_flat_index({i, InfectionState::InfectedSymptomsNaive});
        auto ISyNCi = model.populations.get_flat_index({i, InfectionState::InfectedSymptomsNaiveConfirmed});
        auto INSNi  = model.populations.get_flat_index({i, InfectionState::InfectedNoSymptomsNaive});
        auto INSNCi = model.populations.get_flat_index({i, InfectionState::InfectedNoSymptomsNaiveConfirmed});

        auto ISPIi  = model.populations.get_flat_index({i, InfectionState::InfectedSymptomsPartialImmunity});
        auto ISPICi = model.populations.get_flat_index({i, InfectionState::InfectedSymptomsPartialImmunityConfirmed});
        auto INSPIi = model.populations.get_flat_index({i, InfectionState::InfectedNoSymptomsPartialImmunity});
        auto INSPICi =
            model.populations.get_flat_index({i, InfectionState::InfectedNoSymptomsPartialImmunityConfirmed});

        auto ISyIIi  = model.populations.get_flat_index({i, InfectionState::InfectedSymptomsImprovedImmunity});
        auto ISyIICi = model.populations.get_flat_index({i, InfectionState::InfectedSymptomsImprovedImmunityConfirmed});
        auto INSIIi  = model.populations.get_flat_index({i, InfectionState::InfectedNoSymptomsImprovedImmunity});
        auto INSIICi =
            model.populations.get_flat_index({i, InfectionState::InfectedNoSymptomsImprovedImmunityConfirmed});

        //put detected commuters in their own compartment so they don't contribute to infections in their home node
        sim.get_result().get_last_value()[ISyNi] -= migrated[ISyNi] * (1 - nondetection);
        sim.get_result().get_last_value()[ISyNCi] += migrated[ISyNi] * (1 - nondetection);
        sim.get_result().get_last_value()[INSNi] -= migrated[INSNi] * (1 - nondetection);
        sim.get_result().get_last_value()[INSNCi] += migrated[INSNi] * (1 - nondetection);

        sim.get_result().get_last_value()[ISPIi] -= migrated[ISPIi] * (1 - nondetection);
        sim.get_result().get_last_value()[ISPICi] += migrated[ISPIi] * (1 - nondetection);
        sim.get_result().get_last_value()[INSPIi] -= migrated[INSPIi] * (1 - nondetection);
        sim.get_result().get_last_value()[INSPICi] += migrated[INSPIi] * (1 - nondetection);

        sim.get_result().get_last_value()[ISyIIi] -= migrated[ISyIIi] * (1 - nondetection);
        sim.get_result().get_last_value()[ISyIICi] += migrated[ISyIIi] * (1 - nondetection);
        sim.get_result().get_last_value()[INSIIi] -= migrated[INSIIi] * (1 - nondetection);
        sim.get_result().get_last_value()[INSIICi] += migrated[INSIIi] * (1 - nondetection);

        //reduce the number of commuters
        migrated[ISyNi] *= nondetection;
        migrated[INSNi] *= nondetection;

        migrated[ISPIi] *= nondetection;
        migrated[INSPIi] *= nondetection;

        migrated[ISyIIi] *= nondetection;
        migrated[INSIIi] *= nondetection;
    }
}

} // namespace osecirvvs
} // namespace mio

#endif //ODESECIRVVS_MODEL_H

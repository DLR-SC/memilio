
#ifndef ODESEIRMOBILITY_MODEL_H
#define ODESEIRMOBILITY_MODEL_H

#include "memilio/compartments/flow_model.h"
#include "memilio/epidemiology/populations.h"
#include "models/ode_seir_mobility/infection_state.h"
#include "models/ode_seir_mobility/parameters.h"
#include "models/ode_seir_mobility/regions.h"
#include "memilio/epidemiology/age_group.h"

namespace mio
{
namespace oseirmobility
{

/********************
    * define the model *
    ********************/

using Flows = TypeList<Flow<InfectionState::Susceptible, InfectionState::Exposed>,
                       Flow<InfectionState::Exposed, InfectionState::Infected>,
                       Flow<InfectionState::Infected, InfectionState::Recovered>>;

template <typename FP = ScalarType>
class Model : public FlowModel<FP, InfectionState, mio::Populations<FP, Region, AgeGroup, InfectionState>,
                               Parameters<FP>, Flows>
{

    using Base =
        FlowModel<FP, InfectionState, mio::Populations<FP, Region, AgeGroup, InfectionState>, Parameters<FP>, Flows>;

public:
    using typename Base::ParameterSet;
    using typename Base::Populations;

    Model(int num_regions, int num_agegroups)
        : Base(Populations({Region(num_regions), AgeGroup(num_agegroups), InfectionState::Count}),
               ParameterSet(Region(num_regions), AgeGroup(num_agegroups)))
    {
    }
    // Einmal über den Vektor und später nochmal über die Regions

    void get_flows(Eigen::Ref<const Vector<FP>> pop, Eigen::Ref<const Vector<FP>> y, FP t,
                   Eigen::Ref<Vector<FP>> flows) const override
    {
        const auto& params     = this->parameters;
        const auto& population = this->populations;
        const auto& commuting_strengths =
            params.template get<CommutingStrengths<>>().get_cont_freq_mat().get_matrix_at(t);
        const Index<AgeGroup> n_age_groups = reduce_index<Index<AgeGroup>>(params.get_num_agegroups());
        const Index<Region> n_regions      = reduce_index<Index<Region>>(params.get_num_regions());

        for (auto age_i : make_index_range(n_age_groups)) {
            for (auto age_j : make_index_range(n_age_groups)) {
                for (auto region_n : make_index_range(n_regions)) {
                    FP flow_SE_helper = 0;
                    const size_t Sj   = population.get_flat_index({region_n, age_j, InfectionState::Susceptible});
                    const size_t Ej   = population.get_flat_index({region_n, age_j, InfectionState::Exposed});
                    const size_t Ij   = population.get_flat_index({region_n, age_j, InfectionState::Infected});
                    const size_t Rj   = population.get_flat_index({region_n, age_j, InfectionState::Recovered});

                    const double Nj_inv = 1.0 / (pop[Sj] + pop[Ej] + pop[Ij] + pop[Rj]);

                    double coeffStoI = params.template get<ContactPatterns<FP>>().get_cont_freq_mat().get_matrix_at(t)(
                                           age_i.get(), age_j.get()) *
                                       params.template get<TransmissionProbabilityOnContact<FP>>()[age_i] * Nj_inv;

                    for (auto region_m : make_index_range(n_regions)) {
                        if (region_n == region_m) {
                            flow_SE_helper +=
                                pop[population.get_flat_index({region_n, age_j, InfectionState::Infected})];
                            continue;
                        }
                        flow_SE_helper += (commuting_strengths(region_n.get(), region_m.get()) +
                                           commuting_strengths(region_m.get(), region_n.get())) *
                                          pop[population.get_flat_index({region_m, age_j, InfectionState::Infected})];
                    }
                    flows[Base::template get_flat_flow_index<InfectionState::Susceptible, InfectionState::Exposed>(
                        {region_n, age_i})] +=
                        flow_SE_helper * coeffStoI *
                        y[population.get_flat_index({region_n, age_j, InfectionState::Susceptible})];
                }
            }
            for (auto region : make_index_range(n_regions)) {
                flows[Base::template get_flat_flow_index<InfectionState::Exposed, InfectionState::Infected>(
                    {region, age_i})] = (1.0 / params.template get<TimeExposed<FP>>()[age_i]) *
                                        y[population.get_flat_index({region, age_i, InfectionState::Exposed})];
                flows[Base::template get_flat_flow_index<InfectionState::Infected, InfectionState::Recovered>(
                    {region, age_i})] = (1.0 / params.template get<TimeInfected<FP>>()[age_i]) *
                                        y[population.get_flat_index({region, age_i, InfectionState::Infected})];
            }
        }
    }
};

} // namespace oseirmobility
} // namespace mio

#endif // ODESEIRMOBILITY_MODEL_H


#ifndef ODESEIRMOBILITYIMPROVED_MODEL_H
#define ODESEIRMOBILITYIMPROVED_MODEL_H

#include "memilio/compartments/flow_model.h"
#include "memilio/epidemiology/populations.h"
#include "models/ode_seir_mobility_improved/infection_state.h"
#include "models/ode_seir_mobility_improved/parameters.h"
#include "models/ode_seir_mobility_improved/regions.h"
#include "memilio/epidemiology/age_group.h"
#include "memilio/utils/time_series.h"

GCC_CLANG_DIAGNOSTIC(push)
GCC_CLANG_DIAGNOSTIC(ignored "-Wshadow")
#include <Eigen/Dense>
GCC_CLANG_DIAGNOSTIC(pop)

namespace mio
{
namespace oseirmobilityimproved
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
        , m_population_after_commuting(
              mio::Populations<FP, Region, AgeGroup>({Region(num_regions), AgeGroup(num_agegroups)}))
    {
    }

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
                Eigen::VectorXd infectives_per_region = Eigen::VectorXd::Zero((size_t)n_regions);
                for (auto region_n : make_index_range(n_regions)) {
                    for (auto region_m : make_index_range(n_regions)) {
                        infectives_per_region(region_n.get()) +=
                            commuting_strengths(region_m.get(), region_n.get()) *
                            pop[population.get_flat_index({region_m, age_j, InfectionState::Infected})];
                    }
                }
                for (auto region_n : make_index_range(n_regions)) {
                    FP flow_SE_helper = 0;
                    const size_t Sj   = population.get_flat_index({region_n, age_j, InfectionState::Susceptible});
                    const size_t Ej   = population.get_flat_index({region_n, age_j, InfectionState::Exposed});
                    const size_t Ij   = population.get_flat_index({region_n, age_j, InfectionState::Infected});
                    const size_t Rj   = population.get_flat_index({region_n, age_j, InfectionState::Recovered});

                    const double Nj_inv = 1.0 / (pop[Sj] + pop[Ej] + pop[Ij] + pop[Rj]);
                    double coeffStoI    = 0.5 *
                                       params.template get<ContactPatterns<FP>>().get_cont_freq_mat().get_matrix_at(t)(
                                           age_i.get(), age_j.get()) *
                                       params.template get<TransmissionProbabilityOnContact<FP>>()[age_i];

                    flow_SE_helper +=
                        pop[population.get_flat_index({region_n, age_j, InfectionState::Infected})] * Nj_inv;
                    for (auto region_m : make_index_range(n_regions)) {
                        flow_SE_helper += commuting_strengths(region_n.get(), region_m.get()) *
                                          infectives_per_region(region_m.get()) /
                                          m_population_after_commuting[{region_n, age_j}];
                    }
                    flows[Base::template get_flat_flow_index<InfectionState::Susceptible, InfectionState::Exposed>(
                        {region_n, age_i})] +=
                        flow_SE_helper * coeffStoI *
                        y[population.get_flat_index({region_n, age_i, InfectionState::Susceptible})];
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

    /**
    *@brief Computes the reproduction number at a given index time of the Model output obtained by the Simulation.
    *@param t_idx The index time at which the reproduction number is computed.
    *@param y The TimeSeries obtained from the Model Simulation.
    *@returns The computed reproduction number at the provided index time.
    */
    IOResult<ScalarType> get_reproduction_number(size_t t_idx, const mio::TimeSeries<ScalarType>& y)
    {
        if (!(t_idx < static_cast<size_t>(y.get_num_time_points()))) {
            return mio::failure(mio::StatusCode::OutOfRange, "t_idx is not a valid index for the TimeSeries");
        }

        auto const& params = this->parameters;
        auto const& pop    = this->populations;

        const size_t num_age_groups                = (size_t)params.get_num_agegroups();
        const size_t num_regions                   = (size_t)params.get_num_regions();
        constexpr size_t num_infected_compartments = 2;
        const size_t total_infected_compartments   = num_infected_compartments * num_age_groups * num_regions;

        ContactMatrixGroup const& contact_matrix      = params.template get<ContactPatterns<ScalarType>>();
        ContactMatrixGroup const& commuting_strengths = params.template get<CommutingStrengths<ScalarType>>();

        Eigen::MatrixXd F = Eigen::MatrixXd::Zero(total_infected_compartments, total_infected_compartments);
        Eigen::MatrixXd V = Eigen::MatrixXd::Zero(total_infected_compartments, total_infected_compartments);

        for (auto i = AgeGroup(0); i < AgeGroup(num_age_groups); i++) {
            for (auto n = Region(0); n < Region(num_regions); n++) {
                size_t Si = pop.get_flat_index({n, i, InfectionState::Susceptible});
                for (auto j = AgeGroup(0); j < AgeGroup(num_age_groups); j++) {
                    for (auto m = Region(0); m < Region(num_regions); m++) {
                        auto const population_region     = pop.template slice<Region>({m.get(), 1});
                        auto const population_region_age = population_region.template slice<AgeGroup>({j.get(), 1});
                        auto Njm = std::accumulate(population_region_age.begin(), population_region_age.end(), 0.);

                        double coeffStoE = 0.5 * contact_matrix.get_matrix_at(y.get_time(t_idx))(i.get(), j.get()) *
                                           params.template get<TransmissionProbabilityOnContact<ScalarType>>()[i];
                        if (n == m) {
                            F(i.get() * num_regions + n.get(), num_age_groups * num_regions + j.get() * num_regions +
                                                                   m.get()) += coeffStoE * y.get_value(t_idx)[Si] / Njm;
                        }
                        for (auto k = Region(0); k < Region(num_regions); k++) {
                            F(i.get() * num_regions + n.get(),
                              num_age_groups * num_regions + j.get() * num_regions + m.get()) +=
                                coeffStoE * y.get_value(t_idx)[Si] *
                                commuting_strengths.get_matrix_at(y.get_time(t_idx))(n.get(), k.get()) *
                                commuting_strengths.get_matrix_at(y.get_time(t_idx))(m.get(), k.get()) /
                                m_population_after_commuting[{k, j}];
                        }
                    }
                }

                double T_Ei = params.template get<TimeExposed<ScalarType>>()[i];
                double T_Ii = params.template get<TimeInfected<ScalarType>>()[i];
                V(i.get() * num_regions + n.get(), i.get() * num_regions + n.get()) = 1.0 / T_Ei;
                V(num_age_groups * num_regions + i.get() * num_regions + n.get(), i.get() * num_regions + n.get()) =
                    -1.0 / T_Ei;
                V(num_age_groups * num_regions + i.get() * num_regions + n.get(),
                  num_age_groups * num_regions + i.get() * num_regions + n.get()) = 1.0 / T_Ii;
            }
        }

        V = V.inverse();

        Eigen::MatrixXd NextGenMatrix = Eigen::MatrixXd::Zero(total_infected_compartments, total_infected_compartments);
        NextGenMatrix                 = F * V;

        //Compute the largest eigenvalue in absolute value
        Eigen::ComplexEigenSolver<Eigen::MatrixXd> ces;

        ces.compute(NextGenMatrix);
        const Eigen::VectorXcd eigen_vals = ces.eigenvalues();

        Eigen::VectorXd eigen_vals_abs;
        eigen_vals_abs.resize(eigen_vals.size());

        for (int i = 0; i < eigen_vals.size(); i++) {
            eigen_vals_abs[i] = std::abs(eigen_vals[i]);
        }
        return mio::success(eigen_vals_abs.maxCoeff());
    }

    /**
    *@brief Computes the reproduction number for all time points of the Model output obtained by the Simulation.
    *@param y The TimeSeries obtained from the Model Simulation.
    *@returns vector containing all reproduction numbers
    */
    Eigen::VectorXd get_reproduction_numbers(const mio::TimeSeries<ScalarType>& y)
    {
        auto num_time_points = y.get_num_time_points();
        Eigen::VectorXd temp(num_time_points);
        for (size_t i = 0; i < static_cast<size_t>(num_time_points); i++) {
            temp[i] = get_reproduction_number(i, y).value();
        }
        return temp;
    }

    mio::Populations<FP, Region, AgeGroup> m_population_after_commuting;
}; // namespace oseirmobilityimproved

} // namespace oseirmobilityimproved
} // namespace mio

#endif // ODESEIRMOBILITY_MODEL_H

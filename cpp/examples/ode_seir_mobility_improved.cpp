
#include "memilio/compartments/simulation.h"
#include "memilio/math/euler.h"
#include "memilio/utils/logging.h"
#include "memilio/utils/custom_index_array.h"
#include "memilio/io/mobility_io.h"
#include "models/ode_seir_mobility_improved/infection_state.h"
#include "models/ode_seir_mobility_improved/model.h"
#include "models/ode_seir_mobility_improved/parameters.h"
#include "models/ode_seir_mobility_improved/regions.h"
#include "memilio/io/io.h"
#include "memilio/io/result_io.h"

#include <chrono>

template <typename FP = ScalarType>
mio::IOResult<void> set_mobility_weights(const std::string& mobility_data, mio::oseirmobilityimproved::Model<FP>& model,
                                         size_t number_regions)
{
    // mobility between nodes
    BOOST_OUTCOME_TRY(auto&& mobility_data_commuter,
                      mio::read_mobility_plain(mobility_data + "/mobility" + "/commuter_migration_test.txt"));
    if (mobility_data_commuter.rows() != Eigen::Index(number_regions) ||
        mobility_data_commuter.cols() != Eigen::Index(number_regions)) {
        return mio::failure(mio::StatusCode::InvalidValue,
                            "Mobility matrices do not have the correct size. You may need to run "
                            "transformMobilitydata.py from pycode memilio epidata package.");
    }

    for (size_t county_idx_i = 0; county_idx_i < number_regions; ++county_idx_i) {
        auto population_i = model.populations.get_group_total(mio::oseirmobilityimproved::Region(county_idx_i));
        mobility_data_commuter.row(county_idx_i) /= population_i;
        mobility_data_commuter(county_idx_i, county_idx_i) = 1 - mobility_data_commuter.rowwise().sum()(county_idx_i);
    }
    model.parameters.template get<mio::oseirmobilityimproved::CommutingStrengths<>>()
        .get_cont_freq_mat()[0]
        .get_baseline() = mobility_data_commuter;

    return mio::success();
}

int main()
{
    mio::set_log_level(mio::LogLevel::debug);

    ScalarType t0   = 0.;
    ScalarType tmax = 15.;
    ScalarType dt   = 0.1;

    ScalarType number_regions = 2;
    std::vector<int> region_ids(number_regions);
    iota(region_ids.begin(), region_ids.end(), 1);
    ScalarType number_age_groups = 1;

    mio::log_info("Simulating SIR; t={} ... {} with dt = {}.", t0, tmax, dt);

    const std::string& mobility_data = "";

    mio::oseirmobilityimproved::Model<ScalarType> model(number_regions, number_age_groups);
    model.populations[{mio::oseirmobilityimproved::Region(0), mio::AgeGroup(0),
                       mio::oseirmobilityimproved::InfectionState::Exposed}]     = 10;
    model.populations[{mio::oseirmobilityimproved::Region(0), mio::AgeGroup(0),
                       mio::oseirmobilityimproved::InfectionState::Susceptible}] = 9990;
    for (int i = 1; i < number_regions; i++) {
        model.populations[{mio::oseirmobilityimproved::Region(i), mio::AgeGroup(0),
                           mio::oseirmobilityimproved::InfectionState::Exposed}]     = 0;
        model.populations[{mio::oseirmobilityimproved::Region(i), mio::AgeGroup(0),
                           mio::oseirmobilityimproved::InfectionState::Susceptible}] = 10000;
    }

    model.parameters.set<mio::oseirmobilityimproved::TransmissionProbabilityOnContact<>>(1.);

    model.parameters.set<mio::oseirmobilityimproved::TimeExposed<>>(3.);
    model.parameters.set<mio::oseirmobilityimproved::TimeInfected<>>(5.);

    mio::ContactMatrixGroup& contact_matrix =
        model.parameters.get<mio::oseirmobilityimproved::ContactPatterns<>>().get_cont_freq_mat();
    contact_matrix[0].get_baseline().setConstant(2.7);
    // contact_matrix[0].add_damping(0.5, mio::SimulationTime(5));

    auto result_preprocess = set_mobility_weights(mobility_data, model, number_regions);
    mio::ContactMatrixGroup& commuting_strengths =
        model.parameters.get<mio::oseirmobilityimproved::CommutingStrengths<>>().get_cont_freq_mat();

    auto& population = model.m_population_after_commuting;
    for (int region_n = 0; region_n < number_regions; ++region_n) {
        for (int age = 0; age < number_age_groups; ++age) {
            double population_n = 0;
            for (size_t state = 0; state < (size_t)mio::oseirmobilityimproved::InfectionState::Count; state++) {
                population_n += model.populations[{mio::oseirmobilityimproved::Region(region_n), mio::AgeGroup(age),
                                                   mio::oseirmobilityimproved::InfectionState(state)}];
            }
            population[{mio::oseirmobilityimproved::Region(region_n), mio::AgeGroup(age)}] += population_n;
            for (int region_m = 0; region_m < number_regions; ++region_m) {
                population[{mio::oseirmobilityimproved::Region(region_n), mio::AgeGroup(age)}] -=
                    commuting_strengths[0].get_baseline()(region_n, region_m) * population_n;
                population[{mio::oseirmobilityimproved::Region(region_m), mio::AgeGroup(age)}] +=
                    commuting_strengths[0].get_baseline()(region_n, region_m) * population_n;
            }
        }
    }
    // using DefaultIntegratorCore =
    //     mio::ControlledStepperWrapper<ScalarType, boost::numeric::odeint::runge_kutta_cash_karp54>;

    std::shared_ptr<mio::IntegratorCore<ScalarType>> integrator = std::make_shared<mio::EulerIntegratorCore<>>();

    model.check_constraints();

    printf("Start Simulation\n");
    auto t1              = std::chrono::high_resolution_clock::now();
    auto result_from_sim = simulate(t0, tmax, dt, model, integrator);
    auto t2              = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> ms_double = t2 - t1;

    printf("Runtime: %f\n", ms_double.count());

    auto save_result_status =
        mio::save_result({result_from_sim}, region_ids, number_regions * number_age_groups, "ode_result_test.h5");

    auto reproduction_numbers = model.get_reproduction_numbers(result_from_sim);
    std::cout << "\nbasis reproduction number: " << reproduction_numbers[0] << "\n";

    result_from_sim.print_table();
}

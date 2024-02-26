#include "memilio/utils/time_series.h"
#include "memilio/utils/logging.h"
#include "memilio/compartments/simulation.h"
#include "ode_sir/model.h"

int main(){
    mio::set_log_level(mio::LogLevel::debug);

    double t0   = 0;
    double tmax = 50;
    double dt   = 0.1;

    mio::log_info("Simulating SIR; t={} ... {} with dt = {}.", t0, tmax, dt);

    double cont_freq = 10; // see Polymod study

    double nb_total_t0 = 10000, nb_inf_t0 = 50, nb_rec_t0 = 10;

    // alpha = alpha_in; // percentage of asymptomatic cases
    // beta  = beta_in; // risk of infection from the infected symptomatic patients
    // rho   = rho_in; // hospitalized per infected
    // theta = theta_in; // icu per hospitalized
    // delta = delta_in; // deaths per ICUs

    mio::osir::Model model(3);
    auto nb_groups = model.parameters.get_num_groups();

    double fact    = 1.0 / (double)(size_t)nb_groups;

    auto& params = model.parameters;

    for (auto i = mio::AgeGroup(0); i < nb_groups; i++) {
        model.populations[{i, mio::osir::InfectionState::Infected}]  = fact*nb_inf_t0;
        model.populations[{i, mio::osir::InfectionState::Recovered}] = fact*nb_rec_t0;
        model.populations.set_difference_from_group_total<mio::AgeGroup>({i, mio::osir::InfectionState::Susceptible},
                                                                         fact * nb_total_t0);

        model.parameters.get<mio::osir::TimeInfected>()[i] = 2.0;
        model.parameters.get<mio::osir::TransmissionProbabilityOnContact>()[i] = 0.3;

    }

    mio::ContactMatrixGroup& contact_matrix = params.get<mio::osir::ContactPatterns>();
    contact_matrix[0] =
        mio::ContactMatrix(Eigen::MatrixXd::Constant((size_t)nb_groups, (size_t)nb_groups, fact * cont_freq));
    contact_matrix.add_damping(Eigen::MatrixXd::Constant((size_t)nb_groups, (size_t)nb_groups, 0.7),
                               mio::SimulationTime(30.));

    model.apply_constraints();

    mio::TimeSeries<double> sir = simulate(t0, tmax, dt, model);

    std::vector<std::string> vars = {"S", "I", "R"};
    printf("Number of time points :%d\n", static_cast<int>(sir.get_num_time_points()));
    printf("People in\n");

    for (size_t k = 0; k < (size_t)mio::osir::InfectionState::Count; k++) {
        double dummy = 0;

        for (size_t i = 0; i < (size_t)params.get_num_groups(); i++) {
            printf("\t %s[%d]: %.0f", vars[k].c_str(), (int)i,
                   sir.get_last_value()[k + (size_t)mio::osir::InfectionState::Count * (int)i]);
            dummy += sir.get_last_value()[k + (size_t)mio::osir::InfectionState::Count * (int)i];
        }

        printf("\t %s_otal: %.0f\n", vars[k].c_str(), dummy);
    }
}
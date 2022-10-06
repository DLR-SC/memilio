/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
*
* Authors: Daniel Abele, Martin J. Kuehn
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
#include "secir/secir.h"
#include "memilio/utils/time_series.h"
#include "memilio/utils/logging.h"
#include "memilio/compartments/simulation.h"

int main()
{

    mio::set_log_level(mio::LogLevel::debug);

    double t0   = 0;
    double tmax = 50;
    double dt   = 0.1;

    mio::log_info("Simulating SECIR; t={} ... {} with dt = {}.", t0, tmax, dt);

    double cont_freq = 10; // see Polymod study

    double nb_total_t0 = 10000, nb_exp_t0 = 100, nb_inf_t0 = 50, nb_car_t0 = 50, nb_hosp_t0 = 20, nb_icu_t0 = 10,
           nb_rec_t0 = 10, nb_dead_t0 = 0;

    // alpha = alpha_in; // percentage of asymptomatic cases
    // beta  = beta_in; // risk of infection from the infected symptomatic patients
    // rho   = rho_in; // hospitalized per infected
    // theta = theta_in; // icu per hospitalized
    // delta = delta_in; // deaths per ICUs

    mio::SecirModel model(3);
    auto nb_groups = model.parameters.get_num_groups();
    double fact    = 1.0 / (double)(size_t)nb_groups;

    auto& params = model.parameters;

    params.set<mio::ICUCapacity>(std::numeric_limits<double>::max());
    params.set<mio::StartDay>(0);
    params.set<mio::Seasonality>(0);

    for (auto i = mio::AgeGroup(0); i < nb_groups; i++) {
        params.get<mio::IncubationTime>()[i]         = 5.2;
        params.get<mio::TimeInfectedSymptoms>()[i]     = 6.;
        params.get<mio::SerialInterval>()[i]         = 4.2;
        params.get<mio::TimeInfectedSevere>()[i] = 12;
        params.get<mio::TimeInfectedCritical>()[i]          = 8;

        model.populations[{i, mio::InfectionState::Exposed}]      = fact * nb_exp_t0;
        model.populations[{i, mio::InfectionState::Carrier}]      = fact * nb_car_t0;
        model.populations[{i, mio::InfectionState::Infected}]     = fact * nb_inf_t0;
        model.populations[{i, mio::InfectionState::Hospitalized}] = fact * nb_hosp_t0;
        model.populations[{i, mio::InfectionState::ICU}]          = fact * nb_icu_t0;
        model.populations[{i, mio::InfectionState::Recovered}]    = fact * nb_rec_t0;
        model.populations[{i, mio::InfectionState::Dead}]         = fact * nb_dead_t0;
        model.populations.set_difference_from_group_total<mio::AgeGroup>({i, mio::InfectionState::Susceptible},
                                                                         fact * nb_total_t0);

        params.get<mio::TransmissionProbabilityOnContact>()[i] = 0.05;
        params.get<mio::RelativeTransmissionNoSymptoms>()[i]    = 0.67;
        params.get<mio::RecoveredPerInfectedNoSymptoms>()[i]    = 0.09;
        params.get<mio::RiskOfInfectionFromSymptomatic>()[i]   = 0.25;
        params.get<mio::SeverePerInfectedSymptoms>()[i]  = 0.2;
        params.get<mio::CriticalPerSevere>()[i]         = 0.25;
        params.get<mio::DeathsPerCritical>()[i]                    = 0.3;
    }

    mio::ContactMatrixGroup& contact_matrix = params.get<mio::ContactPatterns>();
    contact_matrix[0] =
        mio::ContactMatrix(Eigen::MatrixXd::Constant((size_t)nb_groups, (size_t)nb_groups, fact * cont_freq));
    contact_matrix.add_damping(Eigen::MatrixXd::Constant((size_t)nb_groups, (size_t)nb_groups, 0.7),
                               mio::SimulationTime(30.));

    model.apply_constraints();

    mio::TimeSeries<double> secir = simulate(t0, tmax, dt, model);

    char vars[] = {'S', 'E', 'C', 'I', 'H', 'U', 'R', 'D'};
    printf("Number of time points :%d\n", static_cast<int>(secir.get_num_time_points()));
    printf("People in\n");

    for (size_t k = 0; k < (size_t)mio::InfectionState::Count; k++) {
        double dummy = 0;

        for (size_t i = 0; i < (size_t)params.get_num_groups(); i++) {
            printf("\t %c[%d]: %.0f", vars[k], (int)i,
                   secir.get_last_value()[k + (size_t)mio::InfectionState::Count * (int)i]);
            dummy += secir.get_last_value()[k + (size_t)mio::InfectionState::Count * (int)i];
        }

        printf("\t %c_otal: %.0f\n", vars[k], dummy);
    }
}

/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Lena Ploetzke
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

#include "lct_secir/model.h"
#include "lct_secir/infection_state.h"
#include "lct_secir/simulation.h"
#include "memilio/config.h"
#include "memilio/utils/time_series.h"
#include "memilio/epidemiology/uncertain_matrix.h"
#include "memilio/math/eigen.h"
#include "memilio/io/result_io.h"

#include "ode_secir/model.h"
#include <vector>
#include "boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp"

int main()
{
    ScalarType t0   = 0;
    ScalarType tmax = 20;
    ScalarType dt   = 0.1;

    // ---Perform simulation for the LCT SECIR model.---
    // Set vector that specifies the number of subcompartments
    std::vector<int> SubcompartmentNumbers((int)mio::lsecir::InfectionStateBase::Count, 1);
    SubcompartmentNumbers[(int)mio::lsecir::InfectionStateBase::Exposed]            = 5;
    SubcompartmentNumbers[(int)mio::lsecir::InfectionStateBase::InfectedNoSymptoms] = 5;
    SubcompartmentNumbers[(int)mio::lsecir::InfectionStateBase::InfectedSymptoms]   = 5;
    mio::lsecir::InfectionState InfState(SubcompartmentNumbers);

    // define initial population distribution in infection states, one entry per Subcompartment
    Eigen::VectorXd init_lct(InfState.get_count());
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::Susceptible)]            = 7500;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::Exposed)]                = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::Exposed) + 1]            = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::Exposed) + 2]            = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::Exposed) + 3]            = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::Exposed) + 4]            = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedNoSymptoms)]     = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedNoSymptoms) + 1] = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedNoSymptoms) + 2] = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedNoSymptoms) + 3] = 50;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedNoSymptoms) + 4] = 50;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedSymptoms)]       = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedSymptoms) + 1]   = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedSymptoms) + 2]   = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedSymptoms) + 3]   = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedSymptoms) + 4]   = 100;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedSevere)]         = 500;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::InfectedCritical)]       = 300;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::Recovered)]              = 200;
    init_lct[InfState.get_firstindex(mio::lsecir::InfectionStateBase::Dead)]                   = 100;

    // initialize model
    mio::lsecir::Model model_lct(std::move(init_lct), InfState);

    // Set Parameters of the model
    model_lct.parameters.get<mio::lsecir::TimeExposed>()            = 2 * 4.2 - 5.2;
    model_lct.parameters.get<mio::lsecir::TimeInfectedNoSymptoms>() = 2 * (5.2 - 4.2);
    model_lct.parameters.get<mio::lsecir::TimeInfectedSymptoms>()   = 5.8;
    model_lct.parameters.get<mio::lsecir::TimeInfectedSevere>()     = 9.5;
    model_lct.parameters.get<mio::lsecir::TimeInfectedCritical>()   = 7.1;

    model_lct.parameters.get<mio::lsecir::TransmissionProbabilityOnContact>() = 0.05;

    mio::ContactMatrixGroup& contact_matrix = model_lct.parameters.get<mio::lsecir::ContactPatterns>();
    contact_matrix[0]                       = mio::ContactMatrix(Eigen::MatrixXd::Constant(1, 1, 10));

    model_lct.parameters.get<mio::lsecir::RelativeTransmissionNoSymptoms>() = 0.7;
    model_lct.parameters.get<mio::lsecir::RiskOfInfectionFromSymptomatic>() = 0.25;
    model_lct.parameters.get<mio::lsecir::RecoveredPerInfectedNoSymptoms>() = 0.09;
    model_lct.parameters.get<mio::lsecir::SeverePerInfectedSymptoms>()      = 0.2;
    model_lct.parameters.get<mio::lsecir::CriticalPerSevere>()              = 0.25;
    model_lct.parameters.get<mio::lsecir::DeathsPerCritical>()              = 0.3;

    // perform simulation
    mio::TimeSeries<ScalarType> result_lct = mio::lsecir::simulate(
        t0, tmax, dt, model_lct,
        std::make_shared<mio::ControlledStepperWrapper<boost::numeric::odeint::runge_kutta_cash_karp54>>());
    // calculate the distribution in Infectionstates without subcompartments of the result
    mio::TimeSeries<ScalarType> populations_lct = model_lct.calculate_populations(result_lct);

    // Save result in HDF5 file
    auto save_result_status_subcompartments = mio::save_result({result_lct}, {0}, 1, "result_lct_subcompartments.h5");
    auto save_result_status_lct             = mio::save_result({populations_lct}, {0}, 1, "result_lct.h5");

    // Initialize ODE model with one single age group
    mio::osecir::Model model_ode(1);

    // Initial population for ODE model fitting to initial condition of lct model
    Eigen::VectorXd init_ode((int)mio::osecir::InfectionState::Count);
    init_ode << 7500, 500, 400, 500, 500, 300, 200, 100;

    //Set population
    model_ode.populations.set_total(init_ode.sum());
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::Exposed}] =
        init_ode[Eigen::Index(mio::lsecir::InfectionStateBase::Exposed)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedNoSymptoms}] =
        init_ode[Eigen::Index(mio::lsecir::InfectionStateBase::InfectedNoSymptoms)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedSymptoms}] =
        init_ode[Eigen::Index(mio::lsecir::InfectionStateBase::InfectedSymptoms)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedSevere}] =
        init_ode[Eigen::Index(mio::lsecir::InfectionStateBase::InfectedSevere)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::InfectedCritical}] =
        init_ode[Eigen::Index(mio::lsecir::InfectionStateBase::InfectedCritical)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::Recovered}] =
        init_ode[Eigen::Index(mio::lsecir::InfectionStateBase::Recovered)];
    model_ode.populations[{mio::AgeGroup(0), mio::osecir::InfectionState::Dead}] =
        init_ode[Eigen::Index(mio::lsecir::InfectionStateBase::Dead)];
    model_ode.populations.set_difference_from_total({mio::AgeGroup(0), mio::osecir::InfectionState::Susceptible},
                                                    init_ode.sum());

    // set parameters fitting to these of the lct model
    // no restrictions by additional parameters
    model_ode.parameters.set<mio::osecir::StartDay>(0);
    model_ode.parameters.set<mio::osecir::Seasonality>(0);
    model_ode.parameters.get<mio::osecir::TestAndTraceCapacity>() = std::numeric_limits<double>::max();
    model_ode.parameters.get<mio::osecir::ICUCapacity>()          = std::numeric_limits<double>::max();

    model_ode.parameters.get<mio::osecir::IncubationTime>()[(mio::AgeGroup)0] =
        5.2; // TimeExposed = 2 * SerialInterval - IncubationTime
    model_ode.parameters.get<mio::osecir::SerialInterval>()[(mio::AgeGroup)0] =
        4.2; // TimeInfectedNoSymptoms = 2* (IncubationTime - SerialInterval)
    model_ode.parameters.get<mio::osecir::TimeInfectedSymptoms>()[(mio::AgeGroup)0] = 5.8;
    model_ode.parameters.get<mio::osecir::TimeInfectedSevere>()[(mio::AgeGroup)0]   = 9.5;
    model_ode.parameters.get<mio::osecir::TimeInfectedCritical>()[(mio::AgeGroup)0] = 7.1;

    mio::ContactMatrixGroup& contact_matrix_ode = model_ode.parameters.get<mio::osecir::ContactPatterns>();
    contact_matrix_ode[0]                       = mio::ContactMatrix(Eigen::MatrixXd::Constant(1, 1, 10));

    model_ode.parameters.get<mio::osecir::TransmissionProbabilityOnContact>()[(mio::AgeGroup)0] = 0.05;
    model_ode.parameters.get<mio::osecir::RelativeTransmissionNoSymptoms>()[(mio::AgeGroup)0]   = 0.7;
    model_ode.parameters.get<mio::osecir::RecoveredPerInfectedNoSymptoms>()[(mio::AgeGroup)0]   = 0.09;
    model_ode.parameters.get<mio::osecir::RiskOfInfectionFromSymptomatic>()[(mio::AgeGroup)0]   = 0.25;
    model_ode.parameters.get<mio::osecir::SeverePerInfectedSymptoms>()[(mio::AgeGroup)0]        = 0.2;
    model_ode.parameters.get<mio::osecir::CriticalPerSevere>()[(mio::AgeGroup)0]                = 0.25;
    model_ode.parameters.get<mio::osecir::DeathsPerCritical>()[(mio::AgeGroup)0]                = 0.3;

    mio::TimeSeries<double> result_ode =
        simulate(t0, tmax, dt, model_ode,
                 std::make_shared<mio::ControlledStepperWrapper<boost::numeric::odeint::runge_kutta_cash_karp54>>());

    // Save result in HDF5 file
    auto save_result_status_ode = mio::save_result({result_ode}, {0}, 1, "result_ode.h5");
}
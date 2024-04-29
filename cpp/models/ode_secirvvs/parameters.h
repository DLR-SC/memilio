/* 
* Copyright (C) 2020-2024 MEmilio
*
* Authors: Wadim Koslow, Daniel Abele, Martin J. Kühn
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
#ifndef ODESECIRVVS_PARAMETERS_H
#define ODESECIRVVS_PARAMETERS_H

#include "memilio/math/eigen.h"
#include "memilio/utils/uncertain_value.h"
#include "memilio/math/adapt_rk.h"
#include "memilio/epidemiology/age_group.h"
#include "memilio/epidemiology/simulation_day.h"
#include "memilio/epidemiology/uncertain_matrix.h"
#include "memilio/epidemiology/dynamic_npis.h"
#include "memilio/utils/parameter_set.h"
#include "memilio/utils/custom_index_array.h"

#include <cstddef>
#include <vector>

namespace mio
{
namespace osecirvvs
{

/**
* @brief the start day in the SECIRVVS model
* The start day defines in which season the simulation can be started
* If the start day is 180 and simulation takes place from t0=0 to
* tmax=100 the days 180 to 280 of the year are simulated
*/
struct StartDay {
    using Type = double;
    static Type get_default(AgeGroup)
    {
        return 0.;
    }
    static std::string name()
    {
        return "StartDay";
    }
};

/**
* @brief the start day of a new variant in the SECIRVVS model
* The start day of the new variant defines in which day of the simulation the new variant is introduced.
* Starting on this day, the new variant will impact the transmission probability depending on the
* infectiousness of the new variant in the parameter InfectiousnessNewVariant.
*/
struct StartDayNewVariant {
    using Type = double;
    static Type get_default(AgeGroup)
    {
        return std::numeric_limits<double>::max();
    }
    static std::string name()
    {
        return "StartDayNewVariant";
    }
};

/**
* @brief the seasonality in the SECIR model
* the seasonality is given as (1+k*sin()) where the sine
* curve is below one in summer and above one in winter
*/
struct Seasonality {
    using Type = UncertainValue;
    static Type get_default(AgeGroup)
    {
        return Type(0.);
    }
    static std::string name()
    {
        return "Seasonality";
    }
};

/**
* @brief the icu capacity in the SECIR model
*/
struct ICUCapacity {
    using Type = UncertainValue;
    static Type get_default(AgeGroup)
    {
        return Type(std::numeric_limits<double>::max());
    }
    static std::string name()
    {
        return "ICUCapacity";
    }
};

/**
 * @brief capacity to test and trace contacts of infected for quarantine per day.
 */
struct TestAndTraceCapacity {
    using Type = UncertainValue;
    static Type get_default(AgeGroup)
    {
        return Type(std::numeric_limits<double>::max());
    }
    static std::string name()
    {
        return "TestAndTraceCapacity";
    }
};

/**
 * @brief the contact patterns within the society are modelled using an UncertainContactMatrix
 */
struct ContactPatterns {
    using Type = UncertainContactMatrix;
    static Type get_default(AgeGroup size)
    {
        return Type(1, static_cast<Eigen::Index>((size_t)size));
    }
    static std::string name()
    {
        return "ContactPatterns";
    }
};

/**
 * @brief the NPIs that are enacted if certain infection thresholds are exceeded.
 */
struct DynamicNPIsInfectedSymptoms {
    using Type = DynamicNPIs;
    static Type get_default(AgeGroup /*size*/)
    {
        return {};
    }
    static std::string name()
    {
        return "DynamicNPIsInfectedSymptoms";
    }
};

/**
 * @brief the (mean) latent time in day unit
 */
struct TimeExposed {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "TimeExposed";
    }
};

/**
 * @brief the (mean) time in day unit for asymptomatic cases that are infected but
 *        have not yet developed symptoms.
 */
struct TimeInfectedNoSymptoms {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "TimeInfectedNoSymptoms";
    }
};

/**
* @brief the infectious time for symptomatic cases that are infected but
*        who do not need to be hsopitalized in the SECIR model in day unit
*/
struct TimeInfectedSymptoms {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "TimeInfectedSymptoms";
    }
};

/**
 * @brief the time people are 'simply' hospitalized before returning home in the SECIR model
 *        in day unit
 */
struct TimeInfectedSevere {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "TimeInfectedSevere";
    }
};

/**
 * @brief the time people are treated by ICU before returning home in the SECIR model
 *        in day unit
 */
struct TimeInfectedCritical {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "TimeInfectedCritical";
    }
};

/**
* @brief probability of getting infected from a contact
*/
struct TransmissionProbabilityOnContact {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "TransmissionProbabilityOnContact";
    }
};

/**
* @brief the relative InfectedNoSymptoms infectability
*/
struct RelativeTransmissionNoSymptoms {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "RelativeTransmissionNoSymptoms";
    }
};

/**
* @brief the percentage of asymptomatic cases in the SECIR model
*/
struct RecoveredPerInfectedNoSymptoms {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 0.);
    }
    static std::string name()
    {
        return "RecoveredPerInfectedNoSymptoms";
    }
};

/**
* @brief the risk of infection from symptomatic cases in the SECIR model
*/
struct RiskOfInfectionFromSymptomatic {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 0.);
    }
    static std::string name()
    {
        return "RiskOfInfectionFromSymptomatic";
    }
};

/**
* @brief risk of infection from symptomatic cases increases as test and trace capacity is exceeded.
*/
struct MaxRiskOfInfectionFromSymptomatic {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 0.);
    }
    static std::string name()
    {
        return "MaxRiskOfInfectionFromSymptomatic";
    }
};

/**
* @brief the percentage of hospitalized patients per infected patients in the SECIR model
*/
struct SeverePerInfectedSymptoms {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 0.);
    }
    static std::string name()
    {
        return "SeverePerInfectedSymptoms";
    }
};

/**
* @brief the percentage of ICU patients per hospitalized patients in the SECIR model
*/
struct CriticalPerSevere {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 0.);
    }
    static std::string name()
    {
        return "CriticalPerSevere";
    }
};

/**
* @brief the percentage of dead patients per ICU patients in the SECIR model
*/
struct DeathsPerCritical {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 0.);
    }
    static std::string name()
    {
        return "DeathsPerCritical";
    }
};

/**
 * @brief Time in days between first and second vaccine dose.
 */
struct VaccinationGap {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 49.0);
    }
    static std::string name()
    {
        return "VaccinationGap";
    }
};

/**
 * @brief Time in days until first vaccine dose takes full effect.
 */
struct DaysUntilEffectivePartialImmunity {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 14.0);
    }
    static std::string name()
    {
        return "DaysUntilEffectivePartialImmunity";
    }
};

/**
 * @brief Time in days until second vaccine dose takes full effect.
 */
struct DaysUntilEffectiveImprovedImmunity {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 7.0);
    }
    static std::string name()
    {
        return "DaysUntilEffectiveImprovedImmunity";
    }
};

/**
* @brief Total number of first vaccinations up to the given day.
*/
struct DailyFirstVaccination {
    using Type = CustomIndexArray<double, AgeGroup, SimulationDay>;
    static Type get_default(AgeGroup size)
    {
        return Type({size, SimulationDay(0)});
    }
    static std::string name()
    {
        return "DailyFirstVaccination";
    }
};

/**
* @brief Total number of full vaccinations up to the given day.
*/
struct DailyFullVaccination {
    using Type = CustomIndexArray<double, AgeGroup, SimulationDay>;
    static Type get_default(AgeGroup size)
    {
        return Type({size, SimulationDay(0)});
    }
    static std::string name()
    {
        return "DailyFullVaccination";
    }
};

/**
 * @brief Factor to reduce infection risk for persons with partial immunity.
 */
struct ReducExposedPartialImmunity {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "ReducExposedPartialImmunity";
    }
};

/**
 * @brief Factor to reduce infection risk for persons with improved immunity.
 */
struct ReducExposedImprovedImmunity {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "ReducExposedImprovedImmunity";
    }
};

/**
 * @brief Factor to reduce risk of developing symptoms for persons with partial immunity.
 */
struct ReducInfectedSymptomsPartialImmunity {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "ReducInfectedSymptomsPartialImmunity";
    }
};

/**
 * @brief Factor to reduce risk of developing symptoms for persons with improved immunity.
 */
struct ReducInfectedSymptomsImprovedImmunity {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "ReducInfectedSymptomsImprovedImmunity";
    }
};

/**
 * @brief Factor to reduce risk of hospitalization for persons with partial immunity.
 * Also applies to ICU and Death risk.
 */
struct ReducInfectedSevereCriticalDeadPartialImmunity {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "ReducInfectedSevereCriticalDeadPartialImmunity";
    }
};

/**
 * @brief Factor to reduce risk of hospitalization for persons with improved immunity.
 */
struct ReducInfectedSevereCriticalDeadImprovedImmunity {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "ReducInfectedSevereCriticalDeadImprovedImmunity";
    }
};

/**
 * @brief Factor to reduce infectious time of persons with partial or improved immunity.
 */
struct ReducTimeInfectedMild {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 0.5);
    }
    static std::string name()
    {
        return "ReducTimeInfectedMild";
    }
};

/**
 * @brief Represents the relative infectiousness of a new variant.
 */
struct InfectiousnessNewVariant {
    using Type = CustomIndexArray<double, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.0);
    }
    static std::string name()
    {
        return "InfectiousnessNewVariant";
    }
};

/**
* @brief Daily ICU occupancy aggregated over all age groups.
*/
struct DailyICUOccupancy {
    using Type = std::vector<double>;
    static Type get_default()
    {
        return {};
    }
    static std::string name()
    {
        return "DailyICUOccupancy";
    }
};

/**
* @brief shape parameter of the gamma distribution for the vaccinations.
*/
struct alphaGammaVaccination {
    using Type = double;
    static Type get_default()
    {
        return 6.;
    }
    static std::string name()
    {
        return "alphaGammaVaccination";
    }
};

/**
* @brief scale parameter of the gamma distribution for the vaccinations.
*/
struct betaGammaVaccination {
    using Type = double;
    static Type get_default()
    {
        return .4;
    }
    static std::string name()
    {
        return "betaGammaVaccination";
    }
};

/**
* @brief shape parameter of the gamma distribution for the contacts.
*/
struct alphaGammaContacts {
    using Type = double;
    static Type get_default()
    {
        return 6.;
    }
    static std::string name()
    {
        return "alphaGammaContacts";
    }
};

/**
* @brief scale parameter of the gamma distribution for the contacts.
*/
struct betaGammaContacts {
    using Type = double;
    static Type get_default()
    {
        return .7;
    }
    static std::string name()
    {
        return "betaGammaContacts";
    }
};

/**
* @brief number of days in the past we consider for the gamma distribution.
*/
struct CutOffGamma {
    using Type = size_t;
    static Type get_default()
    {
        return 40;
    }
    static std::string name()
    {
        return "CutOffGamma";
    }
};

struct ContactReductionWorkMax {
    using Type = double;
    static Type get_default()
    {
        return 0.0;
    }
    static std::string name()
    {
        return "ContactReductionWorkMax";
    }
};

struct ContactReductionWorkMin {
    using Type = double;
    static Type get_default()
    {
        return 1.0;
    }
    static std::string name()
    {
        return "ContactReductionWorkMin";
    }
};

struct ContactReductionHomeMax {
    using Type = double;
    static Type get_default()
    {
        return 0.0;
    }
    static std::string name()
    {
        return "ContactReductionHomeMax";
    }
};

struct ContactReductionHomeMin {
    using Type = double;
    static Type get_default()
    {
        return 1.0;
    }
    static std::string name()
    {
        return "ContactReductionHomeMin";
    }
};

struct ContactReductionSchoolMax {
    using Type = double;
    static Type get_default()
    {
        return 0.0;
    }
    static std::string name()
    {
        return "ContactReductionSchoolMax";
    }
};

struct ContactReductionSchoolMin {
    using Type = double;
    static Type get_default()
    {
        return 1.0;
    }
    static std::string name()
    {
        return "ContactReductionSchoolMin";
    }
};

struct ContactReductionOtherMax {
    using Type = double;
    static Type get_default()
    {
        return 0.0;
    }
    static std::string name()
    {
        return "ContactReductionOtherMax";
    }
};

struct ContactReductionOtherMin {
    using Type = double;
    static Type get_default()
    {
        return 1.0;
    }
    static std::string name()
    {
        return "ContactReductionOtherMin";
    }
};
struct DelayTimeVaccination {
    using Type = double;
    static Type get_default()
    {
        return 1.0;
    }
    static std::string name()
    {
        return "DelayTimeVaccination";
    }
};

struct EpsilonVaccination {
    using Type = double;
    static Type get_default()
    {
        return 0.01;
    }
    static std::string name()
    {
        return "EpsilonVaccination";
    }
};

struct EpsilonContacts {
    using Type = double;
    static Type get_default()
    {
        return 10.0;
    }
    static std::string name()
    {
        return "EpsilonContacts";
    }
};

struct WillignessToVaccinateFirstMax {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "WillignessToVaccinateFirstMax";
    }
};

struct WillignessToVaccinateFullMax {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "WillignessToVaccinateFullMax";
    }
};

struct WillignessToVaccinateBoosterMax {
    using Type = CustomIndexArray<UncertainValue, AgeGroup>;
    static Type get_default(AgeGroup size)
    {
        return Type(size, 1.);
    }
    static std::string name()
    {
        return "WillignessToVaccinateBoosterMax";
    }
};

using ParametersBase =
    ParameterSet<StartDay, Seasonality, ICUCapacity, TestAndTraceCapacity, ContactPatterns, DynamicNPIsInfectedSymptoms,
                 TimeExposed, TimeInfectedNoSymptoms, TimeInfectedSymptoms, TimeInfectedSevere, TimeInfectedCritical,
                 TransmissionProbabilityOnContact, RelativeTransmissionNoSymptoms, RecoveredPerInfectedNoSymptoms,
                 RiskOfInfectionFromSymptomatic, MaxRiskOfInfectionFromSymptomatic, SeverePerInfectedSymptoms,
                 CriticalPerSevere, DeathsPerCritical, VaccinationGap, DaysUntilEffectivePartialImmunity,
                 DaysUntilEffectiveImprovedImmunity, DailyFullVaccination, DailyFirstVaccination,
                 ReducExposedPartialImmunity, ReducExposedImprovedImmunity, ReducInfectedSymptomsPartialImmunity,
                 ReducInfectedSymptomsImprovedImmunity, ReducInfectedSevereCriticalDeadPartialImmunity,
                 ReducInfectedSevereCriticalDeadImprovedImmunity, ReducTimeInfectedMild, InfectiousnessNewVariant,
                 StartDayNewVariant, DailyICUOccupancy, alphaGammaVaccination, betaGammaVaccination, alphaGammaContacts,
                 betaGammaContacts, CutOffGamma, ContactReductionWorkMax, ContactReductionWorkMin,
                 ContactReductionHomeMax, ContactReductionHomeMin, ContactReductionSchoolMax, ContactReductionSchoolMin,
                 ContactReductionOtherMax, ContactReductionOtherMin, DelayTimeVaccination, EpsilonVaccination,
                 EpsilonContacts>;

/**
 * @brief Parameters of an age-resolved SECIR/SECIHURD model with paths for partial and improved immunity through vaccination.
 */
class Parameters : public ParametersBase
{
public:
    Parameters(AgeGroup num_agegroups)
        : ParametersBase(num_agegroups)
        , m_num_groups{num_agegroups}
    {
    }

    AgeGroup get_num_groups() const
    {
        return m_num_groups;
    }

    /**
     * Percentage of infected commuters that are not detected.
     */
    double& get_commuter_nondetection()
    {
        return m_commuter_nondetection;
    }
    double get_commuter_nondetection() const
    {
        return m_commuter_nondetection;
    }

    /**
     * Time in simulation before which no infected commuters are detected.
     */
    double& get_start_commuter_detection()
    {
        return m_start_commuter_detection;
    }

    double get_start_commuter_detection() const
    {
        return m_start_commuter_detection;
    }

    /**
     * Time in simulation after which no infected commuters are detected.
     */
    double& get_end_commuter_detection()
    {
        return m_end_commuter_detection;
    }

    double get_end_commuter_detection() const
    {
        return m_end_commuter_detection;
    }

    /**
     * Time in simulation after which no dynamic NPIs are applied.
     */
    double& get_end_dynamic_npis()
    {
        return m_end_dynamic_npis;
    }
    double get_end_dynamic_npis() const
    {
        return m_end_dynamic_npis;
    }

    /**
     * @brief Checks whether all Parameters satisfy their corresponding constraints and applies them, if they do not.
     * Time spans cannot be negative and probabilities can only take values between [0,1]. 
     *
     * Attention: This function should be used with care. It is necessary for some test problems to run through quickly,
     *            but in a manual execution of an example, check_constraints() may be preferred. Note that the apply_constraints()
     *            function can and will not set Parameters to meaningful values in an epidemiological or virological context,
     *            as all models are designed to be transferable to multiple diseases. Consequently, only acceptable
     *            (like 0 or 1 for probabilities or small positive values for time spans) values are set here and a manual adaptation
     *            may often be necessary to have set meaningful values.
     *
     * @return Returns true if one ore more constraint were corrected, false otherwise.  
     */
    bool apply_constraints()
    {
        int corrected = false;
        if (this->get<Seasonality>() < 0.0 || this->get<Seasonality>() > 0.5) {
            log_warning("Constraint check: Parameter Seasonality changed from {} to {}", this->get<Seasonality>(), 0);
            this->set<Seasonality>(0);
            corrected = true;
        }

        if (this->get<ICUCapacity>() < 0.0) {
            log_warning("Constraint check: Parameter ICUCapacity changed from {} to {}", this->get<ICUCapacity>(), 0);
            this->set<ICUCapacity>(0);
            corrected = true;
        }

        const double tol_times = 1e-1; // accepted tolerance for compartment stays

        for (auto i = AgeGroup(0); i < AgeGroup(m_num_groups); ++i) {

            if (this->get<TimeExposed>()[i] < tol_times) {
                log_warning("Constraint check: Parameter TimeExposed changed from {:.4f} to {:.4f}. Please "
                            "note that unreasonably small compartment stays lead to massively increased run time. "
                            "Consider to cancel and reset parameters.",
                            this->get<TimeExposed>()[i], tol_times);
                this->get<TimeExposed>()[i] = tol_times;
                corrected                   = true;
            }

            if (this->get<TimeInfectedNoSymptoms>()[i] < tol_times) {
                log_warning("Constraint check: Parameter TimeInfectedNoSymptoms changed from {:.4f} to {:.4f}. Please "
                            "note that unreasonably small compartment stays lead to massively increased run time. "
                            "Consider to cancel and reset parameters.",
                            this->get<TimeInfectedNoSymptoms>()[i], tol_times);
                this->get<TimeInfectedNoSymptoms>()[i] = tol_times;
                corrected                              = true;
            }

            if (this->get<TimeInfectedSymptoms>()[i] < tol_times) {
                log_warning("Constraint check: Parameter TimeInfectedSymptoms changed from {} to {}. Please "
                            "note that unreasonably small compartment stays lead to massively increased run time. "
                            "Consider to cancel and reset parameters.",
                            this->get<TimeInfectedSymptoms>()[i], tol_times);
                this->get<TimeInfectedSymptoms>()[i] = tol_times;
                corrected                            = true;
            }

            if (this->get<TimeInfectedSevere>()[i] < tol_times) {
                log_warning("Constraint check: Parameter TimeInfectedSevere changed from {} to {}. Please "
                            "note that unreasonably small compartment stays lead to massively increased run time. "
                            "Consider to cancel and reset parameters.",
                            this->get<TimeInfectedSevere>()[i], tol_times);
                this->get<TimeInfectedSevere>()[i] = tol_times;
                corrected                          = true;
            }

            if (this->get<TimeInfectedCritical>()[i] < tol_times) {
                log_warning("Constraint check: Parameter TimeInfectedCritical changed from {} to {}. Please "
                            "note that unreasonably small compartment stays lead to massively increased run time. "
                            "Consider to cancel and reset parameters.",
                            this->get<TimeInfectedCritical>()[i], tol_times);
                this->get<TimeInfectedCritical>()[i] = tol_times;
                corrected                            = true;
            }

            if (this->get<TransmissionProbabilityOnContact>()[i] < 0.0 ||
                this->get<TransmissionProbabilityOnContact>()[i] > 1.0) {
                log_warning("Constraint check: Parameter TransmissionProbabilityOnContact changed from {} to {} ",
                            this->get<TransmissionProbabilityOnContact>()[i], 0.0);
                this->get<TransmissionProbabilityOnContact>()[i] = 0.0;
                corrected                                        = true;
            }

            if (this->get<RelativeTransmissionNoSymptoms>()[i] < 0.0) {
                log_warning("Constraint check: Parameter RelativeTransmissionNoSymptoms changed from {} to {} ",
                            this->get<RelativeTransmissionNoSymptoms>()[i], 0);
                this->get<RelativeTransmissionNoSymptoms>()[i] = 0;
                corrected                                      = true;
            }

            if (this->get<RecoveredPerInfectedNoSymptoms>()[i] < 0.0 ||
                this->get<RecoveredPerInfectedNoSymptoms>()[i] > 1.0) {
                log_warning("Constraint check: Parameter RecoveredPerInfectedNoSymptoms changed from {} to {} ",
                            this->get<RecoveredPerInfectedNoSymptoms>()[i], 0);
                this->get<RecoveredPerInfectedNoSymptoms>()[i] = 0;
                corrected                                      = true;
            }

            if (this->get<RiskOfInfectionFromSymptomatic>()[i] < 0.0 ||
                this->get<RiskOfInfectionFromSymptomatic>()[i] > 1.0) {
                log_warning("Constraint check: Parameter RiskOfInfectionFromSymptomatic changed from {} to {}",
                            this->get<RiskOfInfectionFromSymptomatic>()[i], 0);
                this->get<RiskOfInfectionFromSymptomatic>()[i] = 0;
                corrected                                      = true;
            }

            if (this->get<SeverePerInfectedSymptoms>()[i] < 0.0 || this->get<SeverePerInfectedSymptoms>()[i] > 1.0) {
                log_warning("Constraint check: Parameter SeverePerInfectedSymptoms changed from {} to {}",
                            this->get<SeverePerInfectedSymptoms>()[i], 0);
                this->get<SeverePerInfectedSymptoms>()[i] = 0;
                corrected                                 = true;
            }

            if (this->get<CriticalPerSevere>()[i] < 0.0 || this->get<CriticalPerSevere>()[i] > 1.0) {
                log_warning("Constraint check: Parameter CriticalPerSevere changed from {} to {}",
                            this->get<CriticalPerSevere>()[i], 0);
                this->get<CriticalPerSevere>()[i] = 0;
                corrected                         = true;
            }

            if (this->get<DeathsPerCritical>()[i] < 0.0 || this->get<DeathsPerCritical>()[i] > 1.0) {
                log_warning("Constraint check: Parameter DeathsPerCritical changed from {} to {}",
                            this->get<DeathsPerCritical>()[i], 0);
                this->get<DeathsPerCritical>()[i] = 0;
                corrected                         = true;
            }

            if (this->get<DaysUntilEffectivePartialImmunity>()[i] < 0.0) {
                log_warning("Constraint check: Parameter DeathsPerCritical changed from {} to {}",
                            this->get<DaysUntilEffectivePartialImmunity>()[i], 0);
                this->get<DaysUntilEffectivePartialImmunity>()[i] = 0;
                corrected                                         = true;
            }
            if (this->get<DaysUntilEffectiveImprovedImmunity>()[i] < 0.0) {
                log_warning("Constraint check: Parameter DaysUntilEffectiveImprovedImmunity changed from {} to {}",
                            this->get<DaysUntilEffectiveImprovedImmunity>()[i], 0);
                this->get<DaysUntilEffectiveImprovedImmunity>()[i] = 0;
                corrected                                          = true;
            }

            if (this->get<ReducExposedPartialImmunity>()[i] <= 0.0 ||
                this->get<ReducExposedPartialImmunity>()[i] > 1.0) {
                log_warning("Constraint check: Parameter ReducExposedPartialImmunity changed from {} to {}",
                            this->get<ReducExposedPartialImmunity>()[i], 1);
                this->get<ReducExposedPartialImmunity>()[i] = 1;
                corrected                                   = true;
            }
            if (this->get<ReducExposedImprovedImmunity>()[i] <= 0.0 ||
                this->get<ReducExposedImprovedImmunity>()[i] > 1.0) {
                log_warning("Constraint check: Parameter ReducExposedImprovedImmunity changed from {} to {}",
                            this->get<ReducExposedImprovedImmunity>()[i], 1);
                this->get<ReducExposedImprovedImmunity>()[i] = 1;
                corrected                                    = true;
            }
            if (this->get<ReducInfectedSymptomsPartialImmunity>()[i] <= 0.0 ||
                this->get<ReducInfectedSymptomsPartialImmunity>()[i] > 1.0) {
                log_warning("Constraint check: Parameter ReducInfectedSymptomsPartialImmunity changed from {} to {}",
                            this->get<ReducInfectedSymptomsPartialImmunity>()[i], 1);
                this->get<ReducInfectedSymptomsPartialImmunity>()[i] = 1;
                corrected                                            = true;
            }
            if (this->get<ReducInfectedSymptomsImprovedImmunity>()[i] <= 0.0 ||
                this->get<ReducInfectedSymptomsImprovedImmunity>()[i] > 1.0) {
                log_warning("Constraint check: Parameter ReducInfectedSymptomsImprovedImmunity changed from {} to {}",
                            this->get<ReducInfectedSymptomsImprovedImmunity>()[i], 1.0);
                this->get<ReducInfectedSymptomsImprovedImmunity>()[i] = 1.0;
                corrected                                             = true;
            }
            if (this->get<ReducInfectedSevereCriticalDeadPartialImmunity>()[i] <= 0.0 ||
                this->get<ReducInfectedSevereCriticalDeadPartialImmunity>()[i] > 1.0) {
                log_warning("Constraint check: Parameter ReducInfectedSevereCriticalDeadPartialImmunity changed from "
                            "{} to {}",
                            this->get<ReducInfectedSevereCriticalDeadPartialImmunity>()[i], 1.0);
                this->get<ReducInfectedSevereCriticalDeadPartialImmunity>()[i] = 1.0;
                corrected                                                      = true;
            }
            if (this->get<ReducInfectedSevereCriticalDeadImprovedImmunity>()[i] <= 0.0 ||
                this->get<ReducInfectedSevereCriticalDeadImprovedImmunity>()[i] > 1.0) {
                log_warning("Constraint check: Parameter ReducInfectedSevereCriticalDeadImprovedImmunity changed from "
                            "{} to {}",
                            this->get<ReducInfectedSevereCriticalDeadImprovedImmunity>()[i], 1.0);
                this->get<ReducInfectedSevereCriticalDeadImprovedImmunity>()[i] = 1.0;
                corrected                                                       = true;
            }
            if (this->get<ReducTimeInfectedMild>()[i] <= 0.0 || this->get<ReducTimeInfectedMild>()[i] > 1.0) {
                log_warning("Constraint check: Parameter ReducTimeInfectedMild changed from {} to {}",
                            this->get<ReducTimeInfectedMild>()[i], 1.0);
                this->get<ReducTimeInfectedMild>()[i] = 1.0;
                corrected                             = true;
            }
            if (this->get<InfectiousnessNewVariant>()[i] < 0.0) {
                log_warning("Constraint check: Parameter InfectiousnessNewVariant changed from {} to {}",
                            this->get<InfectiousnessNewVariant>()[i], 1.0);
                this->get<InfectiousnessNewVariant>()[i] = 1.0;
                corrected                                = true;
            }
            if (this->get<VaccinationGap>()[i] < 0.0) {
                log_warning("Constraint check: Parameter VaccinationGap changed from {} to {}",
                            this->get<VaccinationGap>()[i], 0);
                this->get<VaccinationGap>()[i] = 0;
                corrected                      = true;
            }
        }
        return corrected;
    }

    /**
     * @brief Checks whether all Parameters satisfy their corresponding constraints and logs an error 
     * if constraints are not satisfied.
     * @return Returns true if one constraint is not satisfied, otherwise false.    
     */
    bool check_constraints() const
    {
        const double tol_times = 1e-1; // accepted tolerance for compartment stays
        if (this->get<Seasonality>() < 0.0 || this->get<Seasonality>() > 0.5) {
            log_error("Constraint check: Parameter m_seasonality smaller {} or larger {}", 0, 0.5);
            return true;
        }

        if (this->get<ICUCapacity>() < 0.0) {
            log_error("Constraint check: Parameter m_icu_capacity smaller {}", 0);
            return true;
        }

        for (auto i = AgeGroup(0); i < AgeGroup(m_num_groups); ++i) {

            if (this->get<TimeExposed>()[i] < tol_times) {
                log_error("Constraint check: Parameter TimeExposed {:.4f} smaller {:.4f}. Please "
                          "note that unreasonably small compartment stays lead to massively increased run time. "
                          "Consider to cancel and reset parameters.",
                          this->get<TimeExposed>()[i], tol_times);
                return true;
            }

            if (this->get<TimeInfectedNoSymptoms>()[i] < tol_times) {
                log_error("Constraint check: Parameter TimeInfectedNoSymptoms {:.4f} smaller {:.4f}. Please "
                          "note that unreasonably small compartment stays lead to massively increased run time. "
                          "Consider to cancel and reset parameters.",
                          this->get<TimeInfectedNoSymptoms>()[i], tol_times);
                return true;
            }

            if (this->get<TimeInfectedSymptoms>()[i] < tol_times) {
                log_error("Constraint check: Parameter TimeInfectedSymptoms {} smaller {}. Please "
                          "note that unreasonably small compartment stays lead to massively increased run time. "
                          "Consider to cancel and reset parameters.",
                          this->get<TimeInfectedSymptoms>()[i], tol_times);
                return true;
            }

            if (this->get<TimeInfectedSevere>()[i] < tol_times) {
                log_error("Constraint check: Parameter TimeInfectedSevere {} smaller {}. Please "
                          "note that unreasonably small compartment stays lead to massively increased run time. "
                          "Consider to cancel and reset parameters.",
                          this->get<TimeInfectedSevere>()[i], tol_times);
                return true;
            }

            if (this->get<TimeInfectedCritical>()[i] < tol_times) {
                log_error("Constraint check: Parameter TimeInfectedCritical {} smaller {}. Please "
                          "note that unreasonably small compartment stays lead to massively increased run time. "
                          "Consider to cancel and reset parameters.",
                          this->get<TimeInfectedCritical>()[i], tol_times);
                return true;
            }

            if (this->get<TransmissionProbabilityOnContact>()[i] < 0.0 ||
                this->get<TransmissionProbabilityOnContact>()[i] > 1.0) {
                log_error("Constraint check: Parameter TransmissionProbabilityOnContact smaller {} or larger {}", 0, 1);
                return true;
            }

            if (this->get<RelativeTransmissionNoSymptoms>()[i] < 0.0) {
                log_error("Constraint check: Parameter RelativeTransmissionNoSymptoms smaller {}", 0);
                return true;
            }

            if (this->get<RecoveredPerInfectedNoSymptoms>()[i] < 0.0 ||
                this->get<RecoveredPerInfectedNoSymptoms>()[i] > 1.0) {
                log_error("Constraint check: Parameter RecoveredPerInfectedNoSymptoms smaller {} or larger {}", 0, 1);
                return true;
            }

            if (this->get<RiskOfInfectionFromSymptomatic>()[i] < 0.0 ||
                this->get<RiskOfInfectionFromSymptomatic>()[i] > 1.0) {
                log_error("Constraint check: Parameter RiskOfInfectionFromSymptomatic smaller {} or larger {}", 0, 1);
                return true;
            }

            if (this->get<SeverePerInfectedSymptoms>()[i] < 0.0 || this->get<SeverePerInfectedSymptoms>()[i] > 1.0) {
                log_error("Constraint check: Parameter SeverePerInfectedSymptoms smaller {} or larger {}", 0, 1);
                return true;
            }

            if (this->get<CriticalPerSevere>()[i] < 0.0 || this->get<CriticalPerSevere>()[i] > 1.0) {
                log_error("Constraint check: Parameter CriticalPerSevere smaller {} or larger {}", 0, 1);
                return true;
            }

            if (this->get<DeathsPerCritical>()[i] < 0.0 || this->get<DeathsPerCritical>()[i] > 1.0) {
                log_error("Constraint check: Parameter DeathsPerCritical smaller {} or larger {}", 0, 1);
                return true;
            }

            if (this->get<VaccinationGap>()[i] < 1) {
                log_error("Constraint check: Parameter VaccinationGap smaller {}", 1);
                return true;
            }

            if (this->get<DaysUntilEffectivePartialImmunity>()[i] < 0.0) {
                log_error("Constraint check: Parameter DaysUntilEffectivePartialImmunity smaller {}", 0);
                return true;
            }
            if (this->get<DaysUntilEffectiveImprovedImmunity>()[i] < 0.0) {
                log_error("Constraint check: Parameter DaysUntilEffectiveImprovedImmunity smaller {}", 0);
                return true;
            }

            if (this->get<ReducExposedPartialImmunity>()[i] <= 0.0 ||
                this->get<ReducExposedPartialImmunity>()[i] > 1.0) {
                log_error("Constraint check: Parameter ReducExposedPartialImmunity smaller {} or larger {}", 0, 1);
                return true;
            }
            if (this->get<ReducExposedImprovedImmunity>()[i] <= 0.0 ||
                this->get<ReducExposedImprovedImmunity>()[i] > 1.0) {
                log_error("Constraint check: Parameter ReducExposedImprovedImmunity smaller {} or larger {}", 0, 1);
                return true;
            }
            if (this->get<ReducInfectedSymptomsPartialImmunity>()[i] <= 0.0 ||
                this->get<ReducInfectedSymptomsPartialImmunity>()[i] > 1.0) {
                log_error("Constraint check: Parameter ReducInfectedSymptomsPartialImmunity smaller {} or larger {}", 0,
                          1);
                return true;
            }
            if (this->get<ReducInfectedSymptomsImprovedImmunity>()[i] <= 0.0 ||
                this->get<ReducInfectedSymptomsImprovedImmunity>()[i] > 1.0) {
                log_error("Constraint check: Parameter ReducInfectedSymptomsImprovedImmunity smaller {} or larger {}",
                          0, 1);
                return true;
            }
            if (this->get<ReducInfectedSevereCriticalDeadPartialImmunity>()[i] <= 0.0 ||
                this->get<ReducInfectedSevereCriticalDeadPartialImmunity>()[i] > 1.0) {
                log_error("Constraint check: Parameter ReducInfectedSevereCriticalDeadPartialImmunity smaller {} or "
                          "larger {}",
                          0, 1);
                return true;
            }
            if (this->get<ReducInfectedSevereCriticalDeadImprovedImmunity>()[i] <= 0.0 ||
                this->get<ReducInfectedSevereCriticalDeadImprovedImmunity>()[i] > 1.0) {
                log_error("Constraint check: Parameter ReducInfectedSevereCriticalDeadImprovedImmunity smaller {} or "
                          "larger {}",
                          0, 1);
                return true;
            }
            if (this->get<ReducTimeInfectedMild>()[i] <= 0.0 || this->get<ReducTimeInfectedMild>()[i] > 1.0) {
                log_error("Constraint check: Parameter ReducTimeInfectedMild smaller {} or larger {}", 0, 1);
                return true;
            }
            if (this->get<InfectiousnessNewVariant>()[i] < 0.0) {
                log_error("Constraint check: Parameter InfectiousnessNewVariant smaller {}", 0);
                return true;
            }
        }
        return false;
    }

private:
    Parameters(ParametersBase&& base)
        : ParametersBase(std::move(base))
        , m_num_groups(get<ContactPatterns>().get_cont_freq_mat().get_num_groups())
    {
    }

public:
    /**
     * deserialize an object of this class.
     * @see epi::deserialize
     */
    template <class IOContext>
    static IOResult<Parameters> deserialize(IOContext& io)
    {
        BOOST_OUTCOME_TRY(base, ParametersBase::deserialize(io));
        return success(Parameters(std::move(base)));
    }

private:
    AgeGroup m_num_groups;
    double m_commuter_nondetection    = 0.0;
    double m_start_commuter_detection = 0.0;
    double m_end_commuter_detection   = 0.0;
    double m_end_dynamic_npis         = 0.0;
};

} // namespace osecirvvs
} // namespace mio

#endif // ODESECIRVVS_PARAMETERS_H

/* 
* Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
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
#ifndef SECIRV_INFECTIONSTATE_H
#define SECIRV_INFECTIONSTATE_H
namespace mio
{
namespace secirv
{

    /**
    * @brief The InfectionState enum describes the possible
    * categories for the infectious state of persons.
    * Usable as an index.
    */
    enum class InfectionState
    {
        Susceptible = 0,
        SusceptiblePartiallyImmune, 
        //SusceptibleFullyImmune == Recovered
        Exposed,
        ExposedPartiallyImmune, //vaccinated once
        ExposedFullyImmune, //vaccinated twice or recovered
        Carrier,
        CarrierPartiallyImmune,
        CarrierFullyImmune ,
        CarrierTested, //tested in the zero-covid scheme during commute
        CarrierPartiallyImmuneTested,
        CarrierFullyImmuneTested,
        Infected,
        InfectedPartiallyImmune,
        InfectedFullyImmune,
        InfectedTested,
        InfectedPartiallyImmuneTested,
        InfectedFullyImmuneTested,
        Hospitalized,
        HospitalizedPartiallyImmune,
        HospitalizedFullyImmune,
        ICU,
        ICUPartiallyImmune,
        ICUFullyImmune,
        Recovered,
        Dead,
        InfTotal,
        
        Count
    };

} // namespace secirv
} // namespace mio

#endif //SECIRV_INFECTIONSTATE_H

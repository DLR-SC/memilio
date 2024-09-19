#############################################################################
# Copyright (C) 2020-2024 MEmilio
#
# Authors: Henrik Zunker
#
# Contact: Martin J. Kuehn <Martin.Kuehn@DLR.de>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#############################################################################
import numpy as np
import datetime
import os
import memilio.simulation as mio
import memilio.simulation.osecir as osecir
from memilio.simulation import AgeGroup

from enum import Enum
from memilio.simulation.osecir import (Model, Simulation,
                                       interpolate_simulation_result)


class Location(Enum):
    Home = 0
    School = 1
    Work = 2
    Other = 3


class Simulation:

    def __init__(self, data_dir, start_date, results_dir):
        self.num_groups = 6
        self.data_dir = data_dir
        self.start_date = start_date
        self.results_dir = results_dir
        if not os.path.exists(self.results_dir):
            os.makedirs(self.results_dir)

    def set_covid_parameters(self, model, parameters):
        index = 0
        for i in range(self.num_groups):
            # Compartment transition duration
            model.parameters.TimeExposed[AgeGroup(i)] = parameters[index]
            index += 1
            model.parameters.TimeInfectedNoSymptoms[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.TimeInfectedSymptoms[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.TimeInfectedSevere[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.TimeInfectedCritical[mio.AgeGroup(
                i)] = parameters[index]
            index += 1

            # Compartment transition probabilities
            model.parameters.RelativeTransmissionNoSymptoms[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.TransmissionProbabilityOnContact[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.RecoveredPerInfectedNoSymptoms[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.RiskOfInfectionFromSymptomatic[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.SeverePerInfectedSymptoms[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.CriticalPerSevere[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.DeathsPerCritical[mio.AgeGroup(
                i)] = parameters[index]
            index += 1
            model.parameters.MaxRiskOfInfectionFromSymptomatic[mio.AgeGroup(
                i)] = parameters[index]
            index += 1

        # Start day is set to the n-th day of the year
        model.parameters.StartDay = self.start_date.timetuple().tm_yday
        model.parameters.Seasonality.value = 0.2

    def set_contact_matrices(self, model):
        contact_matrices = mio.ContactMatrixGroup(
            len(list(Location)), self.num_groups)
        locations = ["home", "school_pf_eig", "work", "other"]

        for i, location in enumerate(locations):
            baseline_file = os.path.join(
                self.data_dir, "contacts", "baseline_" + location + ".txt")
            minimum_file = os.path.join(
                self.data_dir, "contacts", "minimum_" + location + ".txt")
            contact_matrices[i] = mio.ContactMatrix(
                mio.read_mobility_plain(baseline_file),
                mio.read_mobility_plain(minimum_file)
            )
        model.parameters.ContactPatterns.cont_freq_mat = contact_matrices

    def get_graph(self, end_date, parameters):
        model = Model(self.num_groups)
        self.set_covid_parameters(model, parameters)
        self.set_contact_matrices(model)

        graph = osecir.ModelGraph()

        scaling_factor_infected = [2.5, 2.5, 2.5, 2.5, 2.5, 2.5]
        scaling_factor_icu = 1.0
        tnt_capacity_factor = 7.5 / 100000.

        path_population_data = os.path.join(
            self.data_dir, "pydata", "Germany",
            "county_current_population.json")

        mio.osecir.set_nodes(
            model.parameters,
            mio.Date(self.start_date.year,
                     self.start_date.month, self.start_date.day),
            mio.Date(end_date.year,
                     end_date.month, end_date.day), self.data_dir,
            path_population_data, True, graph, scaling_factor_infected,
            scaling_factor_icu, tnt_capacity_factor, 0, False)

        mio.osecir.set_edges(
            self.data_dir, graph, len(Location))

        # save graph
        # path_graph = os.path.join(self.results_dir, "graph")
        # if not os.path.exists(path_graph):
        #     os.makedirs(path_graph)
        # osecir.write_graph(graph, path_graph)

        return graph

    def simulator(self, graph, parameters, days_to_simulate):
        t0 = 0.0
        model_new_params = Model(self.num_groups)
        self.set_covid_parameters(model_new_params, parameters)
        for i in range(graph.num_nodes):
            graph.get_node(i).property.parameters = model_new_params.parameters
        study = osecir.ParameterStudyFlow(
            graph, t0, days_to_simulate, 0.5, 1)
        ensemble = study.run()
        # TODO: Implement interpolation for Flow & Compartmental Results in C++
        results = {}
        for i in range(graph.num_nodes):
            results[ensemble[0].get_node(i).id] = {
                "flows": interpolate_simulation_result(
                    ensemble[0].get_node(i).property.flows),
                "result": interpolate_simulation_result(
                    ensemble[0].get_node(i).property.result)
            }

        return results

    def run(self, days_to_simulate, parameters, num_runs):
        mio.set_log_level(mio.LogLevel.Warning)
        end_date = self.start_date + datetime.timedelta(days=days_to_simulate)

        graph = self.get_graph(end_date, parameters)
        results = self.simulator(graph, parameters, days_to_simulate)

        return 0


if __name__ == "__main__":
    file_path = os.path.dirname(os.path.abspath(__file__))
    sim = Simulation(
        data_dir=os.path.join(file_path, "../../../data"),
        start_date=datetime.date(year=2020, month=12, day=12),
        results_dir=os.path.join(file_path, "../../../results_osecir"))
    days_to_simulate = 50
    parameters = [
        3.2, 2.0, 6.0, 12.0, 8.0,  # Compartment transition duration
        # Compartment transition probabilities
        0.67, 1.0, 0.09, 0.25, 0.2, 0.25, 0.3, 0.5
    ] * sim.num_groups
    sim.run(days_to_simulate, parameters, 2)

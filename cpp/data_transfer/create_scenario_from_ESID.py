# TODO
#
# for each scenario we get one scenario.json:
# - read in scenario.json
# - read in model.json (sevirvvs.json)
# - set parameters from scenario.json
#      - from model.json: for each group, check if group is of category AgeGroup and accumulate to find out number of AgeGroups
#      - for each parameterID check corresponding model parameter
#      - then check groupID for corresponding model group
#      - set value(s)
# - set NPIs from scenario.json
#      - for each interventionID check intervention name and description
#      - apply intervention to the model with values
# - create graph model
# - initialize model as usual with data from RKI or other data
# - run simulation
# - post results to ESID backend with new API
#
import json
import time
import numpy as np
from enum import Enum
import os

import memilio.simulation as mio
import memilio.simulation.osecirvvs as osecirvvs
import memilio.plot.createGIF as mp


class Location(Enum):
    Home = 0
    School = 1
    Work = 2
    Other = 3


class Intervention(Enum):
    Home = 0
    SchoolClosure = 1
    HomeOffice = 2
    GatheringBanFacilitiesClosure = 3
    PhysicalDistanceAndMasks = 4
    SeniorAwareness = 5


class InterventionLevel(Enum):
    Main = 0
    PhysicalDistanceAndMasks = 1
    SeniorAwareness = 2
    Holidays = 3


class Simulation:

    def __init__(self, data_dir, results_dir, start_date, run_data_dir):
        self.num_groups = 6
        self.data_dir = data_dir
        self.results_dir = results_dir
        self.start_date = start_date
        self.szenario_data = ""
        self.run_data_dir = run_data_dir
        if not os.path.exists(self.results_dir):
            os.makedirs(self.results_dir)

    def get_parameter_values(self, parameters, parameter_name):
        parameter = next(
            (entry for entry in parameters if entry['name'] == parameter_name), None)
        if parameter:
            min_value = parameter['values'][0]['valueMin']
            max_value = parameter['values'][0]['valueMax']
            return min_value, max_value
        print(f"Parameter {parameter_name} not found in parameters")
        return None, None

    def set_covid_parameters(self, model):
        # read parameters given from the scenario
        parameters = self.scenario_data[0]['modelParameters']

        with open(os.path.join(self.run_data_dir, "parameter_1.json")) as f:
            self.param_dict = json.load(f)

        # Create a mapping from parameterId to name
        id_to_name = {entry['id']: entry['name'] for entry in self.param_dict}

        # Add the corresponding name to each entry in parameters
        # get min, max values for each parameter
        parameter_values = {}
        for parameter in parameters:
            parameter['name'] = id_to_name.get(
                parameter['parameterId'], "Unknown")
            param_name = parameter['name']
            min_value, max_value = self.get_parameter_values(
                parameters, param_name)

            # Store values in the dictionary with dynamically generated keys
            parameter_values[f"{param_name}Min"] = min_value
            parameter_values[f"{param_name}Max"] = max_value

        def array_assign_uniform_distribution(param, min, max, num_groups=6):
            if isinstance(
                    min, (int, float)) and isinstance(
                        max, (int, float)):
                min = min * np.ones(num_groups)
                max = max * np.ones(num_groups)
            elif not (isinstance(min, (list)) and isinstance(
                    max, (list))):
                raise TypeError(
                    "Invalid type for parameter 'min' or 'max. \
                            Expected a scalar or a list. Must be the same for both.")
            for i in range(num_groups):
                param[mio.AgeGroup(i)] = mio.UncertainValue(
                    0.5 * (max[i] + min[i]))
                param[mio.AgeGroup(i)].set_distribution(
                    mio.ParameterDistributionUniform(min[i], max[i]))

        # if the parameter is not found in the scenario data, we use the default values
        # times
        timeExposedMin = parameter_values.get("TimeExposedMin", 2.67)
        timeExposedMax = parameter_values.get("TimeExposedMax", 4.)
        timeInfectedNoSymptomsMin = parameter_values.get(
            "TimeInfectedNoSymptomsMin", 1.2)
        timeInfectedNoSymptomsMax = parameter_values.get(
            "TimeInfectedNoSymptomsMax", 2.53)
        timeInfectedSymptomsMin = parameter_values.get(
            "TimeInfectedSymptomsMin", [
                5.6255, 5.6255, 5.6646, 5.5631, 5.501, 5.465])
        timeInfectedSymptomsMax = parameter_values.get(
            "TimeInfectedSymptomsMax", [
                8.427, 8.427, 8.4684, 8.3139, 8.169, 8.085])
        timeInfectedSevereMin = parameter_values.get(
            "TimeInfectedSevereMin", [
                3.925, 3.925, 4.85, 6.4, 7.2, 9.])
        timeInfectedSevereMax = parameter_values.get(
            "TimeInfectedSevereMax", [6.075, 6.075, 7., 8.7, 9.8, 13.])
        timeInfectedCriticalMin = parameter_values.get(
            "TimeInfectedCriticalMin", [4.95, 4.95, 4.86, 14.14, 14.4, 10.])
        timeInfectedCriticalMax = parameter_values.get(
            "TimeInfectedCriticalMax", [8.95, 8.95, 8.86, 20.58, 19.8, 13.2])

        array_assign_uniform_distribution(
            model.parameters.TimeExposed, timeExposedMin, timeExposedMax)

        array_assign_uniform_distribution(
            model.parameters.TimeInfectedNoSymptoms, timeInfectedNoSymptomsMin,
            timeInfectedNoSymptomsMax)

        array_assign_uniform_distribution(
            model.parameters.TimeInfectedSymptoms, timeInfectedSymptomsMin,
            timeInfectedSymptomsMax)

        array_assign_uniform_distribution(
            model.parameters.TimeInfectedSevere, timeInfectedSevereMin,
            timeInfectedSevereMax)

        array_assign_uniform_distribution(
            model.parameters.TimeInfectedCritical, timeInfectedCriticalMin,
            timeInfectedCriticalMax)

        # probabilities
        variantFactor = 1.4
        transmissionProbabilityOnContactMin = parameter_values.get(
            "TransmissionProbabilityOnContactMin",
            [
                0.02 * variantFactor, 0.05 * variantFactor, 0.05 * variantFactor,
                0.05 * variantFactor, 0.08 * variantFactor, 0.1 * variantFactor
            ]
        )
        transmissionProbabilityOnContactMax = parameter_values.get(
            "TransmissionProbabilityOnContactMax",
            [
                0.04 * variantFactor, 0.07 * variantFactor, 0.07 * variantFactor,
                0.07 * variantFactor, 0.10 * variantFactor, 0.15 * variantFactor
            ]
        )
        relativeTransmissionNoSymptomsMin = parameter_values.get(
            "RelativeTransmissionNoSymptomsMin", 0.5
        )
        relativeTransmissionNoSymptomsMax = parameter_values.get(
            "RelativeTransmissionNoSymptomsMax", 0.5
        )

        # The precise value between Risk* (situation under control) and MaxRisk* (situation not under control)
        # depends on incidence and test and trace capacity
        riskOfInfectionFromSymptomaticMin = parameter_values.get(
            "RiskOfInfectionFromSymptomaticMin", 0.0
        )
        riskOfInfectionFromSymptomaticMax = parameter_values.get(
            "RiskOfInfectionFromSymptomaticMax", 0.2
        )
        maxRiskOfInfectionFromSymptomaticMin = parameter_values.get(
            "MaxRiskOfInfectionFromSymptomaticMin", 0.4
        )
        maxRiskOfInfectionFromSymptomaticMax = parameter_values.get(
            "MaxRiskOfInfectionFromSymptomaticMax", 0.5
        )
        recoveredPerInfectedNoSymptomsMin = parameter_values.get(
            "RecoveredPerInfectedNoSymptomsMin", [
                0.2, 0.2, 0.15, 0.15, 0.15, 0.15]
        )
        recoveredPerInfectedNoSymptomsMax = parameter_values.get(
            "RecoveredPerInfectedNoSymptomsMax", [
                0.3, 0.3, 0.25, 0.25, 0.25, 0.25]
        )
        severePerInfectedSymptomsMin = parameter_values.get(
            "SeverePerInfectedSymptomsMin", [
                0.006, 0.006, 0.015, 0.049, 0.15, 0.20]
        )
        severePerInfectedSymptomsMax = parameter_values.get(
            "SeverePerInfectedSymptomsMax", [
                0.009, 0.009, 0.023, 0.074, 0.18, 0.25]
        )
        criticalPerSevereMin = parameter_values.get(
            "CriticalPerSevereMin", [0.05, 0.05, 0.05, 0.10, 0.25, 0.35]
        )
        criticalPerSevereMax = parameter_values.get(
            "CriticalPerSevereMax", [0.10, 0.10, 0.10, 0.20, 0.35, 0.45]
        )
        deathsPerCriticalMin = parameter_values.get(
            "DeathsPerCriticalMin", [0.00, 0.00, 0.10, 0.10, 0.30, 0.5]
        )
        deathsPerCriticalMax = parameter_values.get(
            "DeathsPerCriticalMax", [0.10, 0.10, 0.18, 0.18, 0.50, 0.7]
        )
        reducExposedPartialImmunityMin = parameter_values.get(
            "ReducExposedPartialImmunityMin", 0.75
        )
        reducExposedPartialImmunityMax = parameter_values.get(
            "ReducExposedPartialImmunityMax", 0.85
        )
        reducExposedImprovedImmunityMin = parameter_values.get(
            "ReducExposedImprovedImmunityMin", 0.281
        )
        reducExposedImprovedImmunityMax = parameter_values.get(
            "ReducExposedImprovedImmunityMax", 0.381
        )
        reducInfectedSymptomsPartialImmunityMin = parameter_values.get(
            "ReducInfectedSymptomsPartialImmunityMin", 0.6
        )
        reducInfectedSymptomsPartialImmunityMax = parameter_values.get(
            "ReducInfectedSymptomsPartialImmunityMax", 0.7
        )
        reducInfectedSymptomsImprovedImmunityMin = parameter_values.get(
            "ReducInfectedSymptomsImprovedImmunityMin", 0.193
        )
        reducInfectedSymptomsImprovedImmunityMax = parameter_values.get(
            "ReducInfectedSymptomsImprovedImmunityMax", 0.293
        )
        reducInfectedSevereCriticalDeadPartialImmunityMin = parameter_values.get(
            "ReducInfectedSevereCriticalDeadPartialImmunityMin", 0.05
        )
        reducInfectedSevereCriticalDeadPartialImmunityMax = parameter_values.get(
            "ReducInfectedSevereCriticalDeadPartialImmunityMax", 0.15
        )
        reducInfectedSevereCriticalDeadImprovedImmunityMin = parameter_values.get(
            "ReducInfectedSevereCriticalDeadImprovedImmunityMin", 0.041
        )
        reducInfectedSevereCriticalDeadImprovedImmunityMax = parameter_values.get(
            "ReducInfectedSevereCriticalDeadImprovedImmunityMax", 0.141
        )
        reducTimeInfectedMild = parameter_values.get(
            "ReducTimeInfectedMild", 1.0)

        array_assign_uniform_distribution(
            model.parameters.TransmissionProbabilityOnContact,
            transmissionProbabilityOnContactMin,
            transmissionProbabilityOnContactMax)

        array_assign_uniform_distribution(
            model.parameters.RelativeTransmissionNoSymptoms,
            relativeTransmissionNoSymptomsMin,
            relativeTransmissionNoSymptomsMax)

        array_assign_uniform_distribution(
            model.parameters.RiskOfInfectionFromSymptomatic,
            riskOfInfectionFromSymptomaticMin,
            riskOfInfectionFromSymptomaticMax)

        array_assign_uniform_distribution(
            model.parameters.MaxRiskOfInfectionFromSymptomatic,
            maxRiskOfInfectionFromSymptomaticMin,
            maxRiskOfInfectionFromSymptomaticMax)

        array_assign_uniform_distribution(
            model.parameters.RecoveredPerInfectedNoSymptoms,
            recoveredPerInfectedNoSymptomsMin,
            recoveredPerInfectedNoSymptomsMax)

        array_assign_uniform_distribution(
            model.parameters.SeverePerInfectedSymptoms,
            severePerInfectedSymptomsMin,
            severePerInfectedSymptomsMax)

        array_assign_uniform_distribution(
            model.parameters.CriticalPerSevere,
            criticalPerSevereMin,
            criticalPerSevereMax)

        array_assign_uniform_distribution(
            model.parameters.DeathsPerCritical,
            deathsPerCriticalMin,
            deathsPerCriticalMax)

        array_assign_uniform_distribution(
            model.parameters.ReducExposedPartialImmunity,
            reducExposedPartialImmunityMin,
            reducExposedPartialImmunityMax)

        array_assign_uniform_distribution(
            model.parameters.ReducExposedImprovedImmunity,
            reducExposedImprovedImmunityMin,
            reducExposedImprovedImmunityMax)

        array_assign_uniform_distribution(
            model.parameters.ReducInfectedSymptomsPartialImmunity,
            reducInfectedSymptomsPartialImmunityMin,
            reducInfectedSymptomsPartialImmunityMax)

        array_assign_uniform_distribution(
            model.parameters.ReducInfectedSymptomsImprovedImmunity,
            reducInfectedSymptomsImprovedImmunityMin,
            reducInfectedSymptomsImprovedImmunityMax)

        array_assign_uniform_distribution(
            model.parameters.ReducInfectedSevereCriticalDeadPartialImmunity,
            reducInfectedSevereCriticalDeadPartialImmunityMin,
            reducInfectedSevereCriticalDeadPartialImmunityMax)

        array_assign_uniform_distribution(
            model.parameters.ReducInfectedSevereCriticalDeadImprovedImmunity,
            reducInfectedSevereCriticalDeadImprovedImmunityMin,
            reducInfectedSevereCriticalDeadImprovedImmunityMax)

        array_assign_uniform_distribution(
            model.parameters.ReducTimeInfectedMild,
            reducTimeInfectedMild,
            reducTimeInfectedMild)

        # start day is set to the n-th day of the year
        model.parameters.StartDay = parameter_values.get(
            "StartDay", self.start_date.day_in_year
        )

        model.parameters.Seasonality = mio.UncertainValue(
            0.5 * (parameter_values.get(
                "SeasonalityMax", 0.3
            ) + parameter_values.get(
                "SeasonalityMin", 0.1
            )))
        model.parameters.Seasonality.set_distribution(
            mio.ParameterDistributionUniform(parameter_values.get(
                "SeasonalityMin", 0.1
            ), parameter_values.get(
                "SeasonalityMax", 0.3
            )))

        model.parameters.StartDayNewVariant = parameter_values.get(
            "StartDayNewVariant", mio.Date(2021, 6, 6).day_in_year
        )

    def set_contact_matrices(self, model):
        contact_matrices = mio.ContactMatrixGroup(
            len(list(Location)), self.num_groups)
        locations = ["home", "school_pf_eig", "work", "other"]

        for i, location in enumerate(locations):
            baseline_file = os.path.join(
                self.data_dir, "contacts", "baseline_" + location + ".txt")
            contact_matrices[i] = mio.ContactMatrix(
                mio.read_mobility_plain(baseline_file),
                np.zeros((self.num_groups, self.num_groups))
            )
        model.parameters.ContactPatterns.cont_freq_mat = contact_matrices

    def get_graph(self, end_date):
        model = osecirvvs.Model(self.num_groups)
        self.set_covid_parameters(model)
        self.set_contact_matrices(model)

        graph = osecirvvs.ModelGraph()

        scaling_factor_infected = np.ones(self.num_groups)
        scaling_factor_icu = 1.0
        tnt_capacity_factor = 1.43 / 100000.

        path_population_data = os.path.join(
            self.data_dir, "pydata", "Germany",
            "county_current_population.json")

        osecirvvs.set_nodes(
            model.parameters,
            self.start_date,
            end_date, self.data_dir,
            path_population_data, True, graph, scaling_factor_infected,
            scaling_factor_icu, tnt_capacity_factor, end_date - self.start_date, False)

        osecirvvs.set_edges(
            self.data_dir, graph, len(Location))

        return graph

    def run(self, num_days_sim, num_runs=10):
        mio.set_log_level(mio.LogLevel.Warning)
        end_date = self.start_date + num_days_sim

        # list all files in dir self.run_data_dir and filter for scenario_*
        scenario_files = [file for file in os.listdir(
            self.run_data_dir) if file.startswith("scenario_")]

        # TODO: Assuming all parameters are equal for each scenarios.
        # Therefore, we only need to build the graph once?

        for scenario_file in scenario_files:
            szenario_data_path = os.path.join(self.run_data_dir, scenario_file)
            with open(szenario_data_path) as f:
                self.scenario_data = json.load(f)

            graph = self.get_graph(end_date)

            study = osecirvvs.ParameterStudy(
                graph, 0., num_days_sim, 0.5, num_runs)
            ensemble = study.run(self.high)

            ensemble_results = []
            ensemble_params = []
            for run in range(num_runs):
                graph_run = ensemble[run]
                ensemble_results.append(
                    osecirvvs.interpolate_simulation_result(graph_run))
                ensemble_params.append(
                    [graph_run.get_node(node_indx).property.model
                     for node_indx in range(graph.num_nodes)])

            node_ids = [graph.get_node(i).id for i in range(graph.num_nodes)]

            save_percentiles = True
            save_single_runs = False

            osecirvvs.save_results(
                ensemble_results, ensemble_params, node_ids, self.results_dir,
                save_single_runs, save_percentiles)

            # update timestamp in the scenario file
            with open(szenario_data_path, 'r') as f:
                data = json.load(f)
                data[0]['timestamp'] = time.strftime("%Y-%m-%dT%H:%M:%S.%fZ")


if __name__ == "__main__":
    cwd = os.getcwd()
    start_date = mio.Date(2022, 6, 1)
    run_data_dir = '/localdata1/revised_paper/memilio/cpp/data_transfer/Scenario_creation/'
    # with open(scenario_data_path) as f:
    #     scenario_data = json.load(f)
    sim = Simulation(
        data_dir=os.path.join(cwd, "data"),
        results_dir=os.path.join(cwd, "results_osecirvvs"), start_date=start_date, run_data_dir=run_data_dir)
    sim.run(num_days_sim=30, num_runs=5)

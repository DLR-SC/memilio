#############################################################################
# Copyright (C) 2020-2022 German Aerospace Center (DLR-SC)
#
# Authors: Maximilian Betz
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
"""
@file generator.py
@brief Generates the model specific python bindings code with the information given by the intermediate represantation.
"""
import string
import os
from typing_extensions import Self
from memilio.generation.intermediate_representation import IntermediateRepresentation
import memilio.generation.template.template_string as StringTemplates

class Generator:
    def __init__(self: Self) -> None:
        self.substitutions_py = {}
        self.substitutions_cpp = {}

    def create_substitutions(self: Self, intermed_repr: IntermediateRepresentation) -> None:
        # create the substitutions with a given intermed_repr

        # substititutions for the py-file
        self.substitutions_py = {}
        self.substitutions_py["python_module_name"] = intermed_repr.python_module_name

        # substititutions for the cpp-file
        self.substitutions_cpp = {}
    
        self.substitutions_cpp["namespace"] = intermed_repr.namespace
        self.substitutions_cpp["model_class_name"] = intermed_repr.model_class
        self.substitutions_cpp["model_base"] = intermed_repr.model_base[0]
        self.substitutions_cpp["model_base_templates"] = intermed_repr.model_base[1][0] + ", " + intermed_repr.model_base[2][0] + ", " + intermed_repr.model_base[3][0]
        self.substitutions_cpp["python_module_name"] = intermed_repr.python_module_name
        self.substitutions_cpp["parameterset"] = intermed_repr.parameterset
        
        self.substitutions_cpp["includes"] = StringTemplates.includes(intermed_repr)
        self.substitutions_cpp["pretty_name_function"] = StringTemplates.pretty_name_function(intermed_repr)
        self.substitutions_cpp["population_enums"] = StringTemplates.population_enums(intermed_repr)
        self.substitutions_cpp["model_init"] = StringTemplates.model_init(intermed_repr)
        self.substitutions_cpp["population"] = StringTemplates.population(intermed_repr)

        # optional substitution strings for model with agegroups
        self.substitutions_cpp["parameterset_indexing"] = StringTemplates.parameterset_indexing(intermed_repr)
        self.substitutions_cpp["parameterset_wrapper"] = StringTemplates.parameterset_wrapper(intermed_repr)
        self.substitutions_cpp["age_group"] = StringTemplates.age_group(intermed_repr)

        # optional substitution strings for model with simulation class
        self.substitutions_cpp["simulation"] = StringTemplates.simulation(intermed_repr)
        self.substitutions_cpp["simulation_graph"] = StringTemplates.simulation_graph(intermed_repr)
        self.substitutions_cpp["simulation_vector_definition"] = StringTemplates.simulation_vector_definition(intermed_repr)


    def generate_files(self: Self, intermed_repr: IntermediateRepresentation) -> None:
        with open(os.path.join(intermed_repr.project_path + "/pycode/memilio-generation/memilio/generation/template/template_py.txt")) as t:
            template_py = string.Template(t.read())
        with open(os.path.join(intermed_repr.project_path + "/pycode/memilio-generation/memilio/generation/template/template_cpp.txt")) as t:
            template_cpp = string.Template(t.read())
        
        output_py = template_py.safe_substitute(**self.substitutions_py)
        output_cpp = template_cpp.safe_substitute(**self.substitutions_cpp)

        py_filename = "/" + intermed_repr.python_module_name + ".py"
        cpp_filename = "/" + intermed_repr.python_module_name + ".cpp"
        with open(os.path.join(intermed_repr.target_folder + py_filename), "w") as output:
            output.write(output_py)
        with open(os.path.join(intermed_repr.target_folder + cpp_filename), "w") as output:
            output.write(output_cpp)

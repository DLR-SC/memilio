import os
import subprocess
import sys
import importlib.util
import pkgutil
import memilio.simulation

setup_content = f"""
from setuptools import setup, find_packages

setup(
    name='memilio-stubs',
    version='0.1',
    packages=['memilio-stubs'],
    package_data={{
        'memilio-stubs': ['simulation/*.pyi'],
    }},
)
"""

if __name__ == "__main__":

    python_interpreter = sys.executable

    # Check for needed packages. If it fails either pacakge is not installed or the wrong python interpreter is detected.
    # For the latter try setting python_interpreter with full path
    if importlib.util.find_spec('pybind11_stubgen') is None:
        print('pybind11_stubgen is not installed')
        exit()
    if importlib.util.find_spec('memilio.simulation') is None:
        print('memilio.simulation is not installed')
        exit()

    file_path = os.path.dirname(os.path.abspath(__file__))
    package_dir = os.path.abspath(os.path.join(
        file_path, "../../memilio-simulation-stubs"))
    output_dir = os.path.join(package_dir, "memilio-stubs/")
    output_module_dir = os.path.join(output_dir, 'memilio')

    # create folders, if they do not exist
    try:
        # output_module_dir is generated by pybind11_stubgen
        os.makedirs(output_dir)
    except:
        pass

    # get all model modules from memilio.simulation
    # if package structure changes this needs to be adjusted
    # models = [m.name for m in pkgutil.iter_modules(
    #     memilio.simulation.__path__)]

    # generate stubs and moce them into correct folder with right name
    # memilio-stubs/simulation module needs same structure as memilio/simulation
    subprocess.check_call(
        [python_interpreter, '-m', 'pybind11_stubgen', '--ignore-all-errors', '-o', output_dir, 'memilio.simulation'])
    # os.rename(os.path.join(output_module_dir, 'simulation.pyi'),
    #           os.path.join(output_dir, '__init__.pyi'))

    # for model in models:
    #     module_name = "memilio._simulation_" + model
    #     subprocess.check_call(
    #         [python_interpreter, '-m', 'pybind11_stubgen', '--ignore-all-errors', '-o', output_dir, module_name])
    #     os.rename(os.path.join(output_module_dir, '_simulation_' + model + '.pyi'),
    #               os.path.join(output_dir, model + '.pyi'))

    # os.rmdir(output_module_dir)

    # create setup.py and install package
    with open(os.path.join(package_dir, "setup.py"), "w") as setup_file:
        setup_file.write(setup_content)
    subprocess.check_call(
        [python_interpreter, '-m', 'pip', 'install', package_dir])

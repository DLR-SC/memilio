import datetime as dt
import os.path
import imageio

import numpy as np
import pandas as pd

import memilio.epidata.getPopulationData as gpd
import memilio.plot.plotMap as pm
from memilio.epidata import geoModificationGermany as geoger
from matplotlib import pyplot as plt
import matplotlib.colors as mcolors
from matplotlib.colors import SymLogNorm, LinearSegmentedColormap
from tqdm.auto import tqdm


def plot_risk_map(path_results, path_plots, days, percentile):
    if not os.path.exists(path_plots):
        os.makedirs(path_plots)
    path_risk_results = os.path.join(path_results, "risk")
    plot_maps(path_risk_results, path_plots, compartments=[0], percentile=percentile,
              days=days, min_val=0, max_val=1, filename="risk_map", relative=False, age_groups={0: '0-4'})


def plot_icu_map(path_results, path_plots, days, percentile):
    if not os.path.exists(path_plots):
        os.makedirs(path_plots)
    plot_maps(path_results, path_plots, compartments=[7], percentile=percentile,
              days=days, min_val=0, max_val=50, filename="icu_map", relative=False)


def create_colorbar(path_plots, norm):
    colors = ["green", "yellow", "red", "purple"]
    cmap = LinearSegmentedColormap.from_list("my_colormap", colors)
    sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
    sm.set_array([])

    # Create a new figure for the colorbar
    cbar_fig, ax = plt.subplots(figsize=(8, 1))
    cbar = plt.colorbar(sm, orientation='horizontal', cax=ax)
    cbar.ax.tick_params(labelsize=10)
    plt.tight_layout()
    plt.savefig(os.path.join(path_plots, 'colorbar.png'), dpi=300)
    plt.clf()


def plot_maps(path_results, path_plots, compartments, percentile, days, min_val, max_val, filename="data", relative=False, age_groups={0: '0-4', 1: '5-14', 2: '15-34',
                                                                                                                                       3: '35-59', 4: '60-79', 5: '80+'}):

    progress_bar = tqdm(total=len(days))

    path = os.path.join(path_results, percentile, "Results")

    get_max_val = 0

    files_input = {
        'Data set 1': path}
    file_format = 'h5'

    if len(age_groups) == 6:
        filter_age = None
    else:
        if file_format == 'json':
            filter_age = [val for val in age_groups.values()]
        else:
            filter_age = ['Group' + str(key + 1)
                          for key in age_groups.keys()]

    norm = SymLogNorm(linthresh=1, linscale=0.7,
                      vmin=min_val, vmax=max_val)
    create_colorbar(path_plots, norm)

    for day in days:
        for file in files_input.values():
            # MEmilio backend hdf5 example

            df = pm.extract_data(
                file, region_spec=None, column=None, date=day,
                filters={'Group': filter_age,
                         'InfectionState': compartments},
                file_format=file_format)

            # all entries to numeric
            df = df.apply(pd.to_numeric, errors='coerce')
            dfs_all = df

            if relative:

                try:
                    population = pd.read_json(
                        'data/pydata/Germany/county_current_population.json')
                # pandas>1.5 raise FileNotFoundError instead of ValueError
                except (ValueError, FileNotFoundError):
                    print(
                        "Population data was not found. Download it from the internet.")
                    population = gpd.get_population_data(
                        read_data=False, file_format=file_format,
                        out_folder='data/pydata/Germany/', no_raw=True,
                        split_gender=False, merge_eisenach=True)

                # For fitting of different age groups we need format ">X".
                age_group_values = list(age_groups.values())
                age_group_values[-1] = age_group_values[-1].replace(
                    '80+', '>79')
                # scale data
                df = pm.scale_dataframe_relative(
                    df, age_group_values, population)

        fn = filename + "_day_" + str(day)

        dfs_all = dfs_all.apply(pd.to_numeric, errors='coerce')

        dfs_all_sorted = dfs_all.sort_values(by='Region')
        dfs_all_sorted = dfs_all_sorted.reset_index(drop=True)

        if dfs_all_sorted['Count'].max() > get_max_val:
            get_max_val = dfs_all_sorted['Count'].max()

        pm.plot_map(norm=norm, data=dfs_all_sorted,
                    scale_colors=[min_val, max_val],
                    legend=['', ''],
                    title='Data day ' + str(day),
                    plot_colorbar=True,
                    output_path=path_plots,
                    fig_name=fn,
                    dpi=300,
                    outercolor='white')

        progress_bar.update()
    progress_bar.close()
    print("max value: ", get_max_val)
    return 0


if __name__ == '__main__':
    path_results = "/localdata1/code_2024/memilio/results"
    path_plots = "/localdata1/code_2024/memilio/plots"
    num_days = 70

    plot_flows = False
    percentile = "p50"
    days = list(range(0, num_days + 1, 10))

    plot_risk_map(path_results, path_plots, days, percentile)
    plot_icu_map(path_results, path_plots, days, percentile)

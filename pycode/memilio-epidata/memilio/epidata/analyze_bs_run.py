# Python script to analyze bs runs
# input is a bs run folder with the following structure:
# bs_run_folder has a txt file for each bs run
# each txt file has a line for each time step
# each line has a column for each compartment as well as the timestep
# each column has the number of individuals in that compartment
# the first line of each txt file is the header

import sys
import argparse
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import matplotlib.colors as colors
import matplotlib.cm as cmx
import matplotlib.patches as mpatches
import matplotlib.lines as mlines
from mpm_plotter import read_from_terminal, plot_populations

def main(n_runs):
    # read in folder and convert txt files to numpy arrays
    # folder path
    folder_path = "memilio/epidata/folder_run_bs"

    # get first_file in folder
    first_file = os.listdir(folder_path)[0]
    file_path = os.path.join(folder_path, first_file)
    # read in txt file
    df = pd.read_csv(file_path, delim_whitespace=True)
    # convert to numpy array
    df_np = df.to_numpy()
    # get the number of rows and columns
    num_rows = df_np.shape[0]
    num_cols = df_np.shape[1]
    # get the number of compartments
    num_compartments = num_cols - 1
    # get the number of time steps
    num_time_steps = num_rows-1
    # get the compartment names
    compartment_names = df.columns[1:]
    # get the time steps
    time_steps = df_np[:, 0]

    # get number of files in folder
    num_files = len([entry for entry in os.listdir(folder_path)])
    # read in each txt file and convert to numpy array
    df_np_3d = np.empty((num_rows, num_cols, n_runs))
    for (file, i) in zip(os.listdir(folder_path), range(n_runs)):
        file_path = os.path.join(folder_path, file)
        # read in txt file
        df = pd.read_csv(file_path, delim_whitespace=True)
        if file.startswith("infection_per_location_type"):
            plot_infection_per_location_type(df)
        if file.startswith("infection_per_age_group"):
            plot_infection_per_age_group(df)
        if file.startswith("abm_result"):
            # convert to numpy array
            df_np = df.to_numpy()
            # attach to array
            df_np_3d[:, :, i] = df_np
            plot_mean_and_std(df_np_3d)


def plot_infection_per_location_type(df):
    df.plot(x='Time', y=['Home', 'Work', 'School', 'SocialEvent', 'BasicsShop', 'Hospital',
            'ICU', 'Car', 'PublicTransport', 'TransportWithoutContact', 'Cemetery'], figsize=(10, 6))
    plt.show()


def plot_infection_per_age_group(df):
    df.plot(x='Time', y=['0_to_4', '5_to_14', '15_to_34',
            '35_to_59', '60_to_79', '80_plus'], figsize=(10, 6))
    plt.show()


def plot_mean_and_std(Y):

    x_plot = Y[:, 0, 0]
    compartments = Y[:,1:,1:]
    # average value
    compartments_avg = np.mean(compartments,axis=2)
    #plot average
    for i in range(compartments_avg.shape[1]):
        plt.plot(x_plot,compartments_avg[:,i])
    
    #plt.plot(x_plot,compartments_avg)
    #legend
    plt.legend(['S', 'E', 'I_NS', 'I_Sy', 'I_Sev', 'I_Crit', 'R', 'D'])
    plt.show()
    # standard deviation
    # compartments_std = np.std(compartments,axis=2)
    # plt.plot(x_plot,compartments_avg + compartments_std)
    # plt.plot(x_plot,compartments_avg - compartments_std)
    # plt.show()


if __name__ == "__main__":
    if (len(sys.argv) > 1):
        n_runs = sys.argv[1]
    else:
        folder_path = "memilio/epidata/folder_run_bs"
        n_runs = len([entry for entry in os.listdir(folder_path)
                     if os.path.isfile(os.path.join(folder_path, entry))])
    main(n_runs)

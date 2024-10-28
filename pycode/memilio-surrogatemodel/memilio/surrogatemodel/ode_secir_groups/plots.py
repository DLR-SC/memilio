import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os
import pickle
import matplotlib.gridspec as gridspec
from memilio.simulation.osecir import InfectionState
import seaborn as sns

filename = 'data_secir_groups_30days_Germany_10k.pickle'

# import data
path = os.path.dirname(os.path.realpath(__file__))
path_data = os.path.join(os.path.dirname(os.path.realpath(
    os.path.dirname(os.path.realpath(path)))), 'data_paper')
# path_data = '/home/schm_a45/Documents/Code/memilio_test/memilio/pycode/memilio-surrogatemodel/memilio/data_paper'

if not os.path.isfile(os.path.join(path_data, filename)):
    ValueError("no dataset found in path: " + path)

file = open(os.path.join(path_data, filename), 'rb')
data = pickle.load(file)

data['inputs'] = np.expm1(np.asarray(data['inputs']))
data['labels'] = np.expm1(np.asarray(data['labels']))


def SINGLE_lineplot_compartments_log_and_nolog_agegroups(inputs_reversed, labels_reversed, num_plots, input_width, label_width):
    num_days = input_width+label_width
    infectionstates = ['Susceptible', 'Exposed', 'InfectedNoSymptoms',
                       'InfectedSymptoms', 'InfectedSevere', 'InfectedCritical', 'Recovered', 'Dead']

    groups = ['0-4', '5-14', '15-34', '35-59', '60-79', '80+']

    # Define different colors for each infection state
    colors = ['blue', 'orange', 'green', 'red',
              'purple', 'brown', 'pink', 'gray']

    # Reshape data to enable plotting by age group
    reshaped_array_input = inputs_reversed.reshape(10000, input_width, 6, 8)
    reshaped_array_labels = labels_reversed.reshape(10000, label_width, 6, 8)

    for i in range(num_plots):
        plt.clf()

        # Create a figure for the two main subplots
        fig = plt.figure(figsize=(20, 10), constrained_layout=True)

        # Define an outer grid with 1 row and 2 columns (for ax1 and ax2)
        outer_grid = gridspec.GridSpec(1, 2, wspace=0.3, hspace=0.3)

        # First main subplot (original data with six nested subplots)
        inner_grid_1 = gridspec.GridSpecFromSubplotSpec(
            3, 2, subplot_spec=outer_grid[0], wspace=0.3, hspace=0.3)
        for group, j, input_pergroup, label_pergroup in zip(groups, np.arange(6), reshaped_array_input[i].transpose(1, 2, 0), reshaped_array_labels[i].transpose(1, 2, 0)):
            ax = fig.add_subplot(inner_grid_1[j])
            for idx, (c, inp, lab) in enumerate(zip(infectionstates, input_pergroup, label_pergroup)):
                # Plot the first 5 days from inp (in a specific color)
                ax.plot(np.arange(1, 6), inp[:5],
                        color=colors[idx], linestyle='dashed')
                # Plot the remaining days from lab (in the same color but dashed)
                ax.plot(np.arange(6, num_days + 1), lab, color=colors[idx])

            ax.set_title(group)

            # Place x-axis labels only on bottom subplots
            if j >= 4:  # Bottom row subplots
                ax.set_xlabel('Number of days')
            # Place y-axis labels only on left subplots
            if j % 2 == 0:  # Left column subplots
                ax.set_ylabel('Number of individuals')

        # Second main subplot (log1p transformed data with six nested subplots)
        inner_grid_2 = gridspec.GridSpecFromSubplotSpec(
            3, 2, subplot_spec=outer_grid[1], wspace=0.3, hspace=0.3)
        for group, j, input_pergroup, label_pergroup in zip(groups, np.arange(6),
                                                            reshaped_array_input[i].transpose(
                                                                1, 2, 0),
                                                            reshaped_array_labels[i].transpose(1, 2, 0)):
            ax = fig.add_subplot(inner_grid_2[j])
            for idx, (c, inp, lab) in enumerate(zip(infectionstates, input_pergroup, label_pergroup)):
                # Apply np.log1p transformation before plotting
                ax.plot(np.arange(1, 6), np.log1p(
                    inp[:5]), color=colors[idx], linestyle='dashed')
                ax.plot(np.arange(6, num_days + 1),
                        np.log1p(lab), color=colors[idx])

            ax.set_title(group)

            # Place x-axis labels only on bottom subplots
            if j >= 4:  # Bottom row subplots
                ax.set_xlabel('Number of days')
            # Place y-axis labels only on left subplots
            if j % 2 == 0:  # Left column subplots
                ax.set_ylabel('log1p(Number of individuals)')

        # Create a single legend for all subplots
        handles = [plt.Line2D([0], [0], color=color, label=state)
                   for color, state in zip(colors, infectionstates)]
        fig.legend(handles=handles, loc='upper center', bbox_to_anchor=(
            0.5, 0.08), ncol=4, title="Compartments")

        # Save the figure
        plt.savefig("/localdata1/gnn_paper_2024/images/lineplots_compartments/with_agegroups_30days_2plt/compartment_lines_withagegroups_30days_2plt_10k_paper_no" + str(i) + ".png")
        print(f'Plot No. {i} done.')


def boxplot_inputs_single(file_path, savename):

    file = open(file_path, 'rb')
    data = pickle.load(file)
    data = np.expm1(data['inputs'])

    reshaped_array_input = data.reshape(10000, 5, 6, 8)
    reshaped_array_input = reshaped_array_input.transpose(2, 0, 1, 3)

    # compartment_array = []
    # for compartment in InfectionState.values():
    #        compartment_array.append(compartment)
    # infectionstates = [str(compartment).split('.')[1] for compartment in compartment_array]
    agegroups = ['0-4', '5-14', '15-34', '35-59', '60-79', '80+']
    for group_input, agegroup in zip(reshaped_array_input, agegroups):

        infectionstates = ['Susceptible', 'Exposed', 'InfectedNoSymptoms',
                           'InfectedSymptoms', 'InfectedSevere', 'InfectedCritical', 'Recovered', 'Dead']

        fig, ((ax1, ax2), (ax3, ax4), (ax5, ax6), (ax7, ax8)) = plt.subplots(
            nrows=4, ncols=2, sharey=False, figsize=(8, 10))
        axes = [ax1, ax2, ax3, ax4, ax5, ax6, ax7, ax8]

        for ax, compartment, d in zip(axes, infectionstates, np.asarray(group_input).transpose()):

            d_df = pd.DataFrame(data=d)
            d_df = pd.melt(d_df.transpose(), var_name="Day")
            d_df['type'] = 'b'

            sns.boxplot(ax=ax, x='Day', y='value', data=d_df,
                        hue='type', palette='Set1', width=0.8, legend='auto')
            ax.set_title(compartment, fontsize=10)
            ax.legend().set_visible(False)

            handles, labels = ax.get_legend_handles_labels()
            fig.legend(handles, labels, loc='upper right', ncol=3,
                       bbox_to_anchor=(0.7, 1), frameon=False)
        plt.tight_layout()

        plt.savefig(
            savename+agegroup+".png")


SINGLE_lineplot_compartments_log_and_nolog_agegroups(
    data['inputs'], data['labels'], 100, input_width=5, label_width=30)


file_path = '/home/schm_a45/Documents/Code/memilio_test/memilio/pycode/memilio-surrogatemodel/memilio/data_paper/data_secir_groups_30days_I_based_Germany_10k.pickle'
savename = 'boxplot_withagegroups_I_based_10k_'
boxplot_inputs_single(file_path, savename)

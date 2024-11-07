import os
import tensorflow as tf
import pickle
from memilio.surrogatemodel.ode_secir_simple.model import split_data, get_test_statistic
# from memilio.surrogatemodel.ode_secir_groups.data_generation_nodamp import get_population
import numpy as np
from memilio.simulation.osecir import InfectionState
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

# load data path = os.path.dirname(os.path.realpath(__file__))

# import data
path = os.path.dirname(os.path.realpath(__file__))
path_data = os.path.join(os.path.dirname(os.path.realpath(
    os.path.dirname(os.path.realpath(path)))), 'data_paper')

filename = "data_secir_simple_30_days_10k.pickle"

if not os.path.isfile(os.path.join(path_data, filename)):
    ValueError("no dataset found in path: " + path_data)

file = open(os.path.join(path_data, filename), 'rb')

data = pickle.load(file)


test_inputs = data['inputs'][int((0.8 * len(data['inputs']))):]
test_labels = data['labels'][int((0.8 * len(data['labels']))):]


# load trained model
# new_model = tf.keras.models.load_model('/home/schm_a45/Documents/code3/memilio/pycode/memilio-surrogatemodel/memilio/saved_models_secir_simple')
new_model = tf.keras.models.load_model(
    '/home/schm_a45/Documents/Code/memilio_test/memilio/pycode/memilio-surrogatemodel/memilio/saved_models_secir_simple_paper/LSTM_30days_secirsimple_10k.h5')

pred = new_model.predict(test_inputs)

pred_reversed = np.expm1(pred)
labels_reversed = np.expm1(np.asarray(test_labels))

mape_per_day = 100*np.mean(abs((test_labels - pred)/test_labels), axis=0)
mape_reversed_per_day = 100 * \
    np.mean(abs((labels_reversed - pred_reversed)/labels_reversed), axis=0)

n_runs = test_labels.shape[0]
n_days = test_labels.shape[1]
n_compartments = test_labels.shape[2]

mape_per_run = 100*np.mean(abs((np.asarray(test_labels).reshape(n_runs, n_days*n_compartments) - np.asarray(pred).reshape(
    n_runs, n_days*n_compartments))/np.asarray(test_labels).reshape(n_runs, n_days*n_compartments)), axis=1)

mape_reversed_per_run = 100 * \
    np.mean(abs((np.asarray(labels_reversed).reshape(n_runs, n_days*n_compartments) - np.asarray(pred_reversed).reshape(
        n_runs, n_days*n_compartments))/np.asarray(labels_reversed).reshape(n_runs, n_days*n_compartments)), axis=1)


def lineplots_compartments(mape_per_day, mape_reversed_per_day, savename):

    infectionstates = ['Susceptible', 'Exposed', 'InfectedNoSymptoms',
                       'InfectedSymptoms', 'InfectedSevere', 'InfectedCritical', 'Recovered', 'Dead']

    plt.clf()
    fig, ((ax1, ax2), (ax3, ax4), (ax5, ax6), (ax7, ax8)) = plt.subplots(
        nrows=4, ncols=2, sharey=False, figsize=(10, 13), constrained_layout=True)

    axes = [ax1, ax2, ax3, ax4, ax5, ax6, ax7, ax8]

    for ax, c, m, mr in zip(axes, infectionstates, mape_per_day.transpose(), mape_reversed_per_day.transpose()):
        # Plot the first 5 days from inp (in blue)
        ax.plot(m, color='blue', label='Test MAPE scaled')

        # Plot the remaining days from lab (in orange) starting from day 6
        ax.plot(mr, color='orange',
                label='Test MAPE not scaled')

        # ax.set_xlabel('Day')
        ax.set_ylabel('Test MAPE')
        ax.set_title(c, fontsize=10)

    ax7.set_xlabel('Day')
    ax8.set_xlabel('Day')

    lines = []
    line_labels = []
    for ax in fig.axes:
        Line, Label = ax.get_legend_handles_labels()
        lines.extend(Line)
        line_labels.extend(Label)

    fig.legend(lines[:2], line_labels[:2], loc='lower center')
    fig.suptitle(
        'Test MAPE for scaled ('+str(np.round(np.mean(mape_per_day), 4))+'%) and unscaled data ('+str(np.round(np.mean(mape_reversed_per_day), 4))+'%)', fontsize=16)

    plt.savefig(savename)


def lineplots_compartments_twoaxes(mape_per_day, mape_reversed_per_day, savename):
    infectionstates = ['Susceptible', 'Exposed', 'InfectedNoSymptoms',
                       'InfectedSymptoms', 'InfectedSevere', 'InfectedCritical', 'Recovered', 'Dead']

    plt.clf()
    fig, ((ax1, ax2), (ax3, ax4), (ax5, ax6), (ax7, ax8)) = plt.subplots(
        nrows=4, ncols=2, sharey=False, figsize=(10, 13), constrained_layout=True)

    axes = [ax1, ax2, ax3, ax4, ax5, ax6, ax7, ax8]

    for ax, c, m, mr in zip(axes, infectionstates, mape_per_day.transpose(), mape_reversed_per_day.transpose()):
        # Plot the first line (mape_per_day) on the primary y-axis (left)
        ax.plot(m, color='blue', label='Test MAPE scaled')
        ax.set_ylabel('Test MAPE (Scaled)', color='blue')
        ax.tick_params(axis='y', labelcolor='blue')

        # Create a secondary y-axis for the second line (mape_reversed_per_day)
        ax2 = ax.twinx()
        ax2.plot(mr, color='orange', label='Test MAPE not scaled')
        ax2.set_ylabel('Test MAPE (Not Scaled)', color='red')
        ax2.tick_params(axis='y', labelcolor='red')

        ax.set_title(c, fontsize=10)

    # Set x-axis labels for the bottom plots
    ax7.set_xlabel('Day')
    ax8.set_xlabel('Day')
    # Collect unique legend information for the combined legend
    lines, labels = [], []
    for ax in fig.axes:
        for line, label in zip(*ax.get_legend_handles_labels()):
            if label not in labels:  # Only add if label is unique
                lines.append(line)
                labels.append(label)

    fig.legend(lines, labels, loc='lower center')
    fig.suptitle(
        'Test MAPE for scaled ('+str(np.round(np.mean(mape_per_day), 4))+'%) and unscaled data ('+str(np.round(np.mean(mape_reversed_per_day), 4))+'%)', fontsize=16)

    plt.savefig(savename)


def lineplots_pred_labels(pred_reversed, labels_reversed, num_plots):

    infectionstates = ['Susceptible', 'Exposed', 'InfectedNoSymptoms',
                       'InfectedSymptoms', 'InfectedSevere', 'InfectedCritical', 'Recovered', 'Dead']

    for i in range(num_plots):

        plt.clf()
        fig, ((ax1, ax2), (ax3, ax4), (ax5, ax6), (ax7, ax8)) = plt.subplots(
            nrows=4, ncols=2, sharey=False, figsize=(10, 13), constrained_layout=True)

        axes = [ax1, ax2, ax3, ax4, ax5, ax6, ax7, ax8]

        for ax, c, p, l in zip(axes, infectionstates, pred_reversed[i].transpose(), labels_reversed[i].transpose()):

            ax.plot(l, color='orange',
                    label='Labels', linewidth=2)

            ax.plot(p, color='blue', label='Predictions',
                    linestyle='--', linewidth=1)

            # ax.set_xlabel('Day')
            ax.set_ylabel('Number of individuals')
            ax.set_title(c+' / MAPE = ' + str(np.round(100 *
                         np.mean(abs((l - p)/l)), 4))+'%'+"//scaled MAPE: " + str(np.round(100 *
                                                                                           np.mean(abs((np.log1p(l) - np.log1p(p))/np.log1p(l))), 4)), fontsize=10)

        ax7.set_xlabel('Day')
        ax8.set_xlabel('Day')

        lines = []
        line_labels = []
        for ax in fig.axes:
            Line, Label = ax.get_legend_handles_labels()
            lines.extend(Line)
            line_labels.extend(Label)

        fig.legend(lines[:2], line_labels[:2], loc='lower center')

        print('Plot No.'+str(i) + ' done.')
        plt.savefig("/localdata1/gnn_paper_2024/images/pred_labels_trajectories/no_agegroups_30days_I_based_pred_labels/compartment_lines_noagegroups_30days_I_based_pred_labels_10k_paper_no" + str(i) + ".png")


def histogram_mape_per_run(mape_per_run, savename):

    plt.figure().clf()

    fig, ax = plt.subplots()
    # axs[0].ticklabel_format(style='plain')
    ax.hist(mape_per_run, bins=100)
    ax.set_xlabel('Test MAPE')
    ax.set_ylabel('Frequency')
    ax.set_title('Histogram for Test MAPE per run')
    # ax.set_xticks(np.arange(0,3500000,500000))

    plt.savefig(savename)


savename = 'secir_noagegroup_90days_I_based_MAPEs'
lineplots_compartments(mape_per_day, mape_reversed_per_day, savename)


savename_2 = 'secir_noagegroup_90days_I_based_MAPEs_two_axex'
lineplots_compartments_twoaxes(mape_per_day, mape_reversed_per_day, savename_2)

lineplots_pred_labels(pred_reversed, labels_reversed, num_plots=100)

savename = 'histogram_mape_per_run_simpleNN.png'
histogram_mape_per_run(mape_per_run, savename)

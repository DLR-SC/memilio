import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os
import ast
import pickle
import re
import matplotlib.gridspec as gridspec
from matplotlib.colors import ListedColormap


def heatmap_gridsearch_results(df_gridsearch, savename):
    df = df_gridsearch
    layers = []
    for i in df['layer']:
        layers.append(re.sub('[^a-zA-Z]+', '', i.split('.')[-1]))

    df['layer'] = layers
    plt.figure().clf()
    df_heatmap1 = pd.DataFrame(data=df.loc[(df['layer'] == 'ARMAConv')][[
                               'number_of_layers', 'number_of_neurons', 'kfold_val']])
    df_heatmap1 = df_heatmap1.pivot(
        index='number_of_layers', columns='number_of_neurons', values='kfold_val')

    df_heatmap2 = pd.DataFrame(data=df.loc[(df['layer'] == 'GCNConv')][[
        'number_of_layers', 'number_of_neurons', 'kfold_val']])
    df_heatmap2 = df_heatmap2.pivot(
        index='number_of_layers', columns='number_of_neurons', values='kfold_val')

    df_heatmap3 = pd.DataFrame(data=df.loc[(df['layer'] == 'APPNPConv')][[
                               'number_of_layers', 'number_of_neurons', 'kfold_val']])
    df_heatmap3 = df_heatmap3.pivot(
        index='number_of_layers', columns='number_of_neurons', values='kfold_val')

    df_heatmap4 = pd.DataFrame(data=df.loc[(df['layer'] == 'GATConv')][[
                               'number_of_layers', 'number_of_neurons', 'kfold_val']])
    df_heatmap4 = df_heatmap4.pivot(
        index='number_of_layers', columns='number_of_neurons', values='kfold_val')

    fig, axs = plt.subplots(nrows=2, ncols=2, sharex=False,
                            figsize=(20, 20), constrained_layout=True)

    for ax, df_heatmap, name in zip(axs.flat, [df_heatmap1, df_heatmap2, df_heatmap3, df_heatmap4], ['ARMAConv', 'GCNConv', 'GATConv', 'APPNPConv']):
        # for ax, df_heatmap, name in zip(axs.flat, [df_heatmap1, df_heatmap2, df_heatmap3], ['ARMAConv', 'GCNConv', 'APPNPConv']):
        im = ax.imshow(df_heatmap.values, cmap='RdYlGn_r')
        plt.rcParams.update({'font.size': 30})
        # Show all ticks and label them with the respective list entries
        ax.set_xticks(np.arange(len(df_heatmap.columns)),
                      labels=df_heatmap.columns, fontsize=25)
        ax.set_yticks(np.arange(len(df_heatmap.index)),
                      labels=df_heatmap.index, fontsize=25)

        ax.set_ylabel('number of hidden layers', fontsize=25)
        ax.set_xlabel('number of neurons per layer', fontsize=25)

        # Rotate the tick labels and set their alignment.
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right",
                 rotation_mode="anchor")

        # Loop over data dimensions and create text annotations.
        for i in range(len(df_heatmap.index)):
            for j in range(len(df_heatmap.columns)):
                text = ax.text(j, i, np.around(df_heatmap.values, decimals=2)[i, j],
                               ha="center", va="center", color="k", fontsize=25)

        ax.set_title('Model = '+name, fontsize=30)

    fig.colorbar(im, ax=axs, shrink=0.75, label='Validation MAPE')
    # fig.delaxes(axs[1][1])
    plt.show()
    plt.savefig(savename)


def ARMA_days_scatter(df_plot):
    df_plot = df[['number_of_layers',
                  'number_of_neurons', 'all_testscores']][:12]
    df_plot['all_testscores'] = df_plot['all_testscores'].apply(
        lambda x: np.asarray(ast.literal_eval(x)))

    # Explode the dataframe based on the 'all_test_scores' column
    df_expanded = df_plot.explode('all_testscores')
    df_expanded.reset_index(drop=True, inplace=True)

    # Define positions
    array = ['1st split', '2nd split', '3rd split', '4th split', '5th split']
    df_expanded['position'] = np.tile(array, len(df_plot))

    # Create a new column by combining the first two columns
    df_expanded['summary'] = df_expanded['number_of_layers'].astype(
        str) + ',' + df_expanded['number_of_neurons'].astype(str)

    # Define your own custom colors
    custom_colors = ['red', 'blue', 'green', 'orange', 'purple']

    # Create a ListedColormap using the custom colors
    custom_cmap = ListedColormap(custom_colors)

    # Plot the scatter plot with color coding based on 'position'
    plt.figure(figsize=(6, 5))  # Adjust figure size as needed
    for i, position in enumerate(array):
        plt.scatter(df_expanded[df_expanded['position'] == position]['summary'], df_expanded[df_expanded['position']
                    == position]['all_testscores'], c=custom_colors[i], label=position, cmap=custom_cmap)

    plt.legend()
    plt.ylabel('MAPE')
    plt.xlabel('Number of Days')
    # Set the x-ticks and labels explicitly
    plt.xticks(ticks=df_plot['summary'], labels=df_plot['summary'])
    plt.tight_layout()  # Adjust layout to prevent label overlap
    plt.savefig("ARMAConv_CV_testscores_days_equalnodes.png")


# HEATMAP
filename = 'GNN_gridsearch_withCV_nodeswithvariance.csv'
filename = 'GNN_gridsearch_withCV_equalnodes.csv'
# filename = 'gridserach_secir_groups_30days_I_based_Germany_10k_nodamp.csv'
path = os.path.dirname(os.path.realpath(__file__))
path_data = os.path.join(os.path.dirname(os.path.realpath(
    os.path.dirname(os.path.realpath(path)))), 'model_evaluations_paper')
filepath = os.path.join(path_data, filename)
df = pd.DataFrame(data=pd.read_csv(filepath))
savename = "GNN_gridsearch_nodeswithvariance.png"
# savename = "GNN_gridsearch_equalnodes.png"
heatmap_gridsearch_results(df, savename)

#############################################################################
# Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
#
# Authors: Agatha Schmidt, Henrik Zunker
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
import os
import pickle

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import tensorflow as tf

from memilio.simulation.secir import InfectionState
from memilio.surrogatemodel.ode_secir_groups import network_architectures


def plot_compartment_prediction_model(
        inputs, labels, modeltype,  model=None,
        plot_compartment='InfectedSymptoms', max_subplots=8):
    """! Plot prediction of the model and label for one compartment. The average of all age groups is plotted. 

    If model is none, we just plot the inputs and labels for the selected compartment without any predictions.  

    @param inputs test inputs for model prediction. 
    @param labels test labels. 
    @param modeltype type of model. Can be 'classic' or 'timeseries'
    @param model trained model. 
    @param plot_col string name of compartment to be plotted. 
    @param max_subplots Number of the simulation runs to be plotted and compared against. 
    """
    if modeltype == 'classic':
        input_width = int(inputs.shape[1] / 48)
        label_width = int(labels.shape[1])

    elif modeltype == 'timeseries':
        input_width = int(inputs.shape[1])
        label_width = int(labels.shape[1])

    plt.figure(figsize=(12, 8))
    plot_compartment_index = 0
    for compartment in InfectionState.values():
        if compartment.name == plot_compartment:
            break
        plot_compartment_index += 1
    if plot_compartment_index == len(InfectionState.values()):
        raise ValueError('Compartment name given could not be found.')
    max_n = min(max_subplots, inputs.shape[0])

    for n in range(max_n):
        plt.subplot(max_n, 1, n+1)
        plt.ylabel(plot_compartment)

        input_array = inputs[n].numpy()
        label_array = labels[n].numpy()

        if modeltype == 'classic':
            input_plot = input_array[:(input_width*6*8)]
            input_plot = input_plot.reshape(5, 48)

            mean_per_day_input = []
            for i in input_plot:
                x = i[plot_compartment_index::8]
                mean_per_day_input.append(x.mean())

            plt.plot(
                np.arange(0, input_width),
                mean_per_day_input,
                label='Inputs', marker='.', zorder=-10)

        elif modeltype == 'timeseries':
            mean_per_day_input = []
            for i in input_array:
                x = i[plot_compartment_index: inputs.shape[1] - 37:8]
                mean_per_day_input.append(x.mean())

            plt.plot(
                np.arange(0, input_width),
                mean_per_day_input,
                label='Inputs', marker='.', zorder=-10)

        mean_per_day = []
        for i in label_array:
            x = i[plot_compartment_index::8]
            mean_per_day.append(x.mean())
        plt.scatter(
            np.arange(input_width, input_width + label_width),
            mean_per_day,
            edgecolors='k', label='Labels', c='#2ca02c', s=64)

        if model is not None:
            input_series = tf.expand_dims(inputs[n], axis=0)
            pred = model(input_series)
            pred = pred.numpy()
            pred = pred.reshape((label_width, 48))

            mean_per_day_pred = []
            for i in pred:
                x = i[plot_compartment_index::8]
                mean_per_day_pred.append(x.mean())

            plt.scatter(np.arange(input_width, input_width+pred.shape[-2]),
                        # pred[0, :, plot_compartment_index],
                        mean_per_day_pred,
                        marker='X', edgecolors='k', label='Predictions',
                        c='#ff7f0e', s=64)

    plt.xlabel('days')
    if os.path.isdir("plots") == False:
        os.mkdir("plots")
    plt.savefig('plots/evaluation_secir_groups_' + plot_compartment + '.png')


#def network_fit(
#        path, filename,  model, modeltype, df_save, max_epochs=30, early_stop=100, plot=True):
def network_fit(
        path, filename,  model, modeltype, max_epochs=30, early_stop=100, plot=True):
    """! Training and evaluation of a given model with mean squared error loss and Adam optimizer using the mean absolute error as a metric.

    @param path path of the dataset. 
    @param model Keras sequential model.
    @param modeltype type of model. Can be 'classic' or 'timeseries'. Data preparation is made based on the modeltype.
    @param max_epochs int maximum number of epochs in training. 
    @param early_stop Integer that forces an early stop of training if the given number of epochs does not give a significant reduction of validation loss. 

    """

    if not os.path.isfile(os.path.join(path, 'data_secir_groups.pickle')):
        ValueError("no dataset found in path: " + path)

    file = open(os.path.join(path, filename), 'rb')

    data = pickle.load(file)
    data_splitted = split_data(data['inputs'], data['labels'])

    if modeltype == 'classic':

        train_inputs_compartments = flat_input(data_splitted["train_inputs"])
        train_labels = (data_splitted["train_labels"])
        valid_inputs_compartments = flat_input(data_splitted["valid_inputs"])
        valid_labels = (data_splitted["valid_labels"])
        test_inputs_compartments = flat_input(data_splitted["test_inputs"])
        test_labels = (data_splitted["test_labels"])


        train_inputs = tf.cast(train_inputs_compartments, tf.float32)
  
        valid_inputs = tf.cast(valid_inputs_compartments, tf.float32)
             
        test_inputs = tf.cast(test_inputs_compartments, tf.float32)


    elif modeltype == 'timeseries':

        train_inputs_compartments = (data_splitted["train_inputs"])
        train_labels = (data_splitted["train_labels"])
        valid_inputs_compartments = (data_splitted["valid_inputs"])
        valid_labels = (data_splitted["valid_labels"])
        test_inputs_compartments = (data_splitted["test_inputs"])
        test_labels = (data_splitted["test_labels"])


        train_inputs = train_inputs_compartments



        test_inputs =test_inputs_compartments


        valid_inputs = valid_inputs_compartments


    batch_size = 32

    early_stopping = tf.keras.callbacks.EarlyStopping(monitor='val_loss',
                                                      patience=early_stop,
                                                      mode='min')

    model.compile(
        loss=tf.keras.losses.MeanAbsolutePercentageError(),
        optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
        metrics=[tf.keras.metrics.MeanSquaredError()])

    history = model.fit(train_inputs, train_labels, epochs=max_epochs,
                        validation_data=(valid_inputs, valid_labels),
                        batch_size=batch_size,
                        callbacks=[early_stopping])

    if (plot):
        plot_losses(history)
        #plot_compartment_prediction_model(
        #    test_inputs, test_labels, modeltype, model=model,
        #    plot_compartment='InfectedSymptoms', max_subplots=3)
        df = get_test_statistic(test_inputs, test_labels, model)
        print(df)
        print('mean: ',  df.mean())

                
        #filename_df = 'datfarame_secirgroups_onedamp_noinfo'
        #df_save.loc[len(df_save.index)] = [df.mean()[0]]
        
        # path = os.path.dirname(os.path.realpath(__file__))
        # file_path = os.path.join(
        #     os.path.dirname(
        #         os.path.realpath(os.path.dirname(os.path.realpath(path)))),
        #     'secir_groups_onedamp_noinfo')
        # if not os.path.isdir(file_path):
        #     os.mkdir(file_path)
        # file_path = os.path.join(file_path,filename_df)
        # df_save.to_csv(file_path)

    return history


def plot_losses(history):
    """! Plots the losses of the model training.  

    @param history model training history. 

    """
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('model loss')
    plt.ylabel('loss')
    plt.xlabel('epoch')
    plt.legend(['train', 'val'], loc='upper left')
    if os.path.isdir("plots") == False:
        os.mkdir("plots")
    plt.savefig('plots/losses_plot.png')
    plt.show()


def get_test_statistic(test_inputs, test_labels, model):
    """! Calculates the mean absolute percentage error based on the test dataset.   

    @param test_inputs inputs from test data.
    @param test_labels labels (output) from test data.
    @param model trained model. 

    """

    pred = model(test_inputs)
    pred = pred.numpy()
    test_labels = np.array(test_labels)

    diff = pred - test_labels
    relative_err = (abs(diff))/abs(test_labels)
    # reshape [batch, time, features] -> [features, time * batch]
    relative_err_transformed = relative_err.transpose(2, 0, 1).reshape(8, -1)
    relative_err_means_percentage = relative_err_transformed.mean(axis=1) * 100

    
    # delete the two confirmed compartments from InfectionStates
    compartment_array = []
    for compartment in InfectionState.values():
        compartment_array.append(compartment) 
    index = [3,5]
    compartments_cleaned= np.delete(compartment_array, index)

    mean_percentage = pd.DataFrame(
        data=relative_err_means_percentage,
        index=[str(compartment).split('.')[1]
               for compartment in compartment_array],
        columns=['Percentage Error'])

    return mean_percentage


def split_data(inputs, labels, split_train=0.7,
               split_valid=0.2, split_test=0.1):
    """! Split data set in training, validation and testing data sets.

   @param inputs input dataset
   @param labels label dataset
   @param split_train Share of training data sets.
   @param split_valid Share of validation data sets.
   @param split_test Share of testing data sets.
   """

    if split_train + split_valid + split_test > 1 + 1e-10:
        raise ValueError(
            "Summed data set shares are greater than 1. Please adjust the values.")
    elif inputs.shape[0] != labels.shape[0] or inputs.shape[2] != labels.shape[2]:
        raise ValueError(
            "Number of batches or features different for input and labels")

    n = inputs.shape[0]
    n_train = int(n * split_train)
    n_valid = int(n * split_valid)
    n_test = n - n_train - n_valid

    inputs_train, inputs_valid, inputs_test = tf.split(
        inputs, [n_train, n_valid, n_test], 0)
    labels_train, labels_valid, labels_test = tf.split(
        labels, [n_train, n_valid, n_test], 0)

    data = {
        'train_inputs': inputs_train,
        'train_labels': labels_train,
        'valid_inputs': inputs_valid,
        'valid_labels': labels_valid,
        'test_inputs': inputs_test,
        'test_labels': labels_test
    }

    return data


def flat_input(input):
    """! Flatten input dimension

   @param input input array

   """
    dim = tf.reduce_prod(tf.shape(input)[1:])
    return tf.reshape(input, [-1, dim])





def get_input_dim_lstm(path, filename):
    """! Extract the dimensiond of the input data

   @param path path to the data 

   """
    file = open(os.path.join(path, filename), 'rb')

    data = pickle.load(file)
    input_dim = data['inputs'].shape[2] 

    return input_dim




def get_output_dim_lstm(path, filename):
    """! Extract the dimensiond of the input data

   @param path path to the data 

   """
    file = open(os.path.join(path, filename), 'rb')

    data = pickle.load(file)
    output_dim = data['labels'].shape[2] * data['labels'].shape[1]

    return output_dim


if __name__ == "__main__":
    path = os.path.dirname(os.path.realpath(__file__))
    path_data = os.path.join(os.path.dirname(os.path.realpath(
        os.path.dirname(os.path.realpath(path)))), 'data')
    filename = 'data_secir_groups_30days_nodamp.pickle'
    max_epochs = 10
    label_width = 30

    input_dim = get_input_dim_lstm(path_data, filename)
    output_dim = get_output_dim_lstm(path_data, filename)
    #df_save = pd.DataFrame(columns = ['MAPE'])





    model = "CNN_LSTM"
    if model == "Dense_Single":
            model = network_architectures.mlp_multi_input_single_output()
            modeltype = 'classic'

    elif model == "Dense":
            model = network_architectures.mlp_multi_input_multi_output(label_width)
            modeltype = 'classic'

    elif model == "LSTM":
            model = network_architectures.lstm_multi_input_multi_output(
                label_width)
            modeltype = 'timeseries'

    elif model == "CNN":
            model = network_architectures.cnn_multi_input_multi_output_simple(label_width)
            modeltype = 'timeseries'

    elif model == "CNN_LSTM":
            model = network_architectures.cnn_lstm_hybrid(input_dim, output_dim)
            modeltype = 'classic'
    
    model_output = network_fit(
            path_data, filename, model=model, modeltype=modeltype,
            max_epochs=max_epochs)



    # for i in range(5):
    #     tf.keras.backend.clear_session()

    #     model = "LSTM"
    #     if model == "Dense_Single":
    #         model = network_architectures.mlp_multi_input_single_output()
    #         modeltype = 'classic'

    #     elif model == "Dense":
    #         model = network_architectures.mlp_multi_input_multi_output(label_width)
    #         modeltype = 'classic'

    #     elif model == "LSTM":
    #         model = network_architectures.lstm_multi_input_multi_output(
    #             label_width)
    #         modeltype = 'timeseries'

    #     elif model == "CNN":
    #         model = network_architectures.cnn_multi_input_multi_output(label_width)
    #         modeltype = 'timeseries'
    
    #     model_output = network_fit(
    #         path_data, filename, model=model, modeltype=modeltype, df_save = df_save,
    #         max_epochs=max_epochs)

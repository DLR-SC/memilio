import numpy as np
import pandas as pd
from datetime import date
from math import ceil
import random
import os
import pickle
from progress.bar import Bar # pip install progess
from sklearn.model_selection import train_test_split
from sklearn.compose import make_column_transformer
from sklearn.preprocessing import MinMaxScaler
import tensorflow as tf
import matplotlib.pyplot as plt
from tensorflow import keras
from keras import Sequential
from keras.layers import Dense
from keras.optimizers import Adam
from keras import backend as K
import seaborn as sns # plot after normalization
from data_secir_simple import generate_data, splitdata
from different_models import *

def create_Dampings():

    dampings = pd.read_csv(
        os.path.join(os.path.dirname(os.path.realpath(path)), 'data35', 'data_secir_simple_dampings.txt'),
         sep=' ')

    dampings.columns = ['factor', 'day']
    i = 0 
    all_dampings = []
    while i < len(dampings):
        damping_date = int(dampings.iloc[i]['day'])
        damping_factor = dampings.iloc[i]['factor']
        days = 35 
        damping_list = []
        damping_list +=  [1] * (int(damping_date+1))
        damping_list +=  [damping_factor] * (days-damping_date) 
        all_dampings.append(damping_list) 
        i +=1



    return all_dampings

def addDampingstoInput(dampings, data):
    inputs, labels = getPartitions(input_width = 1, label_width = 34, data=data)
    dampings = create_Dampings()
    i = 0 
    input_withdamping = []
    while i < len(inputs):
         a = inputs.iloc[i].values.tolist()
         b = dampings[i]
         a.append(b)
         input_withdamping.append(a)
         i += 1

    return input_withdamping

def plotCol(inputs, labels, model=None, plot_col='Susceptible', max_subplots=3):

    input_width = inputs.shape[1]
    label_width = labels.shape[1]
    
    plt.figure(figsize=(12, 8))
    cols = np.array(['Susceptible', 'Exposed', 'Carrier',
                        'Infected', 'Hospitalized', 'ICU', 'Recovered', 'Dead'])  
    plot_col_index = np.where(cols == plot_col)[0][0]
    max_n = min(max_subplots, inputs.shape[0])

    # predictions = model(inputs) # for just one input: input_series = tf.expand_dims(inputs[n], axis=0) -> model(input_series)
    for n in range(max_n):
        plt.subplot(max_n, 1, n+1)
        plt.ylabel(f'{plot_col}')

        input_array = inputs[n].numpy()
        label_array = labels[n].numpy()
        plt.plot(np.arange(0,input_width), input_array[:, plot_col_index],
                label='Inputs', marker='.', zorder=-10)
        plt.scatter(np.arange(input_width,input_width+label_width), label_array[:,plot_col_index],
                    edgecolors='k', label='Labels', c='#2ca02c', s=64)
        
        if model is not None:
            input_series = tf.expand_dims(inputs[n], axis=0)
            pred = model(input_series)
            pred = pred.numpy()
            plt.scatter(np.arange(input_width,input_width+pred.shape[-2]), 
                                pred[0,:,plot_col_index],
                                marker='X', edgecolors='k', label='Predictions',
                                c='#ff7f0e', s=64)

    plt.xlabel('days')
    plt.show()
    plt.savefig('evaluation_secir_simple_' + plot_col + '.pdf')

def network_fit(path, model,  MAX_EPOCHS=30, early_stop=4, save_evaluation_pdf=False):

    if not os.path.isfile(os.path.join(path, 'data_secir_simple.pickle')):
        ValueError("no dataset found in path: " + path)

    file = open(os.path.join(path, 'data_secir_simple.pickle'),'rb')
    data = pickle.load(file)
    data_splitted = splitdata(data["inputs"], data["labels"])

    train_inputs = data_splitted["train_inputs"]
    train_labels = data_splitted["train_labels"]
    valid_inputs = data_splitted["valid_inputs"]
    valid_labels = data_splitted["valid_labels"]
    test_inputs  = data_splitted["test_inputs"]
    test_labels  = data_splitted["test_labels"]

    early_stopping = tf.keras.callbacks.EarlyStopping(monitor='val_loss',
                                                    patience=early_stop,
                                                    mode='min')

    model.compile(loss=tf.keras.losses.MeanSquaredError(),
                optimizer=tf.keras.optimizers.Adam(),
                metrics=[tf.keras.metrics.MeanAbsoluteError()])

    history = model.fit(train_inputs, train_labels, epochs=MAX_EPOCHS,
                      validation_data=(valid_inputs, valid_labels),
                      callbacks=[early_stopping])

    plotCol(test_inputs, test_labels, model=model, plot_col='Susceptible', max_subplots=3)
    return history
    
# simple benchmarking
def timereps(reps, model, input):
    from time import time
    start = time()
    for i in range(0, reps):
        _ = model(input)
    end = time()
    time_passed = end - start
    print(time_passed)
    return (end - start) / reps

# Plot Performance
def plot_histories(histories):
    model_names = ["LSTM", "Dense"]
    count_names = 0
    interval = np.arange(len(histories))
    for x in interval:
        history = histories[x]
        width = 0.3

        train_mae = history.history['mean_absolute_error'][-1]
        valid_mae = history.history['val_mean_absolute_error'][-1]

        plt.bar(x - 0.17, train_mae, width, label='Train')
        plt.bar(x + 0.17, valid_mae, width, label='Validation')
        name = model_names[x]
        # plt.xticks(ticks=x, labels=[name,name],
        #         rotation=45)
        plt.ylabel(f'MAE')
        _ = plt.legend()
        count_names = count_names + 1
    
    plt.show()
    plt.savefig('evaluation_single_shot.pdf')

if __name__ == "__main__":
    # TODO: Save contact matrix depending on the damping.
    # In the actual state it might be enough to save the regular one and the damping

    path = os.path.dirname(os.path.realpath(__file__))
    path_data = os.path.join(os.path.dirname(os.path.realpath(path)), 'data')

    MAX_EPOCHS = 50

    ### Models ###
    # single input
    # hist = network_fit(path_data, model=single_output(), MAX_EPOCHS=MAX_EPOCHS)

    # # multi input
    # lstm_hist = network_fit(path_data, model=lstm_network_multi_input(), MAX_EPOCHS=MAX_EPOCHS)
    # ml_hist = network_fit(path_data, model=multilayer_multi_input(), MAX_EPOCHS=MAX_EPOCHS)

    # # Multi output 
    cnn_output = network_fit(path_data, model=cnn_multi_output(), MAX_EPOCHS=MAX_EPOCHS)
    # lstm_hist_multi = network_fit(path_data, model=lstm_multi_output(), MAX_EPOCHS=MAX_EPOCHS)
    
    # histories = [ lstm_hist, ml_hist]
    # plot_histories(histories)

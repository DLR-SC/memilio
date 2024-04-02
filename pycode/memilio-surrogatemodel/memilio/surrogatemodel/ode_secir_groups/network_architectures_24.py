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
import tensorflow as tf


def mlp_multi_input_single_output(num_age_groups=6):
    """! Simple MLP Network which takes the compartments for multiple time steps as input and
    returns the 8 compartments for all six age groups for one single time step.

    Reshaping adds an extra dimension to the output, so the shape of the output is 1x48.
    This makes the shape comparable to that of the multi-output models.

    @param num_age_groups Number of age groups in population.
    """
    model = tf.keras.Sequential([
        tf.keras.layers.Flatten(),
        tf.keras.layers.Dense(units=32, activation='relu'),
        tf.keras.layers.Dense(units=32, activation='relu'),
        tf.keras.layers.Dense(units=8*num_age_groups),
        tf.keras.layers.Reshape([1, -1]), ])
    return model


def mlp_multi_input_multi_output(label_width, num_age_groups=6):
    """! Simple MLP Network which takes the compartments for multiple time steps as input and
    returns the 8 compartments for all age groups for multiple time steps in the future. 

    Reshaping adds an extra dimension to the output, so the shape of the output is 30x48.
    This makes the shape comparable to that of the multi-output models.

    @param label_width Number of time steps in the output.
    @param num_age_groups Number of age groups in population.
    """
    model = tf.keras.Sequential([
        tf.keras.layers.Flatten(),
        tf.keras.layers.Dense(units=1024, activation='relu'),
        tf.keras.layers.Dense(units=1024, activation='relu'),
        tf.keras.layers.Dense(units=1024, activation='relu'),
        tf.keras.layers.Dense(units=1024, activation='relu'),
        tf.keras.layers.Dense(units=label_width*num_age_groups*8),
        tf.keras.layers.Reshape([label_width, num_age_groups*8])
    ])
    return model


def cnn_multi_input_multi_output(label_width, num_age_groups=6):
    """! CNN Network which uses multiple time steps as input and returns the 8 compartments for
    each age groups for multiple time step in the future.

    Input and output have shape [number of expert model simulations, time points in simulation,
    number of individuals in infection states].

    @param label_width Number of time steps in the output.
    @param num_age_groups Number of age groups in population.

    """

    model = tf.keras.Sequential([
        tf.keras.layers.Conv1D(filters=64, kernel_size=3, activation='relu'),
        tf.keras.layers.Conv1D(filters=64, kernel_size=3, activation='relu'),
        # tf.keras.layers.MaxPooling1D(pool_size=2),
        tf.keras.layers.Flatten(),
        tf.keras.layers.GaussianNoise(0.35),
        tf.keras.layers.Dense(512, activation='relu'),
        # tf.keras.layers.Dense(512, activation='relu'),
        tf.keras.layers.Dense(
            label_width * num_age_groups * 8, activation='linear'),
        tf.keras.layers.Reshape([label_width, 8 * num_age_groups])
    ])
    return model





def cnn_multi_input_multi_output_experiments(label_width, num_age_groups=6):
    """! CNN Network which uses multiple time steps as input and returns the 8 compartments for
    each age groups for multiple time step in the future.

    Input and output have shape [number of expert model simulations, time points in simulation,
    number of individuals in infection states].

    @param label_width Number of time steps in the output.
    @param num_age_groups Number of age groups in population.

    """
    conv_size = 3
    model = tf.keras.Sequential([
        
    tf.keras.layers.Lambda(lambda x: x[:, -conv_size:, :]),
    tf.keras.layers.Conv1D(1024, activation='relu',
                                    kernel_size=(conv_size)),
    tf.keras.layers.Conv1D(1024, activation='relu',
                                    kernel_size=(conv_size)),
    #tf.keras.layers.Flatten(),
    #tf.keras.layers.GaussianNoise(0.35),
    tf.keras.layers.Dense(units=1024, activation='relu'),
    tf.keras.layers.Dense(units=1024, activation='relu'),

    tf.keras.layers.Dense(
            label_width * num_age_groups * 8, activation='linear'),
    tf.keras.layers.Reshape([label_width, 8 * num_age_groups])])
    
    return model


def cnn_multi_input_multi_output_simple(label_width, num_age_groups=6):
    """! CNN Network which uses multiple time steps as input and returns the 8 compartments for
    each age groups for multiple time step in the future.

    Input and output have shape [number of expert model simulations, time points in simulation,
    number of individuals in infection states].

    @param label_width Number of time steps in the output.
    @param num_age_groups Number of age groups in population.

    """
    conv_size = 3
    model = tf.keras.Sequential([
        
    tf.keras.layers.Lambda(lambda x: x[:, -conv_size:, :]),
    tf.keras.layers.Conv1D(1024, activation='relu',
                                    kernel_size=(conv_size)),
    tf.keras.layers.Dense(units=1024, activation='relu'),
    tf.keras.layers.Dense(units=1024, activation='relu'),

    tf.keras.layers.Dense(
            label_width * num_age_groups * 8, activation='linear'),
    tf.keras.layers.Reshape([label_width, 8 * num_age_groups])])
    
    return model



def lstm_multi_input_multi_output(label_width, num_age_groups=6):
    """! LSTM Network which uses multiple time steps as input and returns the 8 compartments for
    one single time step in the future.

    Input and output have shape [number of expert model simulations, time points in simulation,
    number of individuals in infection states].

    @param label_width Number of time steps in the output.
    """
    model = tf.keras.Sequential([
        tf.keras.layers.LSTM(512, return_sequences=False),
        tf.keras.layers.Dense(label_width * 8 * num_age_groups,
                              kernel_initializer=tf.initializers.zeros()),
        tf.keras.layers.Reshape([label_width, 8 * num_age_groups])])
    return model

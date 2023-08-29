import os
import numpy as np
import pandas as pd
import torch
import torch.nn as nn
import torch.nn.functional as F
import pickle
import spektral
import keras
import time

import matplotlib.pyplot as plt
import numpy as np
import scipy.sparse as sp
import tensorflow as tf
from keras.layers import Dense
from keras.losses import MeanAbsolutePercentageError
from keras.metrics import mean_absolute_percentage_error
from keras.models import Model
from keras.optimizers import Adam, Nadam

from sklearn.model_selection import KFold

from spektral.data import Dataset, DisjointLoader, Graph, Loader, BatchLoader, MixedLoader
from spektral.layers import GCSConv, GlobalAvgPool, GlobalAttentionPool, ARMAConv, AGNNConv, APPNPConv, CrystalConv, GCNConv, GATConv, GINConv, XENetConv, ChebConv, ECCConv, GlobalSumPool
from spektral.transforms.normalize_adj import NormalizeAdj
from spektral.utils.convolution import normalized_laplacian, gcn_filter, chebyshev_filter, rescale_laplacian


from memilio.simulation.secir import InfectionState

# add to setup: torch, spektral


path = os.path.dirname(os.path.realpath(__file__))
path_data = os.path.join(
    os.path.dirname(
        os.path.realpath(os.path.dirname(os.path.realpath(path)))),
    'data/data_GNN_nodamp_20pop_1k')

file = open(os.path.join(path_data, 'data_secir_age_groups.pickle'), 'rb')
data_secir = pickle.load(file)

len_dataset = data_secir['inputs'][0].shape[0]
numer_of_nodes = np.asarray(data_secir['inputs']).shape[0]
shape_input_flat = np.asarray(
    data_secir['inputs']).shape[2]*np.asarray(data_secir['inputs']).shape[3]
shape_labels_flat = np.asarray(
    data_secir['labels']).shape[2]*np.asarray(data_secir['labels']).shape[3]


new_inputs = np.asarray(
    data_secir['inputs']).reshape(
    len_dataset, numer_of_nodes, shape_input_flat)
new_labels = np.asarray(data_secir['labels']).reshape(
    len_dataset, numer_of_nodes, shape_labels_flat)

######## open commuter data #########
path = os.path.dirname(os.path.realpath(__file__))
path_data = os.path.join(path,
                         'data')
commuter_file = open(os.path.join(
    path_data, 'commuter_migration_scaled.txt'), 'rb')
commuter_data = pd.read_csv(commuter_file, sep=" ", header=None)
sub_matrix = commuter_data.iloc[:numer_of_nodes, 0:numer_of_nodes]


adjacency_matrix = np.asarray(sub_matrix)

adjacency_matrix[adjacency_matrix > 0] = 1

node_features = new_inputs

node_labels = new_labels


class MyDataset(spektral.data.dataset.Dataset):
    def read(self):
        self.a = gcn_filter(adjacency_matrix)
        # self.a = adjacency_matrix

        return [spektral.data.Graph(x=x, y=y) for x, y in zip(node_features, node_labels)]

        super().__init__(**kwargs)


################################################################################
# Build model
################################################################################


class Net(Model):
    def __init__(self):
        super().__init__()

        # self.conv1 = ARMAConv(32,     activation="relu")

        ##### for mixed mode ####
        self.conv1 = GCNConv(32,     activation="sigmoid")
        self.conv2 = GCNConv(32,   activation="sigmoid")
        # self.conv3 = GATConv(32,   activation="relu",attn_heads = 1, dropout_rate = 0.5)
        #self.global_pool = GlobalSumPool()
        self.dense = Dense(data.n_labels, activation="linear")

    def call(self, inputs):
        x, a = inputs
        #a= rescale_laplacian(normalized_laplacian(np.asarray(a)))
        #a = np.asarray(a)
        

        x = self.conv1([x, a])

        x = self.conv2([x, a])
        # x = self.conv3([x, a])
        #output = self.global_pool(x)
        output = self.dense(x)

        return output





# @tf.function(input_signature=loader_tr.tf_signature(),
# experimental_relax_shapes=True)
def train_step(inputs, target):
    with tf.GradientTape() as tape:
        predictions = model(inputs, training=True)
        loss = loss_fn(target, predictions) + sum(model.losses)
    gradients = tape.gradient(loss, model.trainable_variables)
    optimizer.apply_gradients(zip(gradients, model.trainable_variables))
    acc = tf.reduce_mean(mean_absolute_percentage_error(target, predictions))
    return loss, acc


def evaluate(loader):
    output = []
    step = 0
    while step < loader.steps_per_epoch:
        step += 1
        inputs, target = loader.__next__()
        pred = model(inputs, training=False)
        outs = (
            loss_fn(target, pred),
            tf.reduce_mean(mean_absolute_percentage_error(target, pred)),
            len(target),  # Keep track of batch size
        )
        output.append(outs)
        if step == loader.steps_per_epoch:
            output = np.array(output)
            return np.average(output[:, :-1], 0, weights=output[:, -1])


def test_evaluation(loader):

    inputs, target = loader.__next__()
    pred = model(inputs, training=False)

    mean_per_batch = []

    for batch_p, batch_t in zip(pred, target):
        MAPE_v = []
        for v_p, v_t in zip(batch_p, batch_t):

            pred_ = tf.reshape(v_p, (26, 48))
            target_ = tf.reshape(v_t, (26, 48))

            diff = pred_ - target_
            relative_err = (abs(diff))/abs(target_)
            relative_err_transformed = np.asarray(
                relative_err).transpose().reshape(8, -1)
            relative_err_means_percentage = relative_err_transformed.mean(
                axis=1) * 100

            MAPE_v.append(relative_err_means_percentage)

        mean_per_batch.append(np.asarray(MAPE_v).transpose().mean(axis=1))

    mean_percentage = pd.DataFrame(
        data=np.asarray(mean_per_batch).transpose().mean(axis=1),
        index=[str(compartment).split('.')[1]
               for compartment in InfectionState.values()],
        columns=['Percentage Error'])

    return mean_percentage


################################################################################
#                Training and Evaluation                                       
#################################################################################


# data = MyDataset(transforms=NormalizeAdj())
data = MyDataset()
batch_size = 32
epochs = 1500
es_patience = 50  # Patience for early stopping


idxs = np.random.permutation(len(data))
split_va, split_te = int(0.8 * len(data)), int(0.9 * len(data))
idx_tr, idx_va, idx_te = np.split(idxs, [split_va, split_te])
data_tr = data[idx_tr]
data_va = data[idx_va]
data_te = data[idx_te]



# Data loaders
loader_tr = MixedLoader(
    data_tr, batch_size=batch_size, epochs=epochs)
loader_va = MixedLoader(data_va,  batch_size=batch_size)
loader_te = MixedLoader(data_te, batch_size=data_te.n_graphs)



learning_rate = 0.001
optimizer = Nadam(learning_rate=learning_rate)
loss_fn = MeanAbsolutePercentageError()


start = time.perf_counter()



model = Net()




epoch = step = 0
best_val_loss = np.inf
best_weights = None
patience = es_patience
results = []
losses_history = []
val_losses_history = []


for batch in loader_tr:
        step += 1
        loss, acc = train_step(*batch)
        results.append((loss, acc))
        if step == loader_tr.steps_per_epoch:
            step = 0
            epoch += 1

            # Compute validation loss and accuracy
            val_loss, val_acc = evaluate(loader_va)

            print(
                "Ep. {} - Loss: {:.3f} - Acc: {:.3f} - Val loss: {:.3f} - Val acc: {:.3f}".format(
                    epoch, *np.mean(results, 0), val_loss, val_acc
                )
            )

            # Check if loss improved for early stopping
            if val_loss < best_val_loss:
                best_val_loss = val_loss
                patience = es_patience
                print("New best val_loss {:.3f}".format(val_loss))
                best_weights = model.get_weights()
            else:
                patience -= 1
                if patience == 0:
                    print("Early stopping (best val_loss: {})".format(best_val_loss))

                    break
            results = []
            losses_history.append(loss)
            val_losses_history.append(val_loss)

#elapsed = time.perf_counter() - start
################################################################################
# Evaluate model
################################################################################
model.set_weights(best_weights)  # Load best model
test_loss, test_acc = evaluate(loader_te)
test_MAPE = test_evaluation(loader_te)
print(test_MAPE)





print("Done. Test loss: {:.4f}. Test acc: {:.2f}".format(test_loss, test_acc))
print("Best training loss: {:.4f}. ".format(np.asarray(losses_history).min()))
print("Best validation loss: {:.4f}. ".format(np.asarray(val_losses_history).min()))

elapsed = time.perf_counter() - start


#plot the losses
plt.figure()
plt.plot(np.asarray(losses_history), label='train loss')
plt.plot(np.asarray(val_losses_history), label='val loss')
plt.xlabel('Epoch')
plt.ylabel('Loss ( MAPE)')
plt.title('Loss for l=0.0001')
plt.legend()
plt.savefig('losses_lr0.0001_morepochs.png')


print("Time for training: {:.4f} seconds".format(elapsed))
print("Time for training: {:.4f} minutes".format(elapsed/60))

#############################################################################
# Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
#
# Authors: Martin J. Kuehn
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
import datetime as dt

import numpy as np
import pandas as pd

import memilio.plot.plotMap as pm

if __name__ == '__main__':

    files_input = {'Data set 1': 'PATH_TO_FILE_1',
                   'Data set 2': 'PATH_TO_FILE_2'}
    file_format = 'json'
    # Define age groups which will be considered through filtering
    # Keep keys and values as well as its assignment constant, remove entries
    # if only part of the population should be plotted or considered, e.g., by
    # setting:
    # age_groups = {1: '5-14', 2: '15-34'}
    age_groups = {0: '0-4', 1: '5-14', 2: '15-34',
                  3: '35-59', 4: '60-79', 5: '80+'}
    if len(age_groups) == 6:
        filter_age = None
    else:
        if file_format == 'json':
            filter_age = [val for val in age_groups.values()]
        else:
            filter_age = ['Group' + str(key) for key in age_groups.keys()]

    relative = True

    i = 0
    for file in files_input.values():
        # MEmilio backend hdf5 example
        # df = pm.extract_data(
        #     file, region_spec=None, column=None, date=1,
        #     filters={'Group': filter_age, 'InfectionState': [3, 4]},
        #     file_format=file_format)
        # MEmilio epidata vaccination data json example
        df = pm.extract_data(
            file, region_spec='ID_County',
            column='Vacc_partially',
            date=dt.date(2021, 11, 18),
            filters={'ID_State': None,
                     'Age_RKI': filter_age},
            file_format=file_format)

        if relative:
            # For fitting of different age groups we need format ">X".
            age_group_values = list(age_groups.values())
            age_group_values[-1] = age_group_values[-1].replace('80+', '>79')
            # scale data
            df = pm.scale_dataframe_relative(
                df, age_group_values,
                'PATH_TO_POPULATION_DATA_FILE.json')

        if i == 0:
            dfs_all = pd.DataFrame(df.iloc[:, 0])

        dfs_all[df.columns[-1] + ' ' + str(i)] = df[df.columns[-1]]
        i += 1

    min_val = dfs_all[dfs_all.columns[1:]].min().min()
    max_val = dfs_all[dfs_all.columns[1:]].max().max()

    pm.plot_map(
        dfs_all, scale_colors=np.array([min_val, max_val]),
        legend=['Label 1', 'Label 2'],
        title='Title', plot_colorbar=True,
        fig_name='Plot name', dpi=300,
        outercolor=[205 / 255, 238 / 255, 251 / 255])

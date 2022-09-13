#############################################################################
# Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
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
"""
@file modifyDataframeSeries.py
@brief Tools modify data frame series like imputing zeros for unknown dates,
    copying previous values, and/or computing moving averages
"""
import pandas as pd
import numpy as np
import itertools
from datetime import datetime, timedelta

from memilio.epidata import defaultDict as dd


def impute_and_reduce_df(
        df_old, group_by_cols, mod_cols, impute='forward', moving_average=0,
        min_date='', max_date='', start_w_firstval=False):
    """! Impute missing dates of dataframe time series and optionally calculates a moving average of the data.
    Extracts Dates between min and max date.

    @param df_old old pandas dataframe
    @param group_by_cols Column names for grouping by and items of particular group specification (e.g., for region: list of county oder federal state IDs)
    @param mod_cols List of columns for which the imputation and/or moving average is conducted (e.g., Confirmed or ICU)
    @param impute [Default: 'forward'] imputes either based on older values ('forward') or zeros ('zeros')
    @param moving_average [Default: 0, no averaging] Number of days over which to compute the moving average
    @param min_date [Default: '', taken from df_old] If set, minimum date to be set in new data frame for all items in group_by
    @param max_date [Default: '', taken from df_old] If set, maximum date to be set in new data frame for all items in group_by
    @param start_w_firstval: [Default: False] If True and min_date < first date in dataframe, then between min_date and first date, the value
        of the first date will be repeated backwards. If False, then zero is set there.
    @return dataframe with imputed dates (and moving average if requested)
    """
    # derive date from time
    try:
        df_old.Date = df_old.Date.dt.date
    except AttributeError:
        df_old[dd.EngEng['date']] = pd.to_datetime(df_old[dd.EngEng['date']])
        df_old.Date = df_old.Date.dt.date

    # create empty copy of the df
    df_new = pd.DataFrame(columns=df_old.columns)
    # make pandas use the same data types....
    df_new = df_new.astype(dtype=dict(zip(df_old.columns, df_old.dtypes)))

    # remove 'index' column if available
    try:
        df_new = df_new.drop(columns='index')
    except KeyError:
        pass

    # range of dates which should be filled
    if min_date == '':
        min_date = min(df_old[dd.EngEng['date']])
    if max_date == '':
        max_date = max(df_old[dd.EngEng['date']])

    # Transform dates to datetime
    if isinstance(min_date, str) == True:
        min_date = datetime.strptime(min_date, "%Y-%m-%d")
    if isinstance(max_date, str) == True:
        max_date = datetime.strptime(max_date, "%Y-%m-%d")

    start_date = min_date
    end_date = max_date
    # shift start and end date for relevant dates to compute moving average.
    # if moving average is odd, both dates are shifted equaly.
    # if moving average is even, start date is shifted one day more than end date.
    if moving_average > 0:
        end_date = end_date + timedelta(int(np.floor((moving_average-1)/2)))
        start_date = start_date - timedelta(int(np.ceil((moving_average-1)/2)))

    idx = pd.date_range(start_date, end_date)

    # create list of all possible groupby columns combinations
    unique_ids = []
    for group_key in group_by_cols.keys():
        unique_ids.append(group_by_cols[group_key])
    unique_ids_comb = list(itertools.product(*unique_ids))
    # create list of keys/group_by column names
    group_by = list(group_by_cols.keys())

    # loop over all items in columns that are given to group by (i.e. regions/ages/gender)
    for ids in unique_ids_comb:
        df_local = df_old.copy()
        counter = 0
        # filter df
        while counter < len(ids):
            df_local = df_local[df_local[group_by[counter]] == ids[counter]]
            counter += 1

        # create missing dates
        df_local.index = df_local.Date
        df_local_new = df_local.reindex(idx)
        df_local_new.Date = idx
        df_local_new.index = (range(len(idx)))

        if len(df_local) > 0:
            # create values for first date
            values = {}
            for column in df_local.columns:
                values[column] = df_local[column][0]
            # depending on 'start_w_firstval', missing values at the beginning
            # of the frame will either be set to zero or to the first available
            # value in the data frame
            if not start_w_firstval:
                for avg in mod_cols:
                    values[avg] = 0

            if impute == 'zeros':
                # impute zeros at missing dates
                df_local_new.fillna(values, inplace=True)
            else:
                # fill values of missing dates based on last entry
                df_local_new.fillna(method='ffill', inplace=True)
                # fill value of the first date, if it doesn't exist yet
                # has to be conducted in second step to not impute 'value'
                # at first missing value if start is present
                df_local_new.fillna(values, limit=1, inplace=True)
                # fill remaining values (between first date and first
                # reported date of the df_local)
                df_local_new.fillna(method='ffill', inplace=True)

            # compute 'moving average'-days moving average
            if moving_average > 0:
                for avg in mod_cols:
                    # compute moving average in new column
                    df_local_new['MA' + avg] = df_local_new[avg].rolling(
                        window=moving_average, min_periods=moving_average,
                        center=True).mean().fillna(df_local_new[avg])
                    df_local_new['MA' + avg] = df_local_new['MA' +
                                                            avg].fillna(df_local_new[avg])
                    # overwrite daily values by moving averages
                    df_local_new[avg] = df_local_new['MA' + avg]
                    # remove helper column 'MA'+column_name
                    df_local_new.drop('MA' + avg, axis=1, inplace=True)

        else:
            # Decide whether to activate the following warning or not.
            # It can happen that certain local entities do not have certain data
            # at all (e.g., many counties do not have had any kind of
            # refreshing vaccinations so far.) Then, the following warning
            # is misleading.
            # print('Warning: Tuple ' + str(ids) + ' not found in local data frame. Imputing zeros.')
            # create zero values for non-existent time series
            values = {}
            counter = 0
            while counter < len(ids):
                values[group_by[counter]] = ids[counter]
                counter += 1
            for avg in mod_cols:
                values[avg] = 0
            # TODO: by this the corresponding columns will be zero-filled
            #       other entries such as names etc will get lost here
            #       any idea to introduce these names has to be found.
            df_local_new.fillna(values, inplace=True)

        # append current local entity (i.e., county or state)
        df_new = df_new.append(df_local_new)
        # rearrange indices from 0 to N
        df_new.index = (range(len(df_new)))

    # extract min and max date
    df_new = extract_subframe_based_on_dates(df_new, min_date, max_date)

    return df_new


def split_column_based_on_values(
        df_to_split, column_to_split, column_vals_name, groupby_list,
        new_column_labels, compute_cumsum):
    """! Splits a column in a dataframe into separate columns. For each unique value that appears in a selected column,
    all corresponding values in another column are transfered to a new column. If required, cumulative sum is calculated in new generated columns.

    @param df_to_split global pandas dataframe
    @param column_to_split identifier of the column for which separate values will define separate dataframes
    @param column_vals_name The name of the original column which will be split into separate columns named according to new_column_labels.
    @param groupby_list The name of the original columns with which data of new_column_labels can be joined.
    @param new_column_labels New labels for resulting columns. There have to be the same amount of names and unique values as in groupby_list.
    @param compute_cumsum Computes cumulative sum in new generated columns
    @return a dataframe with the new splitted columns
    """
    # TODO: Maybe we should input a map e.g. 1->Vacc_partially, 2->Vacc_completed etc. This is more future proof.
    # check number of given names and correct if necessary
    column_identifiers = sorted(df_to_split[column_to_split].unique())
    i = 2
    while len(column_identifiers) > len(new_column_labels):
        new_column_labels.append(new_column_labels[-1]+'_'+str(i))
        i += 1

    # create empty copy of the df_to_split
    df_joined = pd.DataFrame(
        columns=df_to_split.columns).drop(
        columns=[column_to_split, column_vals_name])

    for i in range(0, len(column_identifiers)):
        df_reduced = df_to_split[df_to_split[column_to_split] == column_identifiers[i]].rename(
            columns={column_vals_name: new_column_labels[i]}).drop(columns=column_to_split)
        df_reduced = df_reduced.groupby(
            groupby_list).agg({new_column_labels[i]: sum})
        if compute_cumsum:
            # compute cummulative sum over level index of ID_County and level
            # index of Age_RKI
            df_reduced = df_reduced.groupby(
                level=[groupby_list.index(dd.EngEng['idCounty']),
                       groupby_list.index(dd.EngEng['ageRKI'])]).cumsum()
        # joins new generated column to dataframe
        df_joined = df_reduced.reset_index().join(
            df_joined.set_index(groupby_list),
            on=groupby_list, how='outer')

    return new_column_labels, df_joined


def extract_subframe_based_on_dates(df, start_date, end_date):
    """! Removes all data with date lower than start date or higher than end date.

    Returns the Dataframe with only dates between start date and end date.
    Resets the Index of the Dataframe.

    @param df The dataframe which has to be edited
    @param start_date Date of first date in dataframe
    @param end_date Date of last date in dataframe

    @return a dataframe with the extracted dates
    """

    upperdate = datetime.strftime(end_date, '%Y-%m-%d')
    lowerdate = datetime.strftime(start_date, '%Y-%m-%d')

    # Removes dates higher than end_date
    df_new = df[df[dd.EngEng['date']] <= upperdate]
    # Removes dates lower than start_date
    df_new = df_new[df_new[dd.EngEng['date']] >= lowerdate]

    df_new.reset_index(drop=True, inplace=True)

    return df_new


def insert_column_by_map(df, col_to_map, new_col_name, map):
    """! Adds a column to a given dataframe based on a mapping of values of a given column

    The mapping is defined by a list containing tupels of the form (new_value, old_value)
    where old_value is a value in the col_to_map and new_value the value
    that is added in the new column if col_to_map contains the old_value.
    @param df dataframe to modify
    @param col_to_map column containing values to be mapped
    @param new_col_name name of the new column containing the mapped values
    @param map List of tuples of values in the column to be added and values in the given column
    @return dataframe df with column of state names correspomding to state ids
    """
    df_new = df.copy()
    loc_new_col = df_new.columns.get_loc(col_to_map)+1
    df_new.insert(loc=loc_new_col, column=new_col_name,
                  value=df_new[col_to_map])
    for item in map:
        df_new.loc[df_new[col_to_map] == item[1], [new_col_name]] = item[0]
    return df_new


def create_intervals_mapping(from_lower_bounds, to_lower_bounds):
    """! Creates a mapping from given intervals to new desired intervals

    @param from_lower_bounds lower bounds of original intervals
    @param to_lower_bounds desired lower bounds of new intervals
    @return mapping from intervals to intervals
        The mapping is given as a list of tupels for every original interval.
        The list contains a tuple for every new interval intersecting the
        original interval. Each tuple defines the share of the original interval
        that is mapped to the new interval and the index of the new interval. We
        assume that the range of the intervals mapped from is contained in the
        range of the intervals mapped to.
        For example for from_lower_bounds = [5,20,30,80,85,90] and
        to_lower_bounds=[0,15,20,60,100] given the mapping would be
        [[[2/3,0], [1/3,1]],
         [[1,2]],
         [[3/5,2], [2/5,3]],
         [[1,3]],
         [[1,3]]]
    """
    if (from_lower_bounds[0] < to_lower_bounds[0] or
            from_lower_bounds[-1] > to_lower_bounds[-1]):
        raise ValueError("Range of intervals mapped from exeeds range of" +
                         "intervals mapped to.")
    # compute the shares of the original intervals mapped to the new intervals
    from_to_mapping = [[] for i in range(0, len(from_lower_bounds)-1)]
    to_idx = 0  # iterator over new intervals
    # iterate over original intervals
    for from_idx in range(0, len(from_lower_bounds) - 1):
        remaining_share = 1  # share of original interval to be distributed
        # position up to which the distribution is already computed
        pos = from_lower_bounds[from_idx]
        len_orig_interval = from_lower_bounds[from_idx+1] - pos
        # find first new interval intersecting the original interval
        while pos >= to_lower_bounds[to_idx+1]:
            to_idx += 1
        while from_lower_bounds[from_idx+1] > to_lower_bounds[to_idx+1]:
            # compute share of original interval that is send to new interval
            share = (to_lower_bounds[to_idx+1] - pos) / len_orig_interval
            from_to_mapping[from_idx].append([share, to_idx])
            remaining_share -= share
            pos = to_lower_bounds[to_idx+1]
            to_idx += 1
        # if upper bound of the new interval is not smaller than upper bound of
        # the original interval assign remaining share of the original interval
        # to the new interval
        from_to_mapping[from_idx].append([remaining_share, to_idx])
    return from_to_mapping


def fit_age_group_intervals(
        df_age_in, age_out, df_population=None, max_age=100):
    """! Creates a mapping from given intervals to new desired intervals

    @param df_age_in df with 2 rows. first row containts age distribution and second a population
    @param age_out desired age group distribution
    @param df_population population data with arbitrary age groups and same structure as df_age_in
    @return population in new age group age_out

        When df_population is set, we use this for the distribution.
        Otherwiese, we assume the population to be equally distributed.
        Given df_age_in = 
        [ 1-10, 11-60, 61-99],
        [4,     10,     8]
        age_out = [1-5, 6-10, 11-50, 51-99]
        So the output should be:
        [2.,  2.,  8., 10.]

        If we also add an population, like 
        population = ["1-5", "6-7", "8-10", "11-60", "61-99"],
                     [ 40,     5,    5,      25,       25])
        So the population is no longer equally distributed. So the output is:
        [3.2,  0.8,  8., 10.]
    """
    # get minimum ages of each group
    age_in_min = []
    max_entry = 0
    for age in df_age_in.iloc[0]:
        age = age.split()[0]  # remove " years" from string
        if '-' in age:
            age_in_min.append(int(age.split('-')[0]))
            max_entry = np.maximum(max_entry, int(age.split('-')[1]))
        elif '>' in age:
            age_in_min.append(int(age.split('>')[1])+1)
            max_entry = np.maximum(max_entry, int(age.split('>')[1])+1)
        elif '<' in age:
            age_in_min.append(0)
        else:
            print("Undefined entry for one age group in df_age_in")
    if max_entry < max_age:
        age_in_min.append(max_age)

    age_out_min = []
    max_entry = 0
    for age in age_out:
        if "year" in age:
            age = age.split()[0]  # remove " years" from string
        if '-' in age:
            age_out_min.append(int(age.split('-')[0]))
            max_entry = np.maximum(max_entry, int(age.split('-')[1]))
        elif '>' in age:
            age_out_min.append(int(age.split('>')[1])+1)
            max_entry = np.maximum(max_entry, int(age.split('>')[1])+1)
        elif '<' in age:
            age_out_min.append(0)
        else:
            print("Undefined entry for one age group in age_out")
    if max_entry < max_age:
        age_out_min.append(max_age)

    # when no df_population is given, we assume the data is equally distributed
    if df_population is None:
        population = df_age_in.iloc[1].to_numpy()
        age_shares = create_intervals_mapping(age_in_min, age_out_min)
        ans = np.zeros(len(age_out))
        population_indx = 0
        for age_entry_from in age_shares:
            for share_new in age_entry_from:
                ans[share_new[1]] += share_new[0] * population[population_indx]
            population_indx += 1
    else:
        age_pop_min = []
        max_entry = 0
        for age in df_population.iloc[0]:
            if "year" in age:
                age = age.split()[0]  # remove " years" from string
            if '-' in age:
                age_pop_min.append(int(age.split('-')[0]))
                max_entry = np.maximum(max_entry, int(age.split('-')[1]))
            elif '>' in age:
                age_pop_min.append(int(age.split('>')[1])+1)
                max_entry = np.maximum(max_entry, int(age.split('>')[1])+1)
            elif '<' in age:
                age_pop_min.append(0)
            else:
                print("Undefined entry for one age group in population data")
        if max_entry < max_age:
            age_pop_min.append(max_age)

        # get weights from population file
        pop_data = df_population.iloc[1].to_numpy()
        new_pop = np.zeros(df_population.iloc[1].shape[0])
        age_shares_pop = create_intervals_mapping(age_in_min, age_pop_min)
        population_indx = 0
        for age_share in age_shares_pop:
            sum_pop = sum(df_population.iloc[1].to_numpy()[
                          age_share[0][1]:age_share[-1][1]+1])
            for age in age_share:
                new_pop[age[1]] += pop_data[age[1]
                                            ] / sum_pop * df_age_in.iloc[1][population_indx]

            population_indx += 1

        # population is now stored in new_pop for the population age groups
        # now, we need to distribute them to the aim age group
        age_shares = create_intervals_mapping(age_pop_min, age_out_min)
        ans = np.zeros(len(age_out))
        population_indx = 0
        for age_entry_from in age_shares:
            for share_new in age_entry_from:
                ans[share_new[1]] += share_new[0] * new_pop[population_indx]
            population_indx += 1

    return ans

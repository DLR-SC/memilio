#############################################################################
# Copyright (C) 2020-2021 German Aerospace Center (DLR-SC)
#
# Authors: Patrick Lenz
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
@file getHospitalizationData.py
@brief Downloads the hospitalization data of the Robert Koch-Institute (RKI) and provides it in different ways.

The raw hospitalization data we download can be found at
https://github.com/robert-koch-institut/COVID-19-Hospitalisierungen_in_Deutschland
"""

# Imports
import os
import pandas as pd
import numpy as np

from datetime import date, timedelta

from memilio.epidata import getDataIntoPandasDataFrame as gd
from memilio.epidata import defaultDict as dd
from memilio.epidata import modifyDataframeSeries as mdfs

def download_hospitalization_data():
    # RKI content from github
    url = 'https://raw.githubusercontent.com/robert-koch-institut/COVID-19-Hospitalisierungen_in_Deutschland/master/Aktuell_Deutschland_COVID-19-Hospitalisierungen.csv'
    # empty data frame to return if not read correctly
    df = pd.DataFrame()
    # try to read csv
    try:
        df = pd.read_csv(url)
    except Exception:
        print("Error in reading csv while downloading vaccination data.")
        raise
    sanity_checks(df)

    return df

def sanity_checks(df):
    #test if dataframe is empty
    if df.empty:
        raise gd.DataError("Download of Vaccination Data failed. File is empty.")

    actual_strings_list = df.columns.tolist()
    # check number of data categories
    if len(actual_strings_list) != 6:
        raise gd.DataError("Error: Number of data categories changed.")

    # these strings need to be in the header
    test_strings = {
        'Datum', 'Bundesland', 'Bundesland_Id', 'Altersgruppe',
        '7T_Hospitalisierung_Faelle', '7T_Hospitalisierung_Inzidenz'}
    # check if headers are those we want
    for name in test_strings:
        if(name not in actual_strings_list):
            raise gd.DataError("Error: Data categories have changed.")

def compute_hospitailzations_per_day(daily_values, seven_days_values):
    to_split = seven_days_values.copy()
    # start at first known value
    for i in range(7,len(to_split)):
        if to_split[i-1]<to_split[i]:
            daily_values[i] = to_split[i]-to_split[i-1]
            for day in range(7):
                try:
                    to_split[i+day]-=daily_values[i]
                except IndexError:
                    pass
    # backward computation if necessary
    if max(to_split)>0:
        last_seven_days=[0,0,0,0,0,0,0]
        backward = np.zeros(len(daily_values), dtype = float)
        for i in range(len(daily_values)-7):
            last_seven_days.pop(0)
            last_seven_days.append(0)
            backward[-7-i]=to_split[-i-1]-sum(last_seven_days)
            last_seven_days[-1]=backward[-7-i]
        daily_values+=backward

    # check that no negative hospitalizations get into the dataframe
    if len(daily_values[daily_values<0])>0:
        raise gd.DataError('Negative Hospitalizations found.')
    # check that daily values are calculated correctly
    check = np.zeros(len(daily_values)+7, dtype = float)
    for i in range(len(daily_values)):
        for day in range(7):
            check[i+day]+=daily_values[i]
    if sum(check[6:-7]-seven_days_values[6:])>0:
        raise gd.DataError("Can't get hospitalizations per day from incidence.")
    
    return daily_values


def get_hospitalization_data(read_data=dd.defaultDict['read_data'],
                             file_format=dd.defaultDict['file_format'],
                             out_folder=dd.defaultDict['out_folder'],
                             no_raw=dd.defaultDict['no_raw'],
                             start_date=dd.defaultDict['start_date'],
                             end_date=dd.defaultDict['end_date'],
                             impute_dates=dd.defaultDict['impute_dates'],
                             moving_average=dd.defaultDict['moving_average'],
                             make_plot=dd.defaultDict['make_plot']
                             ):
    impute_dates=True
    directory = os.path.join(out_folder, 'Germany/')
    gd.check_dir(directory)

    # get raw dataframe
    filename = "RKIHospitFull"
    if read_data:
        # read json file for already downloaded data
        file_in = os.path.join(directory, filename + ".json")

        try:
            df_raw = pd.read_json(file_in)
        except ValueError:
            raise FileNotFoundError("Error: The file: " + file_in +
                                    " does not exist. Call program without"
                                    " -r flag to get it.")
    else:
        df_raw = download_hospitalization_data()
        if not no_raw:
            gd.write_dataframe(df_raw, directory, filename, file_format)
    df_data = df_raw.copy()
    # drop unwanted columns and rows, rename and sort dataframe
    df_data.rename(dd.GerEng, axis = 1, inplace = True)
    df_data.rename(columns={'Datum': dd.EngEng['date']}, inplace=True)
    df_data = df_data.drop(columns=['State', '7T_Hospitalisierung_Inzidenz'])
    df_data = df_data[df_data[dd.EngEng['idState']]!=0]
    df_data = df_data[df_data[dd.EngEng['ageRKI']]!='00+']
    df_data = df_data.sort_values(by=['Date', 'ID_State', 'Age_RKI']).reset_index(drop=True)
    # impute 6 days before min_date to split up the seven day cases 
    df_data = mdfs.impute_and_reduce_df(
        df_old=df_data,
        group_by_cols={dd.EngEng['idState']:
                       df_data[dd.EngEng['idState']].unique(),
                       dd.EngEng['ageRKI']:
                       df_data[dd.EngEng['ageRKI']].unique()},
        mod_cols=['7T_Hospitalisierung_Faelle'],
        impute='zeros', moving_average=0, min_date=pd.to_datetime(min(df_data.Date)).date()-timedelta(6), max_date='',
        start_w_firstval=False)

    # get data for each day
    # for each state and age group seperately
    df_daily = pd.DataFrame()
    for age in df_data[dd.EngEng['ageRKI']].unique():
        df_age = df_data[df_data[dd.EngEng['ageRKI']]==age]
        for stateid in df_data[dd.EngEng['idState']].unique():
            df_age_stateid = df_age[df_age[dd.EngEng['idState']]==stateid].copy()
            # get hospitalizations per day from incidence
            seven_days_values = df_age_stateid['7T_Hospitalisierung_Faelle'].values
            daily_values = np.zeros(len(seven_days_values), dtype = float)
            daily_values = compute_hospitailzations_per_day(daily_values, seven_days_values)
            # save data in dataframe
            df_age_stateid['hospitalized']=daily_values
            df_age_stateid.drop(['7T_Hospitalisierung_Faelle'], axis = 1)
            df_daily = pd.concat([df_daily.reset_index(drop=True), df_age_stateid.reset_index(drop=True)], join='outer')
    # write dataframe with all states
    df_daily = mdfs.extract_subframe_based_on_dates(df_daily, start_date, end_date)
    filename = gd.append_filename('hospit_all_states', impute_dates, moving_average=0)
    gd.write_dataframe(df_daily, directory, filename, file_format)
    
 

def main():
    """! Main program entry."""

    get_hospitalization_data()

if __name__ == "__main__":

    main()

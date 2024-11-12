#############################################################################
# Copyright (C) 2020-2024 MEmilio
#
# Authors: Lena Ploetzke
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
import math
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import re
import json

colors = ["tab:blue", "tab:orange", "tab:green",
          "tab:red", "tab:purple", "tab:brown"]
fontsize_labels = 14
fontsize_legends = 12


def parse_valgrind_output(input_file, output_file):
    # Define relevant pattern.
    refs_pattern = re.compile(r"refs:\s+([0-9,]+)")
    d1_misses_pattern = re.compile(r"D1\s+misses:\s+([0-9,]+)")
    lld_misses_pattern = re.compile(r"LLd\s+misses:\s+([0-9,]+)")
    # List to store data.
    data = []
    # Open file.
    with open(input_file, 'r') as file:
        lines = file.readlines()

    refs = None
    d1_misses = None
    lld_misses = None
    index = 1

    # Go through all lines of the file.
    for line in lines:
        # Look if matching to refs pattern.
        refs_match = refs_pattern.search(line)
        if refs_match:
            # Remove , and store value.
            refs = refs_match.group(1).replace(',', '')

        # Look if matching to D1-Misses pattern.
        d1_misses_match = d1_misses_pattern.search(line)
        if d1_misses_match:
            d1_misses = d1_misses_match.group(1).replace(
                ',', '')

        # Look if matching to LLd-Misses pattern.
        lld_misses_match = lld_misses_pattern.search(line)
        if lld_misses_match:
            lld_misses = lld_misses_match.group(1).replace(
                ',', '')

        # If all relevant data for one index is found, store them and reset all values.
        if refs and d1_misses and lld_misses:
            data.append({
                'Subcompartments': index,
                'refs': refs,
                'D1_misses': d1_misses,
                'LLd_misses': lld_misses
            })
            refs, d1_misses, lld_misses = None, None, None
            index += 1

    # Save as JSON.
    with open(output_file, 'w') as json_file:
        json.dump(data, json_file, indent=4)


def extract_json_segments(input_file, output_file):
    # Open file and read data.
    with open(input_file, 'r') as file:
        text = file.read()

    # Extract data in {}.
    segments = re.findall(r'\{(.*?)\}', text, re.DOTALL)

    # Convert segments into JSON objects.
    json_data = []
    for segment in segments:
        json_object = json.loads('{' + segment + '}')
        json_data.append(json_object)

    # Save JSON-objects in JSON file.
    with open(output_file, 'w') as json_file:
        json.dump(json_data, json_file, indent=4)


def plot_runtime_ageresolved(jsonfilename, name=''):
    fig = plt.figure()
    df = pd.read_json(jsonfilename)

    for agegroup in [1, 2, 3, 4, 5, 6]:
        df_age = df[(df["Agegroups"] == agegroup)]
        plt.plot(df_age["Subcompartments"], df_age["Time"], linestyle='--', marker='o', linewidth=1.2,
                 label=str(agegroup), color=colors[agegroup-1])
    plt.ylim(bottom=0.)
    plt.legend(loc='center left', bbox_to_anchor=(1, 0.5), fontsize=fontsize_legends,
               title="Number of \nage groups:", title_fontsize=fontsize_legends)
    plt.xlabel('Number of subcompartments', fontsize=fontsize_labels)
    plt.ylabel('Run time [seconds]', fontsize=fontsize_labels)
    plt.grid(True, linestyle='--')
    plt.tight_layout()

    if not os.path.isdir('Plots_time'):
        os.makedirs('Plots_time')
    plt.savefig('Plots_time/run_time_lct'+name +
                '.png', bbox_inches='tight', dpi=500)


def plot_runtime_noage(jsonfilename, name=''):
    fig = plt.figure()
    df = pd.read_json(jsonfilename)

    df_age = df[(df["Agegroups"] == 1)]
    plt.plot(df_age["Subcompartments"], df_age["Time"],
             linestyle='--', marker='o', linewidth=1.2)
    plt.title(
        'Run time for one age group')
    plt.ylim(bottom=0.)
    plt.xlim(left=0., right=df_age["Subcompartments"].max()+1)
    plt.xlabel('Number of subcompartments', fontsize=fontsize_labels)
    plt.ylabel('Run time [seconds]', fontsize=fontsize_labels)
    plt.grid(True, linestyle='--')
    plt.tight_layout()

    if not os.path.isdir('Plots_time'):
        os.makedirs('Plots_time')
    plt.savefig('Plots_time/run_time_lct'+name +
                '.png', bbox_inches='tight', dpi=500)


def plot_cachemisses(jsonfilename, cachelevel=1, savename='', rate=False):
    fig = plt.figure()
    df = pd.read_json(jsonfilename)
    if (cachelevel == 0):
        plt.plot(df["Subcompartments"], df["refs"],
                 linestyle='--', marker='o', linewidth=1.2)
        plt.title(
            'Refs with one age group')
        plt.ylabel('Refs', fontsize=fontsize_labels)
    elif (cachelevel == 1):
        if (rate):
            plt.plot(df["Subcompartments"], df["D1_misses"]/df["refs"]*100,
                     linestyle='--', marker='o', linewidth=1.2)
            plt.title(
                'L1 cache miss rate with one age group')
            plt.ylabel('Cache misses [%]', fontsize=fontsize_labels)
        else:
            plt.plot(df["Subcompartments"], df["D1_misses"],
                     linestyle='--', marker='o', linewidth=1.2)
            plt.title(
                'L1 cache misses with one age group')
            plt.ylabel('Cache misses', fontsize=fontsize_labels)
    else:
        if (rate):
            plt.plot(df["Subcompartments"], df["LLd_misses"]/df["refs"]*100,
                     linestyle='--', marker='o', linewidth=1.2)
            plt.title(
                'LL cache miss rate with one age group')
            plt.ylabel('Cache misses [%]', fontsize=fontsize_labels)
        else:
            plt.plot(df["Subcompartments"], df["LLd_misses"],
                     linestyle='--', marker='o', linewidth=1.2)
            plt.title(
                'LL cache misses with one age group')
            plt.ylabel('Cache misses', fontsize=fontsize_labels)

    plt.ylim(bottom=0.)
    plt.xlim(left=0., right=df["Subcompartments"].max()+1)
    plt.xlabel('Number of subcompartments', fontsize=fontsize_labels)
    plt.grid(True, linestyle='--')
    plt.tight_layout()

    if not os.path.isdir('Plots_time'):
        os.makedirs('Plots_time')
    plt.savefig('Plots_time/'+savename+'.png', bbox_inches='tight', dpi=500)


def main():
    # run times
    jsonfilenameruntimes = 'runtimes/lct_runtimes_85sub_20days.json'
    extract_json_segments(
        'runtimes/lct_runtimes_85sub_20days.txt', jsonfilenameruntimes)
    plot_runtime_noage(jsonfilenameruntimes, '_85sub_20days')

    # Cache
    # jsonfilenamecache = 'runtimes/valgrind_20day_onegroup.json'
    # plot_cachemisses(jsonfilenamecache, 0, 'Refstotal_20days')
    # plot_cachemisses(jsonfilenamecache, 1, 'L1cachemisstotal_20days')
    # plot_cachemisses(jsonfilenamecache, 3, 'LLcachemisstotal_20days')
    # plot_cachemisses(jsonfilenamecache, 1, 'L1cachemissrate_20days', True)
    # plot_cachemisses(jsonfilenamecache, 3, 'LLcachemissrate_20days', True)
    # jsonfilenamecache = 'runtimes/valgrind_85_sub_20days.json'
    # parse_valgrind_output(
    #     'runtimes/valgrind_85_sub_20days.txt', jsonfilenamecache)
    # plot_cachemisses(jsonfilenamecache, 0, 'Refstotal_85sub_20days')


if __name__ == "__main__":
    main()

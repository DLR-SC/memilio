/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Daniel Abele, Henrik Zunker, Martin J. Kuehn
*
* Contact: Martin J. Kuehn <Martin.Kuehn@DLR.de>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
 * generated from https://ferien-api.de/v2/api-docs
 */

#include "memilio/utils/date.h"
#include <utility>
#include <vector>

namespace
{

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_01_sh = {
    {{2017, 4, 7}, {2017, 4, 22}},    {{2017, 5, 26}, {2017, 5, 27}},   {{2017, 7, 24}, {2017, 9, 3}},
    {{2017, 10, 16}, {2017, 10, 28}}, {{2017, 12, 21}, {2018, 1, 7}},   {{2018, 3, 29}, {2018, 4, 14}},
    {{2018, 5, 11}, {2018, 5, 12}},   {{2018, 7, 9}, {2018, 8, 19}},    {{2018, 10, 1}, {2018, 10, 20}},
    {{2018, 12, 21}, {2019, 1, 5}},   {{2019, 4, 4}, {2019, 4, 19}},    {{2019, 5, 31}, {2019, 6, 1}},
    {{2019, 7, 1}, {2019, 8, 11}},    {{2019, 10, 4}, {2019, 10, 19}},  {{2019, 12, 23}, {2020, 1, 7}},
    {{2020, 3, 30}, {2020, 4, 18}},   {{2020, 5, 22}, {2020, 5, 23}},   {{2020, 6, 29}, {2020, 8, 9}},
    {{2020, 10, 5}, {2020, 10, 18}},  {{2020, 12, 21}, {2021, 1, 7}},   {{2021, 4, 1}, {2021, 4, 17}},
    {{2021, 5, 14}, {2021, 5, 16}},   {{2021, 6, 21}, {2021, 8, 1}},    {{2021, 10, 4}, {2021, 10, 17}},
    {{2021, 12, 23}, {2022, 1, 9}},   {{2022, 4, 4}, {2022, 4, 17}},    {{2022, 5, 27}, {2022, 5, 29}},
    {{2022, 7, 4}, {2022, 8, 14}},    {{2022, 10, 10}, {2022, 10, 22}}, {{2022, 12, 23}, {2023, 1, 8}},
    {{2023, 4, 6}, {2023, 4, 22}},    {{2023, 5, 19}, {2023, 5, 20}},   {{2023, 7, 17}, {2023, 8, 26}},
    {{2023, 10, 16}, {2023, 10, 27}}, {{2023, 12, 27}, {2024, 1, 6}},   {{2024, 4, 2}, {2024, 4, 19}},
    {{2024, 5, 10}, {2024, 5, 11}},   {{2024, 7, 22}, {2024, 8, 31}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_02_hh = {
    {{2017, 1, 30}, {2017, 1, 31}}, {{2017, 3, 6}, {2017, 3, 18}},    {{2017, 5, 22}, {2017, 5, 27}},
    {{2017, 7, 20}, {2017, 8, 31}}, {{2017, 10, 16}, {2017, 10, 28}}, {{2017, 12, 22}, {2018, 1, 6}},
    {{2018, 2, 2}, {2018, 2, 3}},   {{2018, 3, 5}, {2018, 3, 17}},    {{2018, 5, 7}, {2018, 5, 12}},
    {{2018, 7, 5}, {2018, 8, 16}},  {{2018, 10, 1}, {2018, 10, 13}},  {{2018, 12, 20}, {2019, 1, 5}},
    {{2019, 2, 1}, {2019, 2, 2}},   {{2019, 3, 4}, {2019, 3, 16}},    {{2019, 5, 13}, {2019, 5, 18}},
    {{2019, 6, 27}, {2019, 8, 8}},  {{2019, 10, 4}, {2019, 10, 19}},  {{2019, 12, 23}, {2020, 1, 4}},
    {{2020, 1, 31}, {2020, 2, 1}},  {{2020, 3, 2}, {2020, 3, 14}},    {{2020, 5, 18}, {2020, 5, 23}},
    {{2020, 6, 25}, {2020, 8, 6}},  {{2020, 10, 5}, {2020, 10, 17}},  {{2020, 12, 21}, {2021, 1, 5}},
    {{2021, 1, 29}, {2021, 1, 30}}, {{2021, 3, 1}, {2021, 3, 13}},    {{2021, 5, 10}, {2021, 5, 15}},
    {{2021, 6, 24}, {2021, 8, 5}},  {{2021, 10, 4}, {2021, 10, 16}},  {{2021, 12, 23}, {2022, 1, 5}},
    {{2022, 1, 28}, {2022, 1, 29}}, {{2022, 3, 7}, {2022, 3, 19}},    {{2022, 5, 23}, {2022, 5, 28}},
    {{2022, 7, 7}, {2022, 8, 18}},  {{2022, 10, 10}, {2022, 10, 22}}, {{2022, 12, 23}, {2023, 1, 7}},
    {{2023, 1, 27}, {2023, 1, 27}}, {{2023, 3, 6}, {2023, 3, 17}},    {{2023, 5, 15}, {2023, 5, 19}},
    {{2023, 7, 13}, {2023, 8, 23}}, {{2023, 10, 2}, {2023, 10, 2}},   {{2023, 10, 16}, {2023, 10, 27}},
    {{2023, 12, 22}, {2024, 1, 5}}, {{2024, 2, 2}, {2024, 2, 2}},     {{2024, 3, 18}, {2024, 3, 28}},
    {{2024, 7, 18}, {2024, 8, 28}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_03_ni = {
    {{2017, 1, 30}, {2017, 2, 1}},    {{2017, 4, 10}, {2017, 4, 23}},   {{2017, 6, 6}, {2017, 6, 7}},
    {{2017, 6, 22}, {2017, 8, 3}},    {{2017, 10, 2}, {2017, 10, 14}},  {{2017, 12, 22}, {2018, 1, 6}},
    {{2018, 3, 19}, {2018, 4, 4}},    {{2018, 4, 30}, {2018, 5, 1}},    {{2018, 6, 28}, {2018, 8, 9}},
    {{2018, 10, 1}, {2018, 10, 13}},  {{2018, 12, 24}, {2019, 1, 5}},   {{2019, 1, 31}, {2019, 2, 2}},
    {{2019, 4, 8}, {2019, 4, 24}},    {{2019, 5, 31}, {2019, 6, 1}},    {{2019, 7, 4}, {2019, 8, 15}},
    {{2019, 10, 4}, {2019, 10, 19}},  {{2019, 12, 23}, {2020, 1, 7}},   {{2020, 2, 3}, {2020, 2, 5}},
    {{2020, 3, 30}, {2020, 4, 15}},   {{2020, 5, 22}, {2020, 5, 23}},   {{2020, 7, 16}, {2020, 8, 27}},
    {{2020, 10, 12}, {2020, 10, 24}}, {{2020, 12, 23}, {2021, 1, 9}},   {{2021, 2, 1}, {2021, 2, 3}},
    {{2021, 3, 29}, {2021, 4, 10}},   {{2021, 5, 14}, {2021, 5, 15}},   {{2021, 7, 22}, {2021, 9, 2}},
    {{2021, 10, 18}, {2021, 10, 30}}, {{2021, 12, 23}, {2022, 1, 8}},   {{2022, 1, 31}, {2022, 2, 2}},
    {{2022, 4, 4}, {2022, 4, 20}},    {{2022, 5, 27}, {2022, 5, 28}},   {{2022, 7, 14}, {2022, 8, 25}},
    {{2022, 10, 17}, {2022, 10, 29}}, {{2022, 12, 23}, {2023, 1, 7}},   {{2023, 1, 30}, {2023, 1, 31}},
    {{2023, 3, 27}, {2023, 4, 11}},   {{2023, 5, 19}, {2023, 5, 19}},   {{2023, 7, 6}, {2023, 8, 16}},
    {{2023, 10, 2}, {2023, 10, 2}},   {{2023, 10, 16}, {2023, 10, 27}}, {{2023, 12, 27}, {2024, 1, 5}},
    {{2024, 2, 1}, {2024, 2, 2}},     {{2024, 3, 18}, {2024, 3, 28}},   {{2024, 6, 24}, {2024, 8, 2}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_04_hb = {
    {{2017, 1, 30}, {2017, 2, 1}},  {{2017, 4, 10}, {2017, 4, 23}},   {{2017, 6, 6}, {2017, 6, 7}},
    {{2017, 6, 22}, {2017, 8, 3}},  {{2017, 10, 14}, {2017, 10, 31}}, {{2017, 12, 23}, {2018, 1, 7}},
    {{2018, 2, 1}, {2018, 2, 3}},   {{2018, 3, 19}, {2018, 4, 4}},    {{2018, 4, 30}, {2018, 5, 1}},
    {{2018, 6, 28}, {2018, 8, 9}},  {{2018, 10, 1}, {2018, 10, 14}},  {{2018, 12, 24}, {2019, 1, 5}},
    {{2019, 1, 31}, {2019, 2, 2}},  {{2019, 4, 6}, {2019, 4, 24}},    {{2019, 5, 31}, {2019, 6, 1}},
    {{2019, 7, 4}, {2019, 8, 15}},  {{2019, 10, 4}, {2019, 10, 19}},  {{2019, 12, 21}, {2020, 1, 7}},
    {{2020, 2, 3}, {2020, 2, 5}},   {{2020, 3, 28}, {2020, 4, 15}},   {{2020, 5, 22}, {2020, 5, 23}},
    {{2020, 7, 16}, {2020, 8, 27}}, {{2020, 10, 12}, {2020, 10, 25}}, {{2020, 12, 23}, {2021, 1, 9}},
    {{2021, 2, 1}, {2021, 2, 3}},   {{2021, 3, 27}, {2021, 4, 11}},   {{2021, 5, 14}, {2021, 5, 15}},
    {{2021, 7, 22}, {2021, 9, 2}},  {{2021, 10, 18}, {2021, 10, 31}}, {{2021, 12, 23}, {2022, 1, 9}},
    {{2022, 1, 31}, {2022, 2, 2}},  {{2022, 4, 4}, {2022, 4, 20}},    {{2022, 5, 27}, {2022, 5, 28}},
    {{2022, 7, 14}, {2022, 8, 25}}, {{2022, 10, 17}, {2022, 10, 30}}, {{2022, 12, 23}, {2023, 1, 7}},
    {{2023, 1, 30}, {2023, 1, 31}}, {{2023, 3, 27}, {2023, 4, 11}},   {{2023, 5, 19}, {2023, 5, 19}},
    {{2023, 7, 6}, {2023, 8, 16}},  {{2023, 10, 2}, {2023, 10, 2}},   {{2023, 10, 16}, {2023, 10, 28}},
    {{2023, 12, 23}, {2024, 1, 5}}, {{2024, 2, 1}, {2024, 2, 2}},     {{2024, 3, 18}, {2024, 4, 2}},
    {{2024, 6, 24}, {2024, 8, 2}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_05_nw = {
    {{2017, 4, 10}, {2017, 4, 23}},   {{2017, 6, 6}, {2017, 6, 7}},     {{2017, 7, 17}, {2017, 8, 30}},
    {{2017, 10, 23}, {2017, 11, 5}},  {{2017, 12, 27}, {2018, 1, 7}},   {{2018, 3, 26}, {2018, 4, 8}},
    {{2018, 5, 22}, {2018, 5, 26}},   {{2018, 7, 16}, {2018, 8, 29}},   {{2018, 10, 15}, {2018, 10, 28}},
    {{2018, 12, 21}, {2019, 1, 5}},   {{2019, 4, 15}, {2019, 4, 28}},   {{2019, 6, 11}, {2019, 6, 12}},
    {{2019, 7, 15}, {2019, 8, 28}},   {{2019, 10, 14}, {2019, 10, 27}}, {{2019, 12, 23}, {2020, 1, 7}},
    {{2020, 4, 6}, {2020, 4, 19}},    {{2020, 6, 2}, {2020, 6, 3}},     {{2020, 6, 29}, {2020, 8, 12}},
    {{2020, 10, 12}, {2020, 10, 25}}, {{2020, 12, 23}, {2021, 1, 7}},   {{2021, 3, 29}, {2021, 4, 11}},
    {{2021, 5, 25}, {2021, 5, 26}},   {{2021, 7, 5}, {2021, 8, 18}},    {{2021, 10, 11}, {2021, 10, 24}},
    {{2021, 12, 24}, {2022, 1, 9}},   {{2022, 4, 11}, {2022, 4, 24}},   {{2022, 6, 27}, {2022, 8, 10}},
    {{2022, 10, 4}, {2022, 10, 16}},  {{2022, 12, 23}, {2023, 1, 7}},   {{2023, 4, 3}, {2023, 4, 15}},
    {{2023, 5, 30}, {2023, 5, 30}},   {{2023, 6, 22}, {2023, 8, 4}},    {{2023, 10, 2}, {2023, 10, 14}},
    {{2023, 12, 21}, {2024, 1, 5}},   {{2024, 3, 25}, {2024, 4, 6}},    {{2024, 5, 21}, {2024, 5, 21}},
    {{2024, 7, 8}, {2024, 8, 20}},    {{2024, 10, 14}, {2024, 10, 26}}, {{2024, 12, 23}, {2025, 1, 6}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_06_he = {
    {{2017, 4, 3}, {2017, 4, 16}},    {{2017, 7, 3}, {2017, 8, 12}},   {{2017, 10, 9}, {2017, 10, 22}},
    {{2017, 12, 24}, {2018, 1, 14}},  {{2018, 3, 26}, {2018, 4, 8}},   {{2018, 6, 25}, {2018, 8, 4}},
    {{2018, 10, 1}, {2018, 10, 14}},  {{2018, 12, 24}, {2019, 1, 13}}, {{2019, 4, 14}, {2019, 4, 28}},
    {{2019, 7, 1}, {2019, 8, 10}},    {{2019, 9, 30}, {2019, 10, 13}}, {{2019, 12, 23}, {2020, 1, 12}},
    {{2020, 4, 6}, {2020, 4, 19}},    {{2020, 7, 6}, {2020, 8, 15}},   {{2020, 10, 5}, {2020, 10, 18}},
    {{2020, 12, 21}, {2021, 1, 10}},  {{2021, 4, 6}, {2021, 4, 17}},   {{2021, 4, 6}, {2021, 4, 17}},
    {{2021, 7, 19}, {2021, 8, 28}},   {{2021, 7, 19}, {2021, 8, 28}},  {{2021, 10, 11}, {2021, 10, 24}},
    {{2021, 10, 11}, {2021, 10, 24}}, {{2021, 12, 23}, {2022, 1, 9}},  {{2021, 12, 23}, {2022, 1, 9}},
    {{2022, 4, 11}, {2022, 4, 24}},   {{2022, 7, 25}, {2022, 9, 3}},   {{2022, 10, 24}, {2022, 10, 30}},
    {{2022, 12, 22}, {2023, 1, 8}},   {{2023, 4, 3}, {2023, 4, 22}},   {{2023, 7, 24}, {2023, 9, 1}},
    {{2023, 10, 23}, {2023, 10, 28}}, {{2023, 12, 27}, {2024, 1, 13}}, {{2024, 3, 25}, {2024, 4, 13}},
    {{2024, 7, 15}, {2024, 8, 23}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_07_rp = {
    {{2017, 4, 10}, {2017, 4, 22}},   {{2017, 7, 3}, {2017, 8, 12}},    {{2017, 10, 2}, {2017, 10, 14}},
    {{2017, 12, 22}, {2018, 1, 10}},  {{2018, 3, 26}, {2018, 4, 7}},    {{2018, 6, 26}, {2018, 8, 4}},
    {{2018, 10, 1}, {2018, 10, 13}},  {{2018, 12, 20}, {2019, 1, 5}},   {{2019, 2, 25}, {2019, 3, 2}},
    {{2019, 4, 23}, {2019, 5, 1}},    {{2019, 7, 1}, {2019, 8, 10}},    {{2019, 9, 30}, {2019, 10, 12}},
    {{2019, 12, 23}, {2020, 1, 7}},   {{2020, 2, 17}, {2020, 2, 22}},   {{2020, 4, 9}, {2020, 4, 18}},
    {{2020, 7, 6}, {2020, 8, 15}},    {{2020, 10, 12}, {2020, 10, 24}}, {{2020, 12, 21}, {2021, 1, 1}},
    {{2021, 3, 29}, {2021, 4, 7}},    {{2021, 5, 25}, {2021, 6, 3}},    {{2021, 7, 19}, {2021, 8, 28}},
    {{2021, 10, 11}, {2021, 10, 23}}, {{2021, 12, 23}, {2022, 1, 1}},   {{2022, 2, 21}, {2022, 2, 26}},
    {{2022, 4, 13}, {2022, 4, 23}},   {{2022, 7, 25}, {2022, 9, 3}},    {{2022, 10, 17}, {2022, 11, 1}},
    {{2022, 12, 23}, {2023, 1, 3}},   {{2023, 4, 3}, {2023, 4, 6}},     {{2023, 5, 30}, {2023, 6, 7}},
    {{2023, 7, 24}, {2023, 9, 1}},    {{2023, 10, 16}, {2023, 10, 27}}, {{2023, 12, 27}, {2024, 1, 5}},
    {{2024, 3, 25}, {2024, 4, 2}},    {{2024, 5, 21}, {2024, 5, 29}},   {{2024, 7, 15}, {2024, 8, 23}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_08_bw = {
    {{2017, 4, 10}, {2017, 4, 22}},   {{2017, 6, 6}, {2017, 6, 17}},    {{2017, 7, 27}, {2017, 9, 10}},
    {{2017, 10, 30}, {2017, 11, 4}},  {{2017, 12, 22}, {2018, 1, 6}},   {{2018, 3, 26}, {2018, 4, 7}},
    {{2018, 5, 22}, {2018, 6, 3}},    {{2018, 7, 26}, {2018, 9, 9}},    {{2018, 10, 29}, {2018, 11, 3}},
    {{2018, 12, 24}, {2019, 1, 6}},   {{2019, 4, 15}, {2019, 4, 28}},   {{2019, 6, 11}, {2019, 6, 22}},
    {{2019, 7, 29}, {2019, 9, 11}},   {{2019, 10, 28}, {2019, 10, 31}}, {{2019, 12, 23}, {2020, 1, 5}},
    {{2020, 4, 6}, {2020, 4, 19}},    {{2020, 6, 2}, {2020, 6, 14}},    {{2020, 7, 30}, {2020, 9, 13}},
    {{2020, 10, 26}, {2020, 10, 31}}, {{2020, 12, 23}, {2021, 1, 10}},  {{2021, 4, 1}, {2021, 4, 2}},
    {{2021, 5, 25}, {2021, 6, 6}},    {{2021, 7, 29}, {2021, 9, 12}},   {{2021, 10, 31}, {2021, 11, 1}},
    {{2021, 12, 23}, {2022, 1, 9}},   {{2022, 4, 14}, {2022, 4, 15}},   {{2022, 6, 7}, {2022, 6, 19}},
    {{2022, 7, 28}, {2022, 9, 11}},   {{2022, 10, 31}, {2022, 11, 1}},  {{2022, 12, 21}, {2023, 1, 8}},
    {{2023, 4, 6}, {2023, 4, 6}},     {{2023, 4, 11}, {2023, 4, 15}},   {{2023, 5, 30}, {2023, 6, 9}},
    {{2023, 7, 27}, {2023, 9, 9}},    {{2023, 10, 30}, {2023, 11, 3}},  {{2023, 12, 23}, {2024, 1, 5}},
    {{2024, 3, 23}, {2024, 4, 5}},    {{2024, 5, 21}, {2024, 5, 31}},   {{2024, 7, 25}, {2024, 9, 7}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_09_by = {
    {{2017, 2, 27}, {2017, 3, 4}},  {{2017, 4, 10}, {2017, 4, 23}},  {{2017, 6, 6}, {2017, 6, 17}},
    {{2017, 7, 29}, {2017, 9, 12}}, {{2017, 10, 30}, {2017, 11, 4}}, {{2017, 12, 23}, {2018, 1, 6}},
    {{2018, 2, 12}, {2018, 2, 17}}, {{2018, 3, 26}, {2018, 4, 7}},   {{2018, 5, 22}, {2018, 6, 3}},
    {{2018, 7, 30}, {2018, 9, 11}}, {{2018, 10, 29}, {2018, 11, 3}}, {{2018, 12, 22}, {2019, 1, 6}},
    {{2019, 3, 4}, {2019, 3, 9}},   {{2019, 4, 15}, {2019, 4, 28}},  {{2019, 6, 11}, {2019, 6, 22}},
    {{2019, 7, 29}, {2019, 9, 10}}, {{2019, 10, 28}, {2019, 11, 1}}, {{2019, 12, 23}, {2020, 1, 5}},
    {{2020, 2, 24}, {2020, 2, 29}}, {{2020, 4, 6}, {2020, 4, 19}},   {{2020, 6, 2}, {2020, 6, 14}},
    {{2020, 7, 27}, {2020, 9, 8}},  {{2020, 10, 31}, {2020, 11, 7}}, {{2020, 12, 23}, {2021, 1, 10}},
    {{2021, 2, 15}, {2021, 2, 20}}, {{2021, 3, 29}, {2021, 4, 11}},  {{2021, 5, 25}, {2021, 6, 5}},
    {{2021, 7, 30}, {2021, 9, 14}}, {{2021, 11, 2}, {2021, 11, 6}},  {{2021, 12, 24}, {2022, 1, 9}},
    {{2022, 2, 28}, {2022, 3, 5}},  {{2022, 4, 11}, {2022, 4, 24}},  {{2022, 6, 7}, {2022, 6, 19}},
    {{2022, 8, 1}, {2022, 9, 13}},  {{2022, 10, 31}, {2022, 11, 5}}, {{2022, 12, 24}, {2023, 1, 8}},
    {{2023, 2, 20}, {2023, 2, 24}}, {{2023, 4, 3}, {2023, 4, 15}},   {{2023, 5, 30}, {2023, 6, 9}},
    {{2023, 7, 31}, {2023, 9, 11}}, {{2023, 10, 30}, {2023, 11, 3}}, {{2023, 11, 22}, {2023, 11, 22}},
    {{2023, 12, 23}, {2024, 1, 5}}, {{2024, 2, 12}, {2024, 2, 16}},  {{2024, 3, 25}, {2024, 4, 6}},
    {{2024, 5, 21}, {2024, 6, 1}},  {{2024, 7, 29}, {2024, 9, 9}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_10_sl = {
    {{2017, 2, 27}, {2017, 3, 5}},   {{2017, 4, 10}, {2017, 4, 23}},   {{2017, 7, 3}, {2017, 8, 15}},
    {{2017, 10, 2}, {2017, 10, 15}}, {{2017, 12, 21}, {2018, 1, 6}},   {{2018, 2, 12}, {2018, 2, 18}},
    {{2018, 3, 26}, {2018, 4, 7}},   {{2018, 6, 25}, {2018, 8, 4}},    {{2019, 2, 25}, {2019, 3, 12}},
    {{2019, 4, 17}, {2019, 4, 27}},  {{2019, 7, 1}, {2019, 8, 10}},    {{2019, 10, 7}, {2019, 10, 19}},
    {{2019, 12, 23}, {2020, 1, 4}},  {{2020, 2, 17}, {2020, 2, 26}},   {{2020, 4, 14}, {2020, 4, 25}},
    {{2020, 7, 6}, {2020, 8, 15}},   {{2020, 10, 12}, {2020, 10, 24}}, {{2020, 12, 21}, {2021, 1, 1}},
    {{2021, 2, 15}, {2021, 2, 20}},  {{2021, 3, 29}, {2021, 4, 8}},    {{2021, 5, 25}, {2021, 5, 29}},
    {{2021, 7, 19}, {2021, 8, 28}},  {{2021, 10, 18}, {2021, 10, 30}}, {{2021, 12, 23}, {2022, 1, 4}},
    {{2022, 2, 21}, {2022, 3, 2}},   {{2022, 4, 14}, {2022, 4, 23}},   {{2022, 6, 7}, {2022, 6, 11}},
    {{2022, 7, 25}, {2022, 9, 3}},   {{2022, 10, 24}, {2022, 11, 5}},  {{2022, 12, 22}, {2023, 1, 5}},
    {{2023, 2, 20}, {2023, 2, 24}},  {{2023, 4, 3}, {2023, 4, 12}},    {{2023, 5, 30}, {2023, 6, 2}},
    {{2023, 7, 24}, {2023, 9, 1}},   {{2023, 10, 23}, {2023, 11, 3}},  {{2023, 12, 21}, {2024, 1, 2}},
    {{2024, 2, 12}, {2024, 2, 16}},  {{2024, 3, 25}, {2024, 4, 5}},    {{2024, 5, 21}, {2024, 5, 24}},
    {{2024, 7, 15}, {2024, 8, 23}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_11_be = {
    {{2017, 1, 30}, {2017, 2, 5}},    {{2017, 4, 10}, {2017, 4, 19}},  {{2017, 5, 24}, {2017, 5, 25}},
    {{2017, 7, 20}, {2017, 9, 2}},    {{2017, 10, 23}, {2017, 11, 5}}, {{2017, 12, 21}, {2018, 1, 3}},
    {{2018, 2, 5}, {2018, 2, 11}},    {{2018, 3, 26}, {2018, 4, 7}},   {{2018, 4, 30}, {2018, 5, 1}},
    {{2018, 7, 5}, {2018, 8, 18}},    {{2018, 10, 22}, {2018, 11, 3}}, {{2018, 12, 22}, {2019, 1, 6}},
    {{2019, 2, 4}, {2019, 2, 10}},    {{2019, 4, 15}, {2019, 4, 27}},  {{2019, 6, 20}, {2019, 8, 3}},
    {{2019, 10, 4}, {2019, 10, 5}},   {{2019, 12, 23}, {2020, 1, 5}},  {{2020, 2, 3}, {2020, 2, 9}},
    {{2020, 4, 6}, {2020, 4, 18}},    {{2020, 5, 8}, {2020, 5, 9}},    {{2020, 6, 25}, {2020, 8, 8}},
    {{2020, 10, 12}, {2020, 10, 25}}, {{2020, 12, 21}, {2021, 1, 3}},  {{2021, 2, 1}, {2021, 2, 7}},
    {{2021, 3, 29}, {2021, 4, 11}},   {{2021, 5, 14}, {2021, 5, 15}},  {{2021, 6, 24}, {2021, 8, 7}},
    {{2021, 10, 11}, {2021, 10, 24}}, {{2021, 12, 24}, {2022, 1, 1}},  {{2022, 1, 29}, {2022, 2, 6}},
    {{2022, 4, 11}, {2022, 4, 24}},   {{2022, 5, 27}, {2022, 5, 28}},  {{2022, 7, 7}, {2022, 8, 20}},
    {{2022, 10, 24}, {2022, 11, 6}},  {{2022, 12, 22}, {2023, 1, 3}},  {{2023, 1, 30}, {2023, 2, 4}},
    {{2023, 4, 3}, {2023, 4, 14}},    {{2023, 5, 19}, {2023, 5, 19}},  {{2023, 7, 13}, {2023, 8, 25}},
    {{2023, 10, 2}, {2023, 10, 2}},   {{2023, 10, 23}, {2023, 11, 4}}, {{2023, 12, 23}, {2024, 1, 5}},
    {{2024, 2, 5}, {2024, 2, 10}},    {{2024, 3, 25}, {2024, 4, 5}},   {{2024, 5, 10}, {2024, 5, 10}},
    {{2024, 7, 18}, {2024, 8, 30}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_12_bb = {
    {{2017, 1, 30}, {2017, 2, 5}},   {{2017, 4, 12}, {2017, 4, 23}},   {{2017, 5, 26}, {2017, 5, 27}},
    {{2017, 7, 20}, {2017, 9, 2}},   {{2017, 10, 23}, {2017, 11, 5}},  {{2017, 12, 21}, {2018, 1, 3}},
    {{2018, 2, 5}, {2018, 2, 11}},   {{2018, 3, 26}, {2018, 4, 7}},    {{2018, 5, 11}, {2018, 5, 12}},
    {{2018, 7, 5}, {2018, 8, 19}},   {{2018, 10, 22}, {2018, 11, 3}},  {{2018, 12, 21}, {2019, 1, 6}},
    {{2019, 2, 4}, {2019, 2, 10}},   {{2019, 4, 15}, {2019, 4, 27}},   {{2019, 6, 20}, {2019, 8, 4}},
    {{2019, 10, 4}, {2019, 10, 19}}, {{2019, 12, 23}, {2020, 1, 4}},   {{2020, 2, 3}, {2020, 2, 9}},
    {{2020, 4, 6}, {2020, 4, 18}},   {{2020, 6, 25}, {2020, 8, 9}},    {{2020, 10, 12}, {2020, 10, 25}},
    {{2020, 12, 21}, {2021, 1, 3}},  {{2021, 2, 1}, {2021, 2, 7}},     {{2021, 3, 29}, {2021, 4, 10}},
    {{2021, 6, 24}, {2021, 8, 8}},   {{2021, 10, 11}, {2021, 10, 24}}, {{2021, 12, 23}, {2022, 1, 1}},
    {{2022, 1, 31}, {2022, 2, 6}},   {{2022, 4, 11}, {2022, 4, 24}},   {{2022, 7, 7}, {2022, 8, 21}},
    {{2022, 10, 24}, {2022, 11, 6}}, {{2022, 12, 22}, {2023, 1, 4}},   {{2023, 1, 30}, {2023, 2, 3}},
    {{2023, 4, 3}, {2023, 4, 14}},   {{2023, 7, 13}, {2023, 8, 26}},   {{2023, 10, 23}, {2023, 11, 4}},
    {{2023, 12, 23}, {2024, 1, 5}},  {{2024, 2, 5}, {2024, 2, 9}},     {{2024, 3, 25}, {2024, 4, 5}},
    {{2024, 7, 18}, {2024, 8, 31}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_13_mv = {
    {{2017, 2, 6}, {2017, 2, 19}},    {{2017, 4, 10}, {2017, 4, 20}},   {{2017, 6, 2}, {2017, 6, 7}},
    {{2017, 7, 24}, {2017, 9, 3}},    {{2017, 10, 23}, {2017, 10, 31}}, {{2017, 12, 21}, {2018, 1, 4}},
    {{2018, 2, 5}, {2018, 2, 17}},    {{2018, 3, 26}, {2018, 4, 5}},    {{2018, 5, 11}, {2018, 5, 12}},
    {{2018, 7, 9}, {2018, 8, 19}},    {{2018, 10, 8}, {2018, 10, 14}},  {{2018, 12, 24}, {2019, 1, 6}},
    {{2019, 2, 4}, {2019, 2, 16}},    {{2019, 4, 15}, {2019, 4, 25}},   {{2019, 5, 22}, {2019, 5, 23}},
    {{2019, 7, 1}, {2019, 8, 11}},    {{2019, 10, 4}, {2019, 10, 5}},   {{2019, 12, 23}, {2020, 1, 5}},
    {{2020, 2, 10}, {2020, 2, 22}},   {{2020, 4, 6}, {2020, 4, 16}},    {{2020, 5, 22}, {2020, 5, 23}},
    {{2020, 6, 22}, {2020, 8, 2}},    {{2020, 10, 5}, {2020, 10, 11}},  {{2020, 12, 21}, {2021, 1, 3}},
    {{2021, 2, 6}, {2021, 2, 19}},    {{2021, 3, 29}, {2021, 4, 8}},    {{2021, 5, 14}, {2021, 5, 15}},
    {{2021, 6, 21}, {2021, 8, 1}},    {{2021, 10, 5}, {2021, 10, 11}},  {{2021, 12, 22}, {2022, 1, 1}},
    {{2022, 2, 5}, {2022, 2, 18}},    {{2022, 4, 11}, {2022, 4, 21}},   {{2022, 5, 27}, {2022, 5, 28}},
    {{2022, 7, 4}, {2022, 8, 14}},    {{2022, 10, 10}, {2022, 10, 15}}, {{2022, 12, 22}, {2023, 1, 3}},
    {{2023, 2, 6}, {2023, 2, 18}},    {{2023, 4, 3}, {2023, 4, 12}},    {{2023, 5, 19}, {2023, 5, 19}},
    {{2023, 5, 26}, {2023, 5, 30}},   {{2023, 7, 17}, {2023, 8, 26}},   {{2023, 10, 9}, {2023, 10, 14}},
    {{2023, 10, 30}, {2023, 10, 30}}, {{2023, 12, 21}, {2024, 1, 3}},   {{2024, 2, 5}, {2024, 2, 16}},
    {{2024, 3, 25}, {2024, 4, 3}},    {{2024, 7, 22}, {2024, 8, 31}},   {{2024, 12, 23}, {2025, 1, 6}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_14_sn = {
    {{2017, 2, 13}, {2017, 2, 25}}, {{2017, 4, 13}, {2017, 4, 23}},   {{2017, 5, 26}, {2017, 5, 27}},
    {{2017, 6, 26}, {2017, 8, 5}},  {{2017, 10, 2}, {2017, 10, 15}},  {{2017, 12, 23}, {2018, 1, 3}},
    {{2018, 2, 12}, {2018, 2, 24}}, {{2018, 3, 29}, {2018, 4, 7}},    {{2018, 5, 11}, {2018, 5, 12}},
    {{2018, 7, 2}, {2018, 8, 11}},  {{2018, 10, 8}, {2018, 10, 21}},  {{2018, 12, 19}, {2019, 1, 5}},
    {{2019, 2, 18}, {2019, 3, 3}},  {{2019, 4, 19}, {2019, 4, 27}},   {{2019, 5, 31}, {2019, 6, 1}},
    {{2019, 7, 8}, {2019, 8, 17}},  {{2019, 10, 14}, {2019, 10, 26}}, {{2019, 12, 21}, {2020, 1, 4}},
    {{2020, 2, 10}, {2020, 2, 23}}, {{2020, 4, 10}, {2020, 4, 19}},   {{2020, 5, 22}, {2020, 5, 23}},
    {{2020, 7, 20}, {2020, 8, 29}}, {{2020, 10, 19}, {2020, 11, 1}},  {{2020, 12, 23}, {2021, 1, 3}},
    {{2021, 2, 8}, {2021, 2, 21}},  {{2021, 4, 2}, {2021, 4, 11}},    {{2021, 5, 14}, {2021, 5, 15}},
    {{2021, 7, 26}, {2021, 9, 4}},  {{2021, 10, 18}, {2021, 10, 31}}, {{2021, 12, 23}, {2022, 1, 2}},
    {{2022, 2, 12}, {2022, 2, 27}}, {{2022, 4, 15}, {2022, 4, 24}},   {{2022, 5, 27}, {2022, 5, 28}},
    {{2022, 7, 18}, {2022, 8, 27}}, {{2022, 10, 17}, {2022, 10, 30}}, {{2022, 12, 22}, {2023, 1, 3}},
    {{2023, 2, 13}, {2023, 2, 24}}, {{2023, 4, 7}, {2023, 4, 15}},    {{2023, 5, 19}, {2023, 5, 19}},
    {{2023, 7, 10}, {2023, 8, 18}}, {{2023, 10, 2}, {2023, 10, 14}},  {{2023, 10, 30}, {2023, 10, 30}},
    {{2023, 12, 23}, {2024, 1, 2}}, {{2024, 2, 12}, {2024, 2, 23}},   {{2024, 3, 28}, {2024, 4, 5}},
    {{2024, 6, 20}, {2024, 8, 2}},  {{2024, 10, 7}, {2024, 10, 19}},  {{2024, 12, 23}, {2025, 1, 3}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_15_st = {
    {{2017, 2, 4}, {2017, 2, 12}},  {{2017, 4, 10}, {2017, 4, 14}},   {{2017, 5, 26}, {2017, 5, 27}},
    {{2017, 6, 26}, {2017, 8, 10}}, {{2017, 10, 2}, {2017, 10, 14}},  {{2017, 12, 21}, {2018, 1, 4}},
    {{2018, 2, 5}, {2018, 2, 10}},  {{2018, 3, 26}, {2018, 4, 1}},    {{2018, 5, 11}, {2018, 5, 20}},
    {{2018, 6, 28}, {2018, 8, 9}},  {{2018, 10, 1}, {2018, 10, 13}},  {{2018, 12, 19}, {2019, 1, 5}},
    {{2019, 2, 11}, {2019, 2, 16}}, {{2019, 4, 18}, {2019, 5, 1}},    {{2019, 5, 31}, {2019, 6, 2}},
    {{2019, 7, 4}, {2019, 8, 15}},  {{2019, 10, 4}, {2019, 10, 12}},  {{2019, 12, 23}, {2020, 1, 5}},
    {{2020, 2, 10}, {2020, 2, 15}}, {{2020, 4, 6}, {2020, 4, 12}},    {{2020, 5, 18}, {2020, 5, 31}},
    {{2020, 7, 16}, {2020, 8, 27}}, {{2020, 10, 19}, {2020, 10, 25}}, {{2020, 12, 21}, {2021, 1, 6}},
    {{2021, 2, 8}, {2021, 2, 14}},  {{2021, 3, 29}, {2021, 4, 4}},    {{2021, 5, 10}, {2021, 5, 23}},
    {{2021, 7, 22}, {2021, 9, 2}},  {{2021, 10, 25}, {2021, 10, 31}}, {{2021, 12, 22}, {2022, 1, 9}},
    {{2022, 2, 12}, {2022, 2, 20}}, {{2022, 4, 11}, {2022, 4, 17}},   {{2022, 5, 23}, {2022, 5, 29}},
    {{2022, 7, 14}, {2022, 8, 25}}, {{2022, 10, 24}, {2022, 11, 5}},  {{2022, 12, 21}, {2023, 1, 6}},
    {{2023, 2, 6}, {2023, 2, 11}},  {{2023, 4, 3}, {2023, 4, 8}},     {{2023, 5, 15}, {2023, 5, 19}},
    {{2023, 7, 6}, {2023, 8, 16}},  {{2023, 10, 2}, {2023, 10, 2}},   {{2023, 10, 16}, {2023, 10, 30}},
    {{2023, 12, 21}, {2024, 1, 3}}, {{2024, 2, 5}, {2024, 2, 10}},    {{2024, 3, 25}, {2024, 3, 30}},
    {{2024, 5, 21}, {2024, 5, 24}}, {{2024, 6, 24}, {2024, 8, 3}},    {{2024, 12, 23}, {2025, 1, 4}}};

static const std::vector<std::pair<mio::Date, mio::Date>> holidays_16_th = {
    {{2017, 2, 6}, {2017, 2, 12}},  {{2017, 4, 10}, {2017, 4, 22}},   {{2017, 5, 26}, {2017, 5, 27}},
    {{2017, 6, 26}, {2017, 8, 10}}, {{2017, 10, 2}, {2017, 10, 15}},  {{2017, 12, 22}, {2018, 1, 6}},
    {{2018, 2, 5}, {2018, 2, 10}},  {{2018, 3, 26}, {2018, 4, 8}},    {{2018, 5, 11}, {2018, 5, 12}},
    {{2018, 7, 2}, {2018, 8, 12}},  {{2018, 10, 1}, {2018, 10, 14}},  {{2018, 12, 21}, {2019, 1, 5}},
    {{2019, 2, 11}, {2019, 2, 16}}, {{2019, 4, 15}, {2019, 4, 28}},   {{2019, 5, 31}, {2019, 6, 1}},
    {{2019, 7, 8}, {2019, 8, 18}},  {{2019, 10, 7}, {2019, 10, 20}},  {{2019, 12, 21}, {2020, 1, 4}},
    {{2020, 2, 10}, {2020, 2, 15}}, {{2020, 4, 6}, {2020, 4, 19}},    {{2020, 5, 22}, {2020, 5, 23}},
    {{2020, 7, 20}, {2020, 8, 30}}, {{2020, 10, 17}, {2020, 10, 31}}, {{2020, 12, 23}, {2021, 1, 3}},
    {{2021, 2, 8}, {2021, 2, 14}},  {{2021, 3, 29}, {2021, 4, 11}},   {{2021, 5, 14}, {2021, 5, 15}},
    {{2021, 7, 26}, {2021, 9, 5}},  {{2021, 10, 25}, {2021, 11, 7}},  {{2021, 12, 23}, {2022, 1, 1}},
    {{2022, 2, 12}, {2022, 2, 20}}, {{2022, 4, 11}, {2022, 4, 24}},   {{2022, 5, 27}, {2022, 5, 28}},
    {{2022, 7, 18}, {2022, 8, 28}}, {{2022, 10, 17}, {2022, 10, 30}}, {{2022, 12, 22}, {2023, 1, 4}},
    {{2023, 2, 13}, {2023, 2, 17}}, {{2023, 4, 3}, {2023, 4, 15}},    {{2023, 5, 19}, {2023, 5, 19}},
    {{2023, 7, 10}, {2023, 8, 19}}, {{2023, 10, 2}, {2023, 10, 14}},  {{2023, 12, 22}, {2024, 1, 5}},
    {{2024, 2, 12}, {2024, 2, 16}}, {{2024, 3, 25}, {2024, 4, 6}},    {{2024, 5, 10}, {2024, 5, 10}},
    {{2024, 6, 20}, {2024, 7, 31}}};

} //namespace

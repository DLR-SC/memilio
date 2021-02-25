#include <epidemiology/utils/date.h>
#include <gtest/gtest.h>

TEST(TestDate, init)
{
    auto date = epi::Date(2021, 3, 12);
    EXPECT_EQ(date.year, 2021);
    EXPECT_EQ(date.month, 3);
    EXPECT_EQ(date.day, 12);
}

TEST(TestDate, comparison)
{
    EXPECT_EQ(epi::Date(2021, 3, 12), epi::Date(2021, 3, 12));
    EXPECT_NE(epi::Date(2021, 5, 11), epi::Date(2021, 5, 12));
    EXPECT_NE(epi::Date(2021, 5, 11), epi::Date(2021, 6, 11));
    EXPECT_NE(epi::Date(2021, 5, 11), epi::Date(2022, 5, 11));
}

TEST(TestDate, offsetByDays)
{
    EXPECT_EQ(epi::offset_date_by_days({2020, 8, 30}, 3), epi::Date(2020, 9, 2));
    EXPECT_EQ(epi::offset_date_by_days({2024, 8, 24}, 6), epi::Date(2024, 8, 30));
    EXPECT_EQ(epi::offset_date_by_days({2024, 8, 24}, -6), epi::Date(2024, 8, 18));
    EXPECT_EQ(epi::offset_date_by_days({2020, 2, 28}, 1), epi::Date(2020, 2, 29));
    EXPECT_EQ(epi::offset_date_by_days({2021, 2, 28}, 1), epi::Date(2021, 3, 1));
    EXPECT_EQ(epi::offset_date_by_days({2021, 2, 3}, -5), epi::Date(2021, 1, 29));
    EXPECT_EQ(epi::offset_date_by_days({2021, 2, 28}, 0), epi::Date(2021, 2, 28));
    EXPECT_EQ(epi::offset_date_by_days({2020, 12, 31}, 1), epi::Date(2021, 1, 1));
    EXPECT_EQ(epi::offset_date_by_days({2021, 12, 31}, 1), epi::Date(2022, 1, 1));
    EXPECT_EQ(epi::offset_date_by_days({2019, 12, 31}, 367), epi::Date(2021, 1, 1));
    EXPECT_EQ(epi::offset_date_by_days({2021, 12, 31}, 366), epi::Date(2023, 1, 1));
    EXPECT_EQ(epi::offset_date_by_days({2021, 1, 1}, -1), epi::Date(2020, 12, 31));
    EXPECT_EQ(epi::offset_date_by_days({2022, 1, 1}, -1), epi::Date(2021, 12, 31));
    EXPECT_EQ(epi::offset_date_by_days({2021, 1, 1}, -367), epi::Date(2019, 12, 31));
    EXPECT_EQ(epi::offset_date_by_days({2022, 1, 1}, -366), epi::Date(2020, 12, 31));
}

TEST(TestDate, parse)
{
    EXPECT_EQ(epi::parse_date("2020.09.02"), epi::Date(2020, 9, 2));
    EXPECT_EQ(epi::parse_date("2021.08.30"), epi::Date(2021, 8, 30));
    EXPECT_EQ(epi::parse_date("2020.02.29"), epi::Date(2020, 2, 29));
    EXPECT_EQ(epi::parse_date("2021.03.01"), epi::Date(2021, 3, 1));
    EXPECT_EQ(epi::parse_date("2021.02.28"), epi::Date(2021, 2, 28));
}

TEST(TestDate, getDayInYear)
{
    auto day = epi::get_day_in_year({2020, 1, 21});
    EXPECT_EQ(day, 21);

    day = epi::get_day_in_year({2020, 2, 14});
    EXPECT_EQ(day, 45);

    day = epi::get_day_in_year({2020, 3, 2});
    EXPECT_EQ(day, 62);

    day = epi::get_day_in_year({2020, 12, 27});
    EXPECT_EQ(day, 362);

    day = epi::get_day_in_year({2021, 1, 21});
    EXPECT_EQ(day, 21);

    day = epi::get_day_in_year({2021, 2, 14});
    EXPECT_EQ(day, 45);

    day = epi::get_day_in_year({2021, 3, 2});
    EXPECT_EQ(day, 61);

    day = epi::get_day_in_year({2021, 12, 27});
    EXPECT_EQ(day, 361);
}

TEST(TestDate, getOffset)
{
    auto offset = epi::get_offset_in_days({2020, 8, 30}, {2020, 8, 15});
    EXPECT_EQ(offset, 15);

    offset = epi::get_offset_in_days({2020, 8, 30}, {2020, 8, 31});
    EXPECT_EQ(offset, -1);

    offset = epi::get_offset_in_days({2020, 9, 12}, {2020, 8, 30});
    EXPECT_EQ(offset, 13);

    offset = epi::get_offset_in_days({2020, 7, 25}, {2020, 5, 25});
    EXPECT_EQ(offset, 61);

    offset = epi::get_offset_in_days({2021, 1, 3}, {2020, 12, 31});
    EXPECT_EQ(offset, 3);

    offset = epi::get_offset_in_days({2021, 3, 3}, {2020, 12, 29});
    EXPECT_EQ(offset, 64);

    offset = epi::get_offset_in_days({2021, 11, 30}, {2020, 11, 30});
    EXPECT_EQ(offset, 365);

    offset = epi::get_offset_in_days({2025, 11, 30}, {2020, 11, 30});
    EXPECT_EQ(offset, 5 * 365 + 1);

    offset = epi::get_offset_in_days({2019, 11, 30}, {2020, 11, 30});
    EXPECT_EQ(offset, -366);
}
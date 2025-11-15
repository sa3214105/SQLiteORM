#pragma once
#include <gtest/gtest.h>
#include "Common.hpp"

class AggregateFunctionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 插入測試數據
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("Alice"),
            UserTable::MakeTableColumn<AgeColumn>(25),
            UserTable::MakeTableColumn<ScoreColumn>(85.5)
        );
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("Bob"),
            UserTable::MakeTableColumn<AgeColumn>(30),
            UserTable::MakeTableColumn<ScoreColumn>(92.0)
        );
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("Charlie"),
            UserTable::MakeTableColumn<AgeColumn>(25),
            UserTable::MakeTableColumn<ScoreColumn>(78.5)
        );
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("David"),
            UserTable::MakeTableColumn<AgeColumn>(35),
            UserTable::MakeTableColumn<ScoreColumn>(88.0)
        );
    }

    void TearDown() override {
        std::remove("test_aggregate.db");
    }

    Database<UserTable> db{"test_aggregate.db"};
};

// 測試 COUNT 函數
TEST_F(AggregateFunctionTest, CountFunction) {
    auto results = db.GetTable<UserTable>().Select<
        Count<UserTable::TableColumn<NameColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto count = std::get<0>(results[0]);
    EXPECT_EQ(count.value, 4);
}

// 測試 AVG 函數
TEST_F(AggregateFunctionTest, AvgFunction) {
    auto results = db.GetTable<UserTable>().Select<
        Avg<UserTable::TableColumn<ScoreColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto avg_score = std::get<0>(results[0]);
    EXPECT_NEAR(avg_score.value, 86.0, 0.1); // (85.5 + 92.0 + 78.5 + 88.0) / 4
}

// 測試 MAX 函數
TEST_F(AggregateFunctionTest, MaxFunction) {
    auto results = db.GetTable<UserTable>().Select<
        Max<UserTable::TableColumn<AgeColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto max_age = std::get<0>(results[0]);
    EXPECT_EQ(max_age.value, 35);
}

// 測試 MIN 函數
TEST_F(AggregateFunctionTest, MinFunction) {
    auto results = db.GetTable<UserTable>().Select<
        Min<UserTable::TableColumn<ScoreColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto min_score = std::get<0>(results[0]);
    EXPECT_NEAR(min_score.value, 78.5, 0.1);
}

// 測試 SUM 函數
TEST_F(AggregateFunctionTest, SumFunction) {
    auto results = db.GetTable<UserTable>().Select<
        Sum<UserTable::TableColumn<AgeColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto sum_age = std::get<0>(results[0]);
    EXPECT_EQ(sum_age.value, 115); // 25 + 30 + 25 + 35
}

// 測試 TOTAL 函數
TEST_F(AggregateFunctionTest, TotalFunction) {
    auto results = db.GetTable<UserTable>().Select<
        Total<UserTable::TableColumn<ScoreColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto total_score = std::get<0>(results[0]);
    EXPECT_NEAR(total_score.value, 344.0, 0.1); // 85.5 + 92.0 + 78.5 + 88.0
}

// 測試多個聚合函數組合
TEST_F(AggregateFunctionTest, MultipleAggregateFunctions) {
    auto results = db.GetTable<UserTable>().Select<
        Count<UserTable::TableColumn<NameColumn> >,
        Avg<UserTable::TableColumn<AgeColumn> >,
        Max<UserTable::TableColumn<ScoreColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto [count, avg_age, max_score] = results[0];
    EXPECT_EQ(count.value, 4);
    EXPECT_NEAR(avg_age.value, 28.75, 0.1); // (25 + 30 + 25 + 35) / 4
    EXPECT_NEAR(max_score.value, 92.0, 0.1);
}

// 測試聚合函數與 WHERE 條件組合
TEST_F(AggregateFunctionTest, AggregateFunctionWithWhere) {
    auto results = db.GetTable<UserTable>().Select<
                Avg<UserTable::TableColumn<ScoreColumn> >
            >()
            .Where(GreaterThan<UserTable::TableColumn<AgeColumn> >(25))
            .Results();

    ASSERT_EQ(results.size(), 1);
    auto avg_score = std::get<0>(results[0]);
    EXPECT_NEAR(avg_score.value, 90.0, 0.1); // (92.0 + 88.0) / 2
}

// 測試 GROUP BY 單列
TEST_F(AggregateFunctionTest, GroupBySingleColumn) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Count<UserTable::TableColumn<NameColumn> >
            >()
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .Results();

    ASSERT_EQ(results.size(), 3); // 3 個不同的年齡組

    for (const auto &[age, count]: results) {
        if (age.value == 25) {
            EXPECT_EQ(count.value, 2); // Alice, Charlie
        } else if (age.value == 30) {
            EXPECT_EQ(count.value, 1); // Bob
        } else if (age.value == 35) {
            EXPECT_EQ(count.value, 1); // David
        }
    }
}

// 測試 GROUP BY 與多個聚合函數
TEST_F(AggregateFunctionTest, GroupByWithMultipleAggregates) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Count<UserTable::TableColumn<NameColumn> >,
                Avg<UserTable::TableColumn<ScoreColumn> >,
                Min<UserTable::TableColumn<ScoreColumn> >,
                Max<UserTable::TableColumn<ScoreColumn> >
            >()
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .Results();

    ASSERT_EQ(results.size(), 3);

    for (const auto &[age, count, avg_score, min_score, max_score]: results) {
        if (age.value == 25) {
            EXPECT_EQ(count.value, 2);
            EXPECT_NEAR(avg_score.value, 82.0, 0.1); // (85.5 + 78.5) / 2
            EXPECT_NEAR(min_score.value, 78.5, 0.1);
            EXPECT_NEAR(max_score.value, 85.5, 0.1);
        } else if (age.value == 30) {
            EXPECT_EQ(count.value, 1);
            EXPECT_NEAR(avg_score.value, 92.0, 0.1);
            EXPECT_NEAR(min_score.value, 92.0, 0.1);
            EXPECT_NEAR(max_score.value, 92.0, 0.1);
        } else if (age.value == 35) {
            EXPECT_EQ(count.value, 1);
            EXPECT_NEAR(avg_score.value, 88.0, 0.1);
            EXPECT_NEAR(min_score.value, 88.0, 0.1);
            EXPECT_NEAR(max_score.value, 88.0, 0.1);
        }
    }
}

// 測試 GROUP BY 與 WHERE 組合
TEST_F(AggregateFunctionTest, GroupByWithWhere) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Count<UserTable::TableColumn<NameColumn> >,
                Avg<UserTable::TableColumn<ScoreColumn> >
            >()
            .Where(GreaterThan<UserTable::TableColumn<ScoreColumn> >(80.0))
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .Results();

    // 只有 score > 80 的記錄: Alice(25, 85.5), Bob(30, 92.0), David(35, 88.0)
    ASSERT_EQ(results.size(), 3);

    for (const auto &[age, count, avg_score]: results) {
        if (age.value == 25) {
            EXPECT_EQ(count.value, 1); // 只有 Alice
            EXPECT_NEAR(avg_score.value, 85.5, 0.1);
        } else if (age.value == 30) {
            EXPECT_EQ(count.value, 1); // Bob
            EXPECT_NEAR(avg_score.value, 92.0, 0.1);
        } else if (age.value == 35) {
            EXPECT_EQ(count.value, 1); // David
            EXPECT_NEAR(avg_score.value, 88.0, 0.1);
        }
    }
}

// 測試 SUM 與 GROUP BY
TEST_F(AggregateFunctionTest, SumWithGroupBy) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Sum<UserTable::TableColumn<ScoreColumn> >
            >()
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .Results();

    ASSERT_EQ(results.size(), 3);

    for (const auto &[age, sum_score]: results) {
        if (age.value == 25) {
            EXPECT_NEAR(sum_score.value, 164.0, 0.1); // 85.5 + 78.5
        } else if (age.value == 30) {
            EXPECT_NEAR(sum_score.value, 92.0, 0.1);
        } else if (age.value == 35) {
            EXPECT_NEAR(sum_score.value, 88.0, 0.1);
        }
    }
}

// 測試 MIN 和 MAX 組合
TEST_F(AggregateFunctionTest, MinMaxCombination) {
    auto results = db.GetTable<UserTable>().Select<
        Min<UserTable::TableColumn<ScoreColumn> >,
        Max<UserTable::TableColumn<ScoreColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto [min_score, max_score] = results[0];
    EXPECT_NEAR(min_score.value, 78.5, 0.1);
    EXPECT_NEAR(max_score.value, 92.0, 0.1);
}

// 測試空表的聚合函數
TEST_F(AggregateFunctionTest, AggregateFunctionOnEmptyTable) {
    db.GetTable<UserTable>().Delete().Execute();

    auto results = db.GetTable<UserTable>().Select<
        Count<UserTable::TableColumn<NameColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto count = std::get<0>(results[0]);
    EXPECT_EQ(count.value, 0);
}

// 測試 COUNT 與 WHERE 條件
TEST_F(AggregateFunctionTest, CountWithWhere) {
    auto results = db.GetTable<UserTable>().Select<
                Count<UserTable::TableColumn<NameColumn> >
            >()
            .Where(Equal<UserTable::TableColumn<AgeColumn> >(25))
            .Results();

    ASSERT_EQ(results.size(), 1);
    auto count = std::get<0>(results[0]);
    EXPECT_EQ(count.value, 2);
}

// 測試 TOTAL 與 SUM 的差異
TEST_F(AggregateFunctionTest, TotalVsSum) {
    auto results = db.GetTable<UserTable>().Select<
        Sum<UserTable::TableColumn<ScoreColumn> >,
        Total<UserTable::TableColumn<ScoreColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto [sum_score, total_score] = results[0];
    // SUM 和 TOTAL 在沒有 NULL 值時應該相等
    EXPECT_NEAR(sum_score.value, total_score.value, 0.1);
}

// 測試所有基本聚合函數組合
TEST_F(AggregateFunctionTest, AllBasicAggregateFunctions) {
    auto results = db.GetTable<UserTable>().Select<
        Count<UserTable::TableColumn<NameColumn> >,
        Sum<UserTable::TableColumn<AgeColumn> >,
        Avg<UserTable::TableColumn<ScoreColumn> >,
        Min<UserTable::TableColumn<ScoreColumn> >,
        Max<UserTable::TableColumn<ScoreColumn> >,
        Total<UserTable::TableColumn<AgeColumn> >
    >().Results();

    ASSERT_EQ(results.size(), 1);
    auto [count, sum_age, avg_score, min_score, max_score, total_age] = results[0];

    EXPECT_EQ(count.value, 4);
    EXPECT_EQ(sum_age.value, 115);
    EXPECT_NEAR(avg_score.value, 86.0, 0.1);
    EXPECT_NEAR(min_score.value, 78.5, 0.1);
    EXPECT_NEAR(max_score.value, 92.0, 0.1);
    EXPECT_NEAR(total_age.value, 115.0, 0.1);
}

// 測試 AVG 與 WHERE 條件的精確計算
TEST_F(AggregateFunctionTest, AvgWithWhereExactCalculation) {
    auto results = db.GetTable<UserTable>().Select<
                Count<UserTable::TableColumn<NameColumn> >,
                Avg<UserTable::TableColumn<ScoreColumn> >,
                Sum<UserTable::TableColumn<ScoreColumn> >
            >()
            .Where(Equal<UserTable::TableColumn<AgeColumn> >(25))
            .Results();

    ASSERT_EQ(results.size(), 1);
    auto [count, avg_score, sum_score] = results[0];

    EXPECT_EQ(count.value, 2);
    EXPECT_NEAR(avg_score.value, 82.0, 0.1); // (85.5 + 78.5) / 2
    EXPECT_NEAR(sum_score.value, 164.0, 0.1); // 85.5 + 78.5
}

// 測試 MIN/MAX 與條件組合
TEST_F(AggregateFunctionTest, MinMaxWithCondition) {
    auto results = db.GetTable<UserTable>().Select<
                Min<UserTable::TableColumn<AgeColumn> >,
                Max<UserTable::TableColumn<AgeColumn> >
            >()
            .Where(GreaterThan<UserTable::TableColumn<ScoreColumn> >(80.0))
            .Results();

    ASSERT_EQ(results.size(), 1);
    auto [min_age, max_age] = results[0];

    EXPECT_EQ(min_age.value, 25); // Alice
    EXPECT_EQ(max_age.value, 35); // David
}

// 測試 COUNT 在不同條件下的結果
TEST_F(AggregateFunctionTest, CountWithDifferentConditions) {
    // 測試大於條件
    auto results1 = db.GetTable<UserTable>().Select<
                Count<UserTable::TableColumn<NameColumn> >
            >()
            .Where(GreaterThan<UserTable::TableColumn<AgeColumn> >(25))
            .Results();
    EXPECT_EQ(std::get<0>(results1[0]).value, 2); // Bob, David

    // 測試大於等於條件
    auto results2 = db.GetTable<UserTable>().Select<
                Count<UserTable::TableColumn<NameColumn> >
            >()
            .Where(GreaterThanEqual<UserTable::TableColumn<AgeColumn> >(25))
            .Results();
    EXPECT_EQ(std::get<0>(results2[0]).value, 4); // All

    // 測試小於條件
    auto results3 = db.GetTable<UserTable>().Select<
                Count<UserTable::TableColumn<NameColumn> >
            >()
            .Where(LessThan<UserTable::TableColumn<AgeColumn> >(30))
            .Results();
    EXPECT_EQ(std::get<0>(results3[0]).value, 2); // Alice, Charlie
}

// 測試 SUM 在不同數據類型上的使用
TEST_F(AggregateFunctionTest, SumOnDifferentTypes) {
    // Integer 類型
    auto results_int = db.GetTable<UserTable>().Select<
        Sum<UserTable::TableColumn<AgeColumn> >
    >().Results();
    EXPECT_EQ(std::get<0>(results_int[0]).value, 115);

    // Real 類型
    auto results_real = db.GetTable<UserTable>().Select<
        Sum<UserTable::TableColumn<ScoreColumn> >
    >().Results();
    EXPECT_NEAR(std::get<0>(results_real[0]).value, 344.0, 0.1);
}

// 測試聚合函數與 LIMIT 組合
TEST_F(AggregateFunctionTest, AggregateFunctionWithLimit) {
    // 對聚合函數使用 LIMIT 應該能正常工作
    auto results = db.GetTable<UserTable>().Select<
                Count<UserTable::TableColumn<NameColumn> >,
                Avg<UserTable::TableColumn<ScoreColumn> >
            >()
            .LimitOffset(1)
            .Results();

    ASSERT_EQ(results.size(), 1);
    auto [count, avg_score] = results[0];
    EXPECT_EQ(count.value, 4);
    EXPECT_NEAR(avg_score.value, 86.0, 0.1);
}

// 測試 GROUP BY 與 LIMIT 組合
TEST_F(AggregateFunctionTest, GroupByWithLimit) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Count<UserTable::TableColumn<NameColumn> >
            >()
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .LimitOffset(2)
            .Results();

    ASSERT_EQ(results.size(), 2); // 限制只返回 2 組
}

// 測試 GROUP BY 與 LIMIT OFFSET 組合
TEST_F(AggregateFunctionTest, GroupByWithLimitOffset) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Count<UserTable::TableColumn<NameColumn> >
            >()
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .LimitOffset(1, 1)
            .Results();

    ASSERT_EQ(results.size(), 1); // 跳過第一組，只返回 1 組
}

// 測試複雜的聚合查詢
TEST_F(AggregateFunctionTest, ComplexAggregateQuery) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Count<UserTable::TableColumn<NameColumn> >,
                Sum<UserTable::TableColumn<ScoreColumn> >,
                Avg<UserTable::TableColumn<ScoreColumn> >,
                Min<UserTable::TableColumn<ScoreColumn> >,
                Max<UserTable::TableColumn<ScoreColumn> >
            >()
            .Where(GreaterThan<UserTable::TableColumn<ScoreColumn> >(75))
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .Results();

    ASSERT_GT(results.size(), 0);

    for (const auto &[age, count, sum, avg, min, max]: results) {
        // 驗證聚合計算的一致性
        EXPECT_NEAR(avg.value, sum.value / count.value, 0.1);
        EXPECT_LE(min.value, avg.value);
        EXPECT_GE(max.value, avg.value);
    }
}

// 測試 TOTAL 與 GROUP BY
TEST_F(AggregateFunctionTest, TotalWithGroupBy) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Total<UserTable::TableColumn<ScoreColumn> >
            >()
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .Results();

    ASSERT_EQ(results.size(), 3);

    for (const auto &[age, total]: results) {
        if (age.value == 25) {
            EXPECT_NEAR(total.value, 164.0, 0.1); // 85.5 + 78.5
        } else if (age.value == 30) {
            EXPECT_NEAR(total.value, 92.0, 0.1);
        } else if (age.value == 35) {
            EXPECT_NEAR(total.value, 88.0, 0.1);
        }
    }
}

// 測試空表的 GROUP BY
TEST_F(AggregateFunctionTest, GroupByOnEmptyTable) {
    db.GetTable<UserTable>().Delete().Execute();

    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Count<UserTable::TableColumn<NameColumn> >
            >()
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .Results();

    ASSERT_EQ(results.size(), 0); // 空表應該返回空結果
}

// 測試 GROUP BY 後所有聚合函數
TEST_F(AggregateFunctionTest, AllAggregateFunctionsWithGroupBy) {
    auto results = db.GetTable<UserTable>().Select<
                UserTable::TableColumn<AgeColumn>,
                Count<UserTable::TableColumn<NameColumn> >,
                Sum<UserTable::TableColumn<ScoreColumn> >,
                Avg<UserTable::TableColumn<ScoreColumn> >,
                Min<UserTable::TableColumn<ScoreColumn> >,
                Max<UserTable::TableColumn<ScoreColumn> >,
                Total<UserTable::TableColumn<ScoreColumn> >
            >()
            .GroupBy<UserTable::TableColumn<AgeColumn> >()
            .Results();

    ASSERT_EQ(results.size(), 3);

    for (const auto &[age, count, sum, avg, min, max, total]: results) {
        // 驗證聚合函數之間的一致性
        EXPECT_NEAR(avg.value, sum.value / count.value, 0.1);
        EXPECT_NEAR(sum.value, total.value, 0.1);
        EXPECT_LE(min.value, avg.value);
        EXPECT_GE(max.value, avg.value);
        EXPECT_LE(min.value, max.value);
    }
}

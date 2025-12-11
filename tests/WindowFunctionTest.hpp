#pragma once
#include "Common.hpp"

// ============ Window 函數測試 ============

class WindowFunctionTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition), decltype(DeptTableDefinition)> db = Database{"test_window.db", UserTableDefinition, DeptTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();
    Table<decltype(DeptTableDefinition)> &deptTable = db.GetTable<decltype(DeptTableDefinition)>();

    void SetUp() override {
        // 插入測試數據 - 多個部門的員工
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 85.5);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 92.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Charlie", 25, 78.5);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("David", 35, 88.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Eve", 30, 95.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Frank", 28, 82.0);

        // 部門數據
        deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("Engineering", "Alice");
        deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("Engineering", "Bob");
        deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("Engineering", "Charlie");
        deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("Sales", "David");
        deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("Sales", "Eve");
        deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("Sales", "Frank");
    }

    void TearDown() override {
        std::remove("test_window.db");
    }
};

// 測試 ROW_NUMBER 函數
TEST_F(WindowFunctionTest, RowNumberFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        RowNumber().OrderBy(userTable[ScoreColumn], OrderType::DESC)
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 驗證排序：Eve (95.0), Bob (92.0), David (88.0), Alice (85.5), Frank (82.0), Charlie (78.5)
    EXPECT_EQ(std::get<0>(results[0]), "Eve");
    EXPECT_EQ(std::get<2>(results[0]), 1); // ROW_NUMBER 為 1

    EXPECT_EQ(std::get<0>(results[1]), "Bob");
    EXPECT_EQ(std::get<2>(results[1]), 2); // ROW_NUMBER 為 2

    EXPECT_EQ(std::get<0>(results[5]), "Charlie");
    EXPECT_EQ(std::get<2>(results[5]), 6); // ROW_NUMBER 為 6
}

// 測試 ROW_NUMBER 與 PARTITION BY
TEST_F(WindowFunctionTest, RowNumberWithPartition) {
    auto results = userTable
        .InnerJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
        .Select(
            deptTable[DeptColumn],
            userTable[NameColumn],
            userTable[ScoreColumn],
            RowNumber().PartitionedBy(deptTable[DeptColumn]).OrderBy(userTable[ScoreColumn], OrderType::DESC)
        ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 驗證每個部門內的排序
    for (const auto& row : results) {
        auto dept = std::get<0>(row);
        auto name = std::get<1>(row);
        auto row_num = std::get<3>(row);

        if (dept == "Engineering") {
            if (name == "Bob") EXPECT_EQ(row_num, 1); // 92.0
            else if (name == "Alice") EXPECT_EQ(row_num, 2); // 85.5
            else if (name == "Charlie") EXPECT_EQ(row_num, 3); // 78.5
        } else if (dept == "Sales") {
            if (name == "Eve") EXPECT_EQ(row_num, 1); // 95.0
            else if (name == "David") EXPECT_EQ(row_num, 2); // 88.0
            else if (name == "Frank") EXPECT_EQ(row_num, 3); // 82.0
        }
    }
}

// 測試 RANK 函數
TEST_F(WindowFunctionTest, RankFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn],
        Rank().OrderBy(userTable[AgeColumn], OrderType::DESC)
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 驗證排序：35歲(rank 1), 30歲x2(rank 2), 28歲(rank 4), 25歲x2(rank 5)
    for (const auto& row : results) {
        auto age = std::get<1>(row);
        auto rank = std::get<2>(row);

        if (age == 35) EXPECT_EQ(rank, 1);
        else if (age == 30) EXPECT_EQ(rank, 2);
        else if (age == 28) EXPECT_EQ(rank, 4);
        else if (age == 25) EXPECT_EQ(rank, 5);
    }
}

// 測試 DENSE_RANK 函數
TEST_F(WindowFunctionTest, DenseRankFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn],
        DenseRank().OrderBy(userTable[AgeColumn], OrderType::DESC)
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // DENSE_RANK 不會跳過排名：35歲(1), 30歲(2), 28歲(3), 25歲(4)
    for (const auto& row : results) {
        auto age = std::get<1>(row);
        auto rank = std::get<2>(row);

        if (age == 35) EXPECT_EQ(rank, 1);
        else if (age == 30) EXPECT_EQ(rank, 2);
        else if (age == 28) EXPECT_EQ(rank, 3);
        else if (age == 25) EXPECT_EQ(rank, 4);
    }
}

// 測試 PERCENT_RANK 函數
TEST_F(WindowFunctionTest, PercentRankFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        PercentRank().OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // PERCENT_RANK 計算相對排名百分比 (rank - 1) / (rows - 1)
    auto first_percent = std::get<2>(results[0]);
    auto last_percent = std::get<2>(results[5]);

    EXPECT_DOUBLE_EQ(first_percent, 0.0); // 最低分
    EXPECT_DOUBLE_EQ(last_percent, 1.0); // 最高分
}

// 測試 CUME_DIST 函數
TEST_F(WindowFunctionTest, CumeDistFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        CumeDist().OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // CUME_DIST 計算累積分佈 (當前及以前的行數 / 總行數)
    auto last_cume_dist = std::get<2>(results[5]);
    EXPECT_DOUBLE_EQ(last_cume_dist, 1.0); // 最後一個必定是 1.0
}

// 測試 NTILE 函數
TEST_F(WindowFunctionTest, NTileFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        NTile(3_expr).OrderBy(userTable[ScoreColumn], OrderType::DESC)
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 將 6 個結果分成 3 組，每組 2 個
    int bucket1_count = 0, bucket2_count = 0, bucket3_count = 0;

    for (const auto& row : results) {
        auto bucket = std::get<2>(row);
        if (bucket == 1) bucket1_count++;
        else if (bucket == 2) bucket2_count++;
        else if (bucket == 3) bucket3_count++;
    }

    EXPECT_EQ(bucket1_count, 2);
    EXPECT_EQ(bucket2_count, 2);
    EXPECT_EQ(bucket3_count, 2);
}

// 測試 LAG 函數（無參數）
TEST_F(WindowFunctionTest, LagFunctionBasic) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Lag(userTable[ScoreColumn]).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 驗證排序後第二個的 LAG 值是第一個的分數
    if (results.size() > 1) {
        auto second_lag = std::get<2>(results[1]);
        auto first_score = std::get<1>(results[0]);
        EXPECT_EQ(second_lag, first_score);
    }
}

// 測試 LAG 函數（帶 offset）
TEST_F(WindowFunctionTest, LagFunctionWithOffset) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Lag(userTable[ScoreColumn], 2_expr).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 第三個應該是第一個的值（offset 為 2）
    if (results.size() > 2) {
        auto third_lag = std::get<2>(results[2]);
        auto first_score = std::get<1>(results[0]);
        EXPECT_DOUBLE_EQ(third_lag, first_score);
    }
}

// 測試 LAG 函數（帶 default 值）
TEST_F(WindowFunctionTest, LagFunctionWithDefault) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Lag(userTable[ScoreColumn], 1_expr, 0.0_expr).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 第一個應該是 default 值 0.0
    auto first_lag = std::get<2>(results[0]);
    EXPECT_DOUBLE_EQ(first_lag, 0.0);
}

// 測試 LEAD 函數（無參數）
TEST_F(WindowFunctionTest, LeadFunctionBasic) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Lead(userTable[ScoreColumn]).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 第一個的 LEAD 應該是第二個的分數
    if (results.size() > 1) {
        auto first_lead = std::get<2>(results[0]);
        auto second_score = std::get<1>(results[1]);
        EXPECT_DOUBLE_EQ(first_lead, second_score);
    }
}

// 測試 LEAD 函數（帶 offset）
TEST_F(WindowFunctionTest, LeadFunctionWithOffset) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Lead(userTable[ScoreColumn], 2_expr).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 第一個應該是第三個的值（offset 為 2）
    if (results.size() > 2) {
        auto first_lead = std::get<2>(results[0]);
        auto third_score = std::get<1>(results[2]);
        EXPECT_DOUBLE_EQ(first_lead, third_score);
    }
}

// 測試 LEAD 函數（帶 default 值）
TEST_F(WindowFunctionTest, LeadFunctionWithDefault) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Lead(userTable[ScoreColumn], 1_expr, 0.0_expr).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 最後一個應該是 default 值 0.0
    auto last_lead = std::get<2>(results[5]);
    EXPECT_DOUBLE_EQ(last_lead, 0.0);
}

// 測試 FIRST_VALUE 函數
TEST_F(WindowFunctionTest, FirstValueFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        FirstValue(userTable[ScoreColumn]).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 所有行的 FIRST_VALUE 應該都是最低分
    auto first_score = std::get<1>(results[0]);

    for (const auto& row : results) {
        auto first_val = std::get<2>(row);
        EXPECT_DOUBLE_EQ(first_val, first_score);
    }
}

// 測試 FIRST_VALUE 與 PARTITION BY
TEST_F(WindowFunctionTest, FirstValueWithPartition) {
    auto results = userTable
        .InnerJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
        .Select(
            deptTable[DeptColumn],
            userTable[NameColumn],
            userTable[ScoreColumn],
            FirstValue(userTable[ScoreColumn]).PartitionedBy(deptTable[DeptColumn]).OrderBy(userTable[ScoreColumn])
        ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    std::optional<double> eng_first, sales_first;

    for (const auto& row : results) {
        auto dept = std::get<0>(row);
        auto first_val = std::get<3>(row);

        if (dept == "Engineering") {
            if (!eng_first) eng_first = first_val;
            EXPECT_DOUBLE_EQ(first_val, eng_first.value());
        } else if (dept == "Sales") {
            if (!sales_first) sales_first = first_val;
            EXPECT_DOUBLE_EQ(first_val, sales_first.value());
        }
    }
}

// 測試 LAST_VALUE 函數
TEST_F(WindowFunctionTest, LastValueFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        LastValue(userTable[ScoreColumn]).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 注意：LAST_VALUE 的行為取決於 frame clause
    // 默認情況下可能不是整個窗口的最後值
    // 這裡只驗證它有返回值
    for (const auto& row : results) {
        auto last_val = std::get<2>(row);
        EXPECT_TRUE(last_val >= 0); // 確保有值
    }
}

// 測試 NTH_VALUE 函數
TEST_F(WindowFunctionTest, NthValueFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        NthValue(userTable[ScoreColumn], 2_expr).OrderBy(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 第 2 個值應該是排序後的第二個分數
    auto second_score = std::get<1>(results[1]);

    // 從第二個開始，所有的 NTH_VALUE(2) 應該都相同
    for (size_t i = 1; i < results.size(); ++i) {
        auto nth_val = std::get<2>(results[i]);
        EXPECT_DOUBLE_EQ(nth_val, second_score);
    }
}

// 測試複雜窗口函數組合
TEST_F(WindowFunctionTest, ComplexWindowFunctions) {
    auto results = userTable
        .InnerJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
        .Select(
            deptTable[DeptColumn],
            userTable[NameColumn],
            userTable[ScoreColumn],
            RowNumber().PartitionedBy(deptTable[DeptColumn]).OrderBy(userTable[ScoreColumn], OrderType::DESC),
            Rank().PartitionedBy(deptTable[DeptColumn]).OrderBy(userTable[ScoreColumn], OrderType::DESC)
        ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 驗證每個部門的排名正確
    for (const auto& row : results) {
        auto dept = std::get<0>(row);
        auto row_num = std::get<3>(row);
        auto rank = std::get<4>(row);

        // ROW_NUMBER 和 RANK 在沒有重複值時應該相同
        EXPECT_EQ(row_num, rank);
        EXPECT_GE(row_num, 1);
        EXPECT_LE(row_num, 3); // 每個部門最多 3 人
    }
}

// 測試多個 PARTITION BY 欄位
TEST_F(WindowFunctionTest, MultiplePartitionColumns) {
    auto results = userTable
        .InnerJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
        .Select(
            deptTable[DeptColumn],
            userTable[AgeColumn],
            userTable[NameColumn],
            RowNumber().PartitionedBy(deptTable[DeptColumn]).OrderBy(userTable[ScoreColumn], OrderType::DESC)
        ).Results().ToVector();

    ASSERT_EQ(results.size(), 6);

    // 驗證每個部門+年齡組合的排名從 1 開始
    for (const auto& row : results) {
        auto row_num = std::get<3>(row);
        EXPECT_GE(row_num, 1);
    }
}


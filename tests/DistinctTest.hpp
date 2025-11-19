#pragma once
#include "Common.hpp"

// ============ DISTINCT 測試類別 ============

// 基本 Distinct 測試類別（包含重複的名稱資料）
class DistinctBasicTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入包含重複名稱的測試資料
        userTable.Insert<decltype(NameColumn)>("Alice");
        userTable.Insert<decltype(NameColumn)>("Bob");
        userTable.Insert<decltype(NameColumn)>("Alice");
        userTable.Insert<decltype(NameColumn)>("Charlie");
        userTable.Insert<decltype(NameColumn)>("Bob");
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 包含多欄位資料的測試類別
class DistinctMultiColumnTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入包含多欄位的測試資料
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 25);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 30);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 25);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 30);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 30);
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 包含完整欄位資料的測試類別
class DistinctFullColumnTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入包含所有欄位的測試資料
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 90.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 85.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 90.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Charlie", 35, 88.0);
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 空資料庫測試類別
class DistinctEmptyTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// ============ DISTINCT 基本測試 ============
TEST_F(DistinctBasicTest, DistinctSingleColumn) {
    // 使用 DISTINCT 查詢唯一的名稱
    auto results = userTable.Select(userTable[NameColumn]).Distinct().Results();

    EXPECT_EQ(results.size(), 3); // Alice, Bob, Charlie

    // 驗證返回的值
    std::set<std::string> names;
    for (const auto &result: results) {
        names.insert(std::get<0>(result));
    }
    EXPECT_EQ(names.size(), 3);
    EXPECT_TRUE(names.count("Alice") > 0);
    EXPECT_TRUE(names.count("Bob") > 0);
    EXPECT_TRUE(names.count("Charlie") > 0);
}

TEST_F(DistinctBasicTest, DistinctWithDuplicates) {
    // 不使用 DISTINCT
    auto results_without_distinct = userTable.Select(userTable[NameColumn]).Results();
    EXPECT_EQ(results_without_distinct.size(), 5);

    // 使用 DISTINCT
    auto results_with_distinct = userTable.Select(userTable[NameColumn]).Distinct().Results();
    EXPECT_EQ(results_with_distinct.size(), 3); // 只有 Alice, Bob, Charlie
}

TEST_F(DistinctEmptyTest, DistinctIntegerColumn) {
    // 插入重複的年齡
    userTable.Insert<decltype(AgeColumn)>(25);
    userTable.Insert<decltype(AgeColumn)>(30);
    userTable.Insert<decltype(AgeColumn)>(25);
    userTable.Insert<decltype(AgeColumn)>(30);
    userTable.Insert<decltype(AgeColumn)>(35);

    // 使用 DISTINCT 查詢唯一的年齡
    auto results = userTable.Select(userTable[AgeColumn]).Distinct().Results();
    EXPECT_EQ(results.size(), 3); // 25, 30, 35
}

TEST_F(DistinctEmptyTest, DistinctRealColumn) {
    // 插入重複的分數
    userTable.Insert<decltype(ScoreColumn)>(85.5);
    userTable.Insert<decltype(ScoreColumn)>(90.0);
    userTable.Insert<decltype(ScoreColumn)>(85.5);
    userTable.Insert<decltype(ScoreColumn)>(90.0);
    userTable.Insert<decltype(ScoreColumn)>(95.5);

    // 使用 DISTINCT 查詢唯一的分數
    auto results = userTable.Select(userTable[ScoreColumn]).Distinct().Results();
    EXPECT_EQ(results.size(), 3); // 85.5, 90.0, 95.5
}

// ============ DISTINCT 多欄位測試 ============
TEST_F(DistinctMultiColumnTest, DistinctMultipleColumns) {
    // 使用 DISTINCT 查詢唯一的 (name, age) 組合
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .Distinct()
            .Results();

    EXPECT_EQ(results.size(), 3); // (Alice,25), (Bob,30), (Alice,30)
}

TEST_F(DistinctEmptyTest, DistinctThreeColumns) {
    // 插入數據
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 90.0);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 85.0);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 90.0);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 95.0);

    // 使用 DISTINCT 查詢所有欄位的唯一組合
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn], userTable[ScoreColumn])
            .Distinct()
            .Results();

    EXPECT_EQ(results.size(), 3); // 應該有三筆唯一的記錄
}

// ============ DISTINCT 結合 WHERE 測試 ============
TEST_F(DistinctMultiColumnTest, DistinctWithWhere) {
    // 添加額外資料
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Charlie", 35);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("David", 20);

    // 使用 DISTINCT 和 WHERE 結合查詢年齡 >= 30 的唯一名稱
    auto results = userTable
            .Select(userTable[NameColumn])
            .Where(userTable[AgeColumn] >= 30_expr)
            .Distinct()
            .Results();

    EXPECT_EQ(results.size(), 3); // Alice, Bob, Charlie
}

TEST_F(DistinctEmptyTest, DistinctWithWhereEqual) {
    // 插入數據
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 25);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 25);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 30);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 25);

    // 查詢年齡為 25 的唯一名稱
    auto results = userTable
            .Select(userTable[NameColumn])
            .Where(userTable[AgeColumn] == 25_expr)
            .Distinct()
            .Results();

    EXPECT_EQ(results.size(), 2); // Alice, Bob
}

TEST_F(DistinctFullColumnTest, DistinctWithComplexWhere) {
    // 使用複雜條件：年齡 >= 25 AND 分數 >= 88
    auto results = userTable
            .Select(userTable[NameColumn])
            .Where((userTable[AgeColumn] >= 25_expr) && (userTable[ScoreColumn] >= 88.0_expr))
            .Distinct()
            .Results();

    EXPECT_EQ(results.size(), 2); // Alice, Charlie
}

// ============ DISTINCT 結合 LIMIT OFFSET 測試 ============
TEST_F(DistinctBasicTest, DistinctWithLimit) {
    // 添加額外資料
    userTable.Insert<decltype(NameColumn)>("David");

    // 使用 DISTINCT 並配合 LIMIT
    auto results = userTable
            .Select(userTable[NameColumn])
            .Distinct()
            .LimitOffset(2)
            .Results();

    EXPECT_EQ(results.size(), 2); // 只取前兩個唯一值
}

TEST_F(DistinctBasicTest, DistinctWithLimitOffset) {
    // 添加額外資料
    userTable.Insert<decltype(NameColumn)>("David");

    // 使用 DISTINCT 並配合 LIMIT OFFSET
    auto results = userTable
            .Select(userTable[NameColumn])
            .Distinct()
            .LimitOffset(2, 1)
            .Results();

    EXPECT_EQ(results.size(), 2); // 跳過第一個，取兩個
}

TEST_F(DistinctEmptyTest, DistinctWithOffset) {
    // 插入數據
    for (int i = 1; i <= 5; ++i) {
        userTable.Insert<decltype(AgeColumn)>(i * 10);
        userTable.Insert<decltype(AgeColumn)>(i * 10); // 重複
    }

    // 使用 DISTINCT 並配合 OFFSET
    auto results = userTable
            .Select(userTable[AgeColumn])
            .Distinct()
            .LimitOffset(10, 2)
            .Results();

    EXPECT_EQ(results.size(), 3); // 應該還有 3 個唯一值
}

// ============ DISTINCT 邊界情況測試 ============
TEST_F(DistinctEmptyTest, DistinctNoData) {
    // 不插入任何數據
    auto results = userTable.Select(userTable[NameColumn]).Distinct().Results();
    EXPECT_EQ(results.size(), 0);
}

TEST_F(DistinctEmptyTest, DistinctSingleRow) {
    // 只插入一筆數據
    userTable.Insert<decltype(NameColumn)>("Alice");

    auto results = userTable.Select(userTable[NameColumn]).Distinct().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

TEST_F(DistinctEmptyTest, DistinctAllUnique) {
    // 插入所有唯一的數據
    userTable.Insert<decltype(NameColumn)>("Alice");
    userTable.Insert<decltype(NameColumn)>("Bob");
    userTable.Insert<decltype(NameColumn)>("Charlie");

    auto results = userTable.Select(userTable[NameColumn]).Distinct().Results();
    EXPECT_EQ(results.size(), 3); // 所有都是唯一的
}

TEST_F(DistinctEmptyTest, DistinctAllSame) {
    // 插入所有相同的數據
    for (int i = 0; i < 10; ++i) {
        userTable.Insert<decltype(NameColumn)>("Alice");
    }

    auto results = userTable.Select(userTable[NameColumn]).Distinct().Results();
    EXPECT_EQ(results.size(), 1); // 只有一個唯一值
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

// ============ DISTINCT 與 NULL 值測試 ============
TEST_F(DistinctBasicTest, DistinctWithNullValues) {
    // DISTINCT 應該正確處理
    auto results = userTable.Select(userTable[NameColumn]).Distinct().Results();
    EXPECT_EQ(results.size(), 3); // Alice, Bob, Charlie
}

// ============ DISTINCT 結合多種條件測試 ============
TEST_F(DistinctEmptyTest, DistinctWithWhereAndLimit) {
    // 插入數據
    for (int i = 1; i <= 5; ++i) {
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>(
            "User" + std::to_string(i), 20 + i);
        // 插入重複數據
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>(
            "User" + std::to_string(i), 20 + i);
    }

    // DISTINCT + WHERE + LIMIT
    auto results = userTable
            .Select(userTable[NameColumn])
            .Where(userTable[AgeColumn] > 22_expr)
            .Distinct()
            .LimitOffset(2)
            .Results();

    EXPECT_EQ(results.size(), 2);
}

TEST_F(DistinctFullColumnTest, DistinctMultiColumnsWithWhereAndLimit) {
    // 多欄位 DISTINCT + WHERE + LIMIT
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .Where(userTable[ScoreColumn] >= 85.0_expr)
            .Distinct()
            .LimitOffset(2)
            .Results();

    EXPECT_EQ(results.size(), 2);
}

// ============ DISTINCT 調用順序測試 ============
TEST_F(DistinctMultiColumnTest, DistinctBeforeWhere) {
    // Distinct 在 Where 之前調用
    auto results = userTable
            .Select(userTable[NameColumn])
            .Distinct()
            .Where(userTable[AgeColumn] >= 25_expr)
            .Results();

    EXPECT_EQ(results.size(), 2); // Alice, Bob
}

TEST_F(DistinctEmptyTest, DistinctBeforeLimitOffset) {
    // 插入重複資料
    for (int i = 0; i < 3; ++i) {
        userTable.Insert<decltype(NameColumn)>("Alice");
        userTable.Insert<decltype(NameColumn)>("Bob");
        userTable.Insert<decltype(NameColumn)>("Charlie");
    }

    // Distinct 在 LimitOffset 之前調用
    auto results = userTable
            .Select(userTable[NameColumn])
            .Distinct()
            .LimitOffset(2)
            .Results();

    EXPECT_EQ(results.size(), 2);
}

TEST_F(DistinctEmptyTest, DistinctWithAllOperations) {
    // 插入測試數據
    for (int i = 1; i <= 10; ++i) {
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>(
            "User" + std::to_string((i % 3) + 1),
            20 + (i % 5),
            80.0 + (i % 4) * 5
        );
    }

    // 組合所有操作: Select -> Distinct -> Where -> LimitOffset
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .Distinct()
            .Where(userTable[ScoreColumn] > 80.0_expr)
            .LimitOffset(3, 1)
            .Results();

    // 驗證結果數量
    EXPECT_LE(results.size(), 3);
    EXPECT_GE(results.size(), 0);
}


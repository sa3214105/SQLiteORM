#pragma once
#include "Common.hpp"

// ============ DISTINCT 基本測試 ============
TEST(DistinctTest, DistinctSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    // 插入重複的名稱
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));

    // 使用 DISTINCT 查詢唯一的名稱
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 3); // Alice, Bob, Charlie

    // 驗證返回的值
    std::set<std::string> names;
    for (const auto &result: results) {
        names.insert(std::get<UserTable::TableColumn<NameColumn>>(result).value);
    }
    EXPECT_EQ(names.size(), 3);
    EXPECT_TRUE(names.count("Alice") > 0);
    EXPECT_TRUE(names.count("Bob") > 0);
    EXPECT_TRUE(names.count("Charlie") > 0);
}

TEST(DistinctTest, DistinctWithDuplicates) {
    Database<UserTable> db("test_database.db", true);
    // 插入大量重複數據
    for (int i = 0; i < 5; ++i) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    }

    // 不使用 DISTINCT
    auto results_without_distinct = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Results();
    EXPECT_EQ(results_without_distinct.size(), 10);

    // 使用 DISTINCT
    auto results_with_distinct = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results_with_distinct.size(), 2); // 只有 Alice 和 Bob
}

TEST(DistinctTest, DistinctIntegerColumn) {
    Database<UserTable> db("test_database.db", true);
    // 插入重複的年齡
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(35));

    // 使用 DISTINCT 查詢唯一的年齡
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<AgeColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 3); // 25, 30, 35
}

TEST(DistinctTest, DistinctRealColumn) {
    Database<UserTable> db("test_database.db", true);
    // 插入重複的分數
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<ScoreColumn>(85.5));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<ScoreColumn>(90.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<ScoreColumn>(85.5));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<ScoreColumn>(90.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<ScoreColumn>(95.5));

    // 使用 DISTINCT 查詢唯一的分數
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<ScoreColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 3); // 85.5, 90.0, 95.5
}

// ============ DISTINCT 多欄位測試 ============
TEST(DistinctTest, DistinctMultipleColumns) {
    Database<UserTable> db("test_database.db", true);
    // 插入數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25)); // 完全重複
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(30)); // 名字相同但年齡不同
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30)); // 完全重複

    // 使用 DISTINCT 查詢唯一的 (name, age) 組合
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 3); // (Alice,25), (Bob,30), (Alice,30)
}

TEST(DistinctTest, DistinctThreeColumns) {
    Database<UserTable> db("test_database.db", true);
    // 插入數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25),
                                    UserTable::MakeTableColumn<ScoreColumn>(90.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30),
                                    UserTable::MakeTableColumn<ScoreColumn>(85.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25),
                                    UserTable::MakeTableColumn<ScoreColumn>(90.0)); // 完全重複
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25),
                                    UserTable::MakeTableColumn<ScoreColumn>(95.0)); // 分數不同

    // 使用 DISTINCT 查詢所有欄位的唯一組合
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>,
                    UserTable::TableColumn<AgeColumn>,
                    UserTable::TableColumn<ScoreColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 3); // 應該有三筆唯一的記錄
}

// ============ DISTINCT 結合 WHERE 測試 ============
TEST(DistinctTest, DistinctWithWhere) {
    Database<UserTable> db("test_database.db", true);
    // 插入數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(35));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));

    // 使用 DISTINCT 和 WHERE 結合查詢年齡 >= 30 的唯一名稱
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Where(GreaterThanEqual<UserTable::TableColumn<AgeColumn>>(30))
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 2); // Bob, Charlie
}

TEST(DistinctTest, DistinctWithWhereEqual) {
    Database<UserTable> db("test_database.db", true);
    // 插入數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));

    // 查詢年齡為 25 的唯一名稱
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Where(Equal<UserTable::TableColumn<AgeColumn>>(25))
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 2); // Alice, Bob
}

TEST(DistinctTest, DistinctWithComplexWhere) {
    Database<UserTable> db("test_database.db", true);
    // 插入數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25),
                                    UserTable::MakeTableColumn<ScoreColumn>(90.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30),
                                    UserTable::MakeTableColumn<ScoreColumn>(85.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25),
                                    UserTable::MakeTableColumn<ScoreColumn>(95.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(35),
                                    UserTable::MakeTableColumn<ScoreColumn>(88.0));

    // 使用複雜條件：年齡 >= 25 AND 分數 >= 88
    auto cond = GreaterThanEqual<UserTable::TableColumn<AgeColumn>>(25) &&
                GreaterThanEqual<UserTable::TableColumn<ScoreColumn>>(88.0);
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Where(cond)
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 2); // Alice, Charlie
}

// ============ DISTINCT 結合 LIMIT OFFSET 測試 ============
TEST(DistinctTest, DistinctWithLimit) {
    Database<UserTable> db("test_database.db", true);
    // 插入重複數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"));

    // 使用 DISTINCT 並配合 LIMIT
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .LimitOffset(2)
            .Results();
    EXPECT_EQ(results.size(), 2); // 只取前兩個唯一值
}

TEST(DistinctTest, DistinctWithLimitOffset) {
    Database<UserTable> db("test_database.db", true);
    // 插入重複數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"));

    // 使用 DISTINCT 並配合 LIMIT OFFSET
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .LimitOffset(2, 1)
            .Results();
    EXPECT_EQ(results.size(), 2); // 跳過第一個，取兩個
}

TEST(DistinctTest, DistinctWithOffset) {
    Database<UserTable> db("test_database.db", true);
    // 插入數據
    for (int i = 1; i <= 5; ++i) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(i * 10));
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(i * 10)); // 重複
    }

    // 使用 DISTINCT 並配合 OFFSET
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<AgeColumn>>()
            .Distinct()
            .LimitOffset(10, 2)
            .Results(); // 跳過前兩個
    EXPECT_EQ(results.size(), 3); // 應該還有 3 個唯一值
}

// ============ DISTINCT 邊界情況測試 ============
TEST(DistinctTest, DistinctNoData) {
    Database<UserTable> db("test_database.db", true);
    // 不插入任何數據
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 0);
}

TEST(DistinctTest, DistinctSingleRow) {
    Database<UserTable> db("test_database.db", true);
    // 只插入一筆數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));

    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

TEST(DistinctTest, DistinctAllUnique) {
    Database<UserTable> db("test_database.db", true);
    // 插入所有唯一的數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));

    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 3); // 所有都是唯一的
}

TEST(DistinctTest, DistinctAllSame) {
    Database<UserTable> db("test_database.db", true);
    // 插入所有相同的數據
    for (int i = 0; i < 10; ++i) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    }

    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 1); // 只有一個唯一值
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

// ============ DISTINCT 與 NULL 值測試 ============
TEST(DistinctTest, DistinctWithNullValues) {
    Database<UserTable> db("test_database.db", true);
    // 插入包含 NULL 的數據（某些欄位未設置）
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));

    // DISTINCT 應該正確處理
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .Results();
    EXPECT_EQ(results.size(), 2); // Alice, Bob
}

// ============ DISTINCT 結合多種條件測試 ============
TEST(DistinctTest, DistinctWithWhereAndLimit) {
    Database<UserTable> db("test_database.db", true);
    // 插入數據
    for (int i = 1; i <= 5; ++i) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
                                        UserTable::MakeTableColumn<AgeColumn>(20 + i));
        // 插入重複數據
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
                                        UserTable::MakeTableColumn<AgeColumn>(20 + i));
    }

    // DISTINCT + WHERE + LIMIT
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Where(GreaterThan<UserTable::TableColumn<AgeColumn>>(22))
            .Distinct()
            .LimitOffset(2)
            .Results();
    EXPECT_EQ(results.size(), 2);
}

TEST(DistinctTest, DistinctMultiColumnsWithWhereAndLimit) {
    Database<UserTable> db("test_database.db", true);
    // 插入數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25),
                                    UserTable::MakeTableColumn<ScoreColumn>(90.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30),
                                    UserTable::MakeTableColumn<ScoreColumn>(85.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25),
                                    UserTable::MakeTableColumn<ScoreColumn>(90.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(35),
                                    UserTable::MakeTableColumn<ScoreColumn>(95.0));

    // 多欄位 DISTINCT + WHERE + LIMIT
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
            .Where(GreaterThanEqual<UserTable::TableColumn<ScoreColumn>>(85.0))
            .Distinct()
            .LimitOffset(2)
            .Results();
    EXPECT_EQ(results.size(), 2);
}

// ============ DISTINCT 調用順序測試 ============
TEST(DistinctTest, DistinctBeforeWhere) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));

    // Distinct 在 Where 之前調用
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .Where(GreaterThanEqual<UserTable::TableColumn<AgeColumn>>(25))
            .Results();
    EXPECT_EQ(results.size(), 2); // Alice, Bob
}

TEST(DistinctTest, DistinctBeforeLimitOffset) {
    Database<UserTable> db("test_database.db", true);
    for (int i = 0; i < 3; ++i) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));
    }

    // Distinct 在 LimitOffset 之前調用
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>>()
            .Distinct()
            .LimitOffset(2)
            .Results();
    EXPECT_EQ(results.size(), 2);
}

TEST(DistinctTest, DistinctWithAllOperations) {
    Database<UserTable> db("test_database.db", true);
    // 插入測試數據
    for (int i = 1; i <= 10; ++i) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string((i % 3) + 1)),
            UserTable::MakeTableColumn<AgeColumn>(20 + (i % 5)),
            UserTable::MakeTableColumn<ScoreColumn>(80.0 + (i % 4) * 5)
        );
    }

    // 組合所有操作: Select -> Distinct -> Where -> LimitOffset
    auto results = db.GetTable<UserTable>()
            .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
            .Distinct()
            .Where(GreaterThan<UserTable::TableColumn<ScoreColumn>>(80.0))
            .LimitOffset(3, 1)
            .Results();

    // 驗證結果數量
    EXPECT_LE(results.size(), 3);
    EXPECT_GE(results.size(), 0);
}


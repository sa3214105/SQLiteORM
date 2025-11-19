#pragma once
#include "Common.hpp"

// ============ 批量插入測試 ============

class BatchInsertTest : public ::testing::Test {
protected:
    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 測試基本批量插入
TEST_F(BatchInsertTest, InsertManyBasic) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_insert_many">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn)
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 準備批量資料
    std::vector<std::tuple<int, std::string, int>> rows;
    for (int i = 1; i <= 100; ++i) {
        rows.push_back(std::make_tuple(i, "User" + std::to_string(i), 20 + (i % 50)));
    }

    // 批量插入
    testTable.InsertMany<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(rows);

    // 驗證結果
    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[AgeColumn]
    ).Results();

    EXPECT_EQ(results.size(), 100);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "User1");
    EXPECT_EQ(std::get<0>(results[99]), 100);
    EXPECT_EQ(std::get<1>(results[99]), "User100");
}

// 測試批量插入空資料
TEST_F(BatchInsertTest, InsertManyEmpty) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_insert_many_empty">(
        std::make_tuple(IdColumn{}, NameColumn)
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    std::vector<std::tuple<int, std::string>> rows;

    // 批量插入空資料，不應該拋出異常
    EXPECT_NO_THROW((testTable.InsertMany<IdColumn, decltype(NameColumn)>(rows)));

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn]
    ).Results();

    EXPECT_EQ(results.size(), 0);
}

// 測試批量插入部分欄位
TEST_F(BatchInsertTest, InsertManyPartialColumns) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_insert_many_partial">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn)
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 只插入部分欄位
    std::vector<std::tuple<int, std::string>> rows;
    for (int i = 1; i <= 10; ++i) {
        rows.push_back(std::make_tuple(i, "User" + std::to_string(i)));
    }

    testTable.InsertMany<IdColumn, decltype(NameColumn)>(rows);

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn]
    ).Results();

    EXPECT_EQ(results.size(), 10);
    EXPECT_EQ(std::get<1>(results[5]), "User6");
}

// 測試批量插入違反約束
TEST_F(BatchInsertTest, InsertManyWithConstraintViolation) {
    using IdColumn = Column<"id", ExprResultType::INTEGER, ColumnPrimaryKey<>>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_insert_many_constraint">(
        std::make_tuple(IdColumn{}, NameColumn, EmailColumn{})
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    std::vector<std::tuple<int, std::string, std::string>> rows;

    // 添加重複的 ID
    rows.push_back(std::make_tuple(1, "Alice", "alice@example.com"));
    rows.push_back(std::make_tuple(1, "Bob", "bob@example.com")); // 重複的 ID

    // 應該拋出異常並回滾
    EXPECT_ANY_THROW((testTable.InsertMany<
        IdColumn,
        decltype(NameColumn),
        EmailColumn
    >(rows)));

    // 驗證沒有資料被插入（因為 transaction 被回滾）
    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[EmailColumn{}]
    ).Results();

    EXPECT_EQ(results.size(), 0);
}

// 測試批量插入與單筆插入混合使用
TEST_F(BatchInsertTest, InsertManyMixedWithSingleInsert) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_insert_many_mixed">(
        std::make_tuple(IdColumn{}, NameColumn)
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 先單筆插入
    testTable.Insert<IdColumn, decltype(NameColumn)>(1, "Single");

    // 然後批量插入
    std::vector<std::tuple<int, std::string>> rows;
    for (int i = 2; i <= 11; ++i) {
        rows.push_back(std::make_tuple(i, "Batch" + std::to_string(i)));
    }

    testTable.InsertMany<IdColumn, decltype(NameColumn)>(rows);

    // 再單筆插入
    testTable.Insert<IdColumn, decltype(NameColumn)>(12, "AnotherSingle");

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn]
    ).Results();

    EXPECT_EQ(results.size(), 12);
    EXPECT_EQ(std::get<1>(results[0]), "Single");
    EXPECT_EQ(std::get<1>(results[11]), "AnotherSingle");
}

// 測試批量插入效能（大量資料）
TEST_F(BatchInsertTest, InsertManyPerformance) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_insert_many_performance">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn, ScoreColumn)
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 準備 1000 筆資料
    std::vector<std::tuple<int, std::string, int, double>> rows;
    for (int i = 1; i <= 1000; ++i) {
        rows.push_back(std::make_tuple(
            i,
            "User" + std::to_string(i),
            20 + (i % 60),
            50.0 + (i % 50)
        ));
    }

    // 批量插入
    auto start = std::chrono::high_resolution_clock::now();
    testTable.InsertMany<
        IdColumn,
        decltype(NameColumn),
        decltype(AgeColumn),
        decltype(ScoreColumn)
    >(rows);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "InsertMany 1000 rows took: " << duration.count() << "ms" << std::endl;

    // 驗證結果
    auto results = testTable.Select(testTable[IdColumn{}]).Results();

    EXPECT_EQ(results.size(), 1000);
}


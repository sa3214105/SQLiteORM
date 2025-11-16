#pragma once
#include "Common.hpp"

// ============ 批量插入測試 ============

// 測試基本批量插入
TEST(BatchInsertTest, InsertManyBasic) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_insert_many", IdColumn, NameColumn, AgeColumn>;
    Database<TestTable> db("test_database.db", true);

    // 準備批量資料
    std::vector<std::tuple<int, std::string, int>> rows;

    for (int i = 1; i <= 100; ++i) {
        rows.push_back(std::make_tuple(i, "User" + std::to_string(i), 20 + (i % 50)));
    }

    // 批量插入
    db.GetTable<TestTable>().InsertMany<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >(rows);

    // 驗證結果
    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<AgeColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 100);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "User1");
    EXPECT_EQ(std::get<0>(results[99]), 100);
    EXPECT_EQ(std::get<1>(results[99]), "User100");
}

// 測試批量插入空資料
TEST(BatchInsertTest, InsertManyEmpty) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_insert_many_empty", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    std::vector<std::tuple<int, std::string>> rows;

    // 批量插入空資料，不應該拋出異常
    EXPECT_NO_THROW((db.GetTable<TestTable>().InsertMany<TestTable::TableColumn<IdColumn>,TestTable::TableColumn<NameColumn>>(rows)));

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 0);
}

// 測試批量插入部分欄位
TEST(BatchInsertTest, InsertManyPartialColumns) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_insert_many_partial", IdColumn, NameColumn, AgeColumn>;
    Database<TestTable> db("test_database.db", true);

    // 只插入部分欄位
    std::vector<std::tuple<int, std::string>> rows;

    for (int i = 1; i <= 10; ++i) {
        rows.push_back(std::make_tuple(i, "User" + std::to_string(i)));
    }

    db.GetTable<TestTable>().InsertMany<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >(rows);

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 10);
    EXPECT_EQ(std::get<1>(results[5]), "User6");
}

// 測試批量插入違反約束
TEST(BatchInsertTest, InsertManyWithConstraintViolation) {
    using IdColumn = Column<"id", ExprResultType::INTEGER, ColumnPrimaryKey<>>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using TestTable = Table<"test_insert_many_constraint", IdColumn, NameColumn, EmailColumn>;
    Database<TestTable> db("test_database.db", true);

    std::vector<std::tuple<int, std::string, std::string>> rows;

    // 添加重複的 ID
    rows.push_back(std::make_tuple(1, "Alice", "alice@example.com"));
    rows.push_back(std::make_tuple(1, "Bob", "bob@example.com")); // 重複的 ID

    // 應該拋出異常並回滾
    EXPECT_ANY_THROW((db.GetTable<TestTable>().InsertMany<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >(rows)));

    // 驗證沒有資料被插入（因為 transaction 被回滾）
    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<EmailColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 0);
}

// 測試批量插入與單筆插入混合使用
TEST(BatchInsertTest, InsertManyMixedWithSingleInsert) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_insert_many_mixed", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    // 先單筆插入
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >(1, "Single");

    // 然後批量插入
    std::vector<std::tuple<int, std::string>> rows;

    for (int i = 2; i <= 11; ++i) {
        rows.push_back(std::make_tuple(i, "Batch" + std::to_string(i)));
    }

    db.GetTable<TestTable>().InsertMany<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >(rows);

    // 再單筆插入
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >(12, "AnotherSingle");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 12);
    EXPECT_EQ(std::get<1>(results[0]), "Single");
    EXPECT_EQ(std::get<1>(results[11]), "AnotherSingle");
}

// 測試批量插入效能（大量資料）
TEST(BatchInsertTest, InsertManyPerformance) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_insert_many_performance", IdColumn, NameColumn, AgeColumn, ScoreColumn>;
    Database<TestTable> db("test_database.db", true);

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
    db.GetTable<TestTable>().InsertMany<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>,
        TestTable::TableColumn<ScoreColumn>
    >(rows);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "InsertMany 1000 rows took: " << duration.count() << "ms" << std::endl;

    // 驗證結果
    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<IdColumn>()).Results();

    EXPECT_EQ(results.size(), 1000);
}

#pragma once
#include "Common.hpp"

// ============ Upsert 測試 ============

class UpsertTest : public ::testing::Test {
protected:
    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 測試基本 Upsert (INSERT 行為)
TEST_F(UpsertTest, UpsertInsertNewRow) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_upsert_insert">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // Upsert 新資料（應執行 INSERT）
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 25);

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[AgeColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
    EXPECT_EQ(std::get<2>(results[0]), 25);
}

// 測試 Upsert (UPDATE 行為)
TEST_F(UpsertTest, UpsertUpdateExistingRow) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_upsert_update">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 先插入原始資料
    testTable.Insert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 25);

    // Upsert 衝突資料（應執行 UPDATE）
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Bob", 30);

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[AgeColumn]
    ).Results().ToVector();

    // 應該只有一筆資料，且被更新
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Bob");
    EXPECT_EQ(std::get<2>(results[0]), 30);
}

// 測試 Upsert 單一欄位
TEST_F(UpsertTest, UpsertSingleColumn) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_upsert_single">(
        std::make_tuple(IdColumn{}, NameColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 第一次 Upsert
    testTable.Upsert<IdColumn>(1);

    // 第二次 Upsert 相同 ID（應該更新）
    testTable.Upsert<IdColumn>(1);

    auto results = testTable.Select(testTable[IdColumn{}]).Results().ToVector();

    // 應該只有一筆資料
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
}

// 測試 Upsert 多次操作
TEST_F(UpsertTest, UpsertMultipleTimes) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_upsert_multiple">(
        std::make_tuple(IdColumn{}, NameColumn, ScoreColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 第一次 Upsert
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(ScoreColumn)>(1, "Alice", 85.5);

    // 第二次 Upsert（更新分數）
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(ScoreColumn)>(1, "Alice", 90.0);

    // 第三次 Upsert（更新名字和分數）
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(ScoreColumn)>(1, "Alicia", 95.5);

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[ScoreColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alicia");
    EXPECT_EQ(std::get<2>(results[0]), 95.5);
}

// 測試 Upsert 複合主鍵
TEST_F(UpsertTest, UpsertWithCompositePrimaryKey) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_upsert_composite">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}, NameColumn))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 插入第一筆資料
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 25);

    // 插入不同組合的資料（不衝突）
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Bob", 30);

    // 更新已存在的複合鍵資料
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 26);

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[AgeColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);

    // 驗證 Alice 的年齡被更新
    auto alice = std::find_if(results.begin(), results.end(), [](const auto &row) {
        return std::get<1>(row) == "Alice";
    });
    EXPECT_NE(alice, results.end());
    EXPECT_EQ(std::get<2>(*alice), 26);

    // 驗證 Bob 的資料
    auto bob = std::find_if(results.begin(), results.end(), [](const auto &row) {
        return std::get<1>(row) == "Bob";
    });
    EXPECT_NE(bob, results.end());
    EXPECT_EQ(std::get<2>(*bob), 30);
}

// 測試 Upsert 與 UNIQUE 約束
TEST_F(UpsertTest, UpsertWithUniqueConstraint) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_upsert_unique">(
        std::make_tuple(IdColumn{}, NameColumn, EmailColumn{}),
        std::make_tuple(
            TablePrimaryKey(std::make_tuple(IdColumn{})),
            TableUnique(std::make_tuple(EmailColumn{}))
        ),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 第一次插入
    testTable.Upsert<IdColumn, decltype(NameColumn), EmailColumn>(1, "Alice", "alice@example.com");

    // Upsert 相同的 ID（應該更新）
    testTable.Upsert<IdColumn, decltype(NameColumn), EmailColumn>(1, "Alicia", "alicia@example.com");

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[EmailColumn{}]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alicia");
    EXPECT_EQ(std::get<2>(results[0]), "alicia@example.com");
}

// 測試 Upsert 混合 INSERT 和 UPDATE
TEST_F(UpsertTest, UpsertMixedInsertAndUpdate) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_upsert_mixed">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // Upsert ID=1 (INSERT)
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 25);

    // Upsert ID=2 (INSERT)
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(2, "Bob", 30);

    // Upsert ID=1 (UPDATE)
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice Updated", 26);

    // Upsert ID=3 (INSERT)
    testTable.Upsert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(3, "Charlie", 35);

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[AgeColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 3);

    // 驗證 ID=1 被更新
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice Updated");
    EXPECT_EQ(std::get<2>(results[0]), 26);

    // 驗證 ID=2
    EXPECT_EQ(std::get<0>(results[1]), 2);
    EXPECT_EQ(std::get<1>(results[1]), "Bob");
    EXPECT_EQ(std::get<2>(results[1]), 30);

    // 驗證 ID=3
    EXPECT_EQ(std::get<0>(results[2]), 3);
    EXPECT_EQ(std::get<1>(results[2]), "Charlie");
    EXPECT_EQ(std::get<2>(results[2]), 35);
}


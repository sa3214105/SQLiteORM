#pragma once
#include "Common.hpp"

// ============ Table Options 測試 ============

class TableOptionTest : public ::testing::Test {
protected:
    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 測試 WITHOUT ROWID 選項
TEST_F(TableOptionTest, TableOptionWithoutRowId) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_without_rowid">(
        std::make_tuple(IdColumn{}, NameColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}))),
        std::make_tuple(WithoutRowId{})
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // WITHOUT ROWID 表必須有 PRIMARY KEY
    testTable.Insert<IdColumn, decltype(NameColumn)>(1, "Alice");
    testTable.Insert<IdColumn, decltype(NameColumn)>(2, "Bob");

    auto results = testTable.Select(testTable[IdColumn{}], testTable[NameColumn]).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

// 測試 STRICT 選項
TEST_F(TableOptionTest, TableOptionStrict) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_strict">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn),
        std::make_tuple(Strict{}),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // STRICT 模式下，型別檢查更嚴格
    testTable.Insert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 30);

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[AgeColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<2>(results[0]), 30);
}

// 測試 WITHOUT ROWID 與 STRICT 組合
TEST_F(TableOptionTest, TableOptionWithoutRowIdAndStrict) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_without_rowid_strict">(
        std::make_tuple(IdColumn{}, NameColumn, EmailColumn{}),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}))),
        std::make_tuple(WithoutRowId{}, Strict{})
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn>(1, "Alice", "alice@example.com");
    testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn>(2, "Bob", "bob@example.com");

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[EmailColumn{}]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試帶有複合主鍵的 WITHOUT ROWID
TEST_F(TableOptionTest, TableOptionWithoutRowIdCompositeKey) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using CategoryColumn = Column<"category", ExprResultType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_without_rowid_composite">(
        std::make_tuple(IdColumn{}, CategoryColumn{}, NameColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(CategoryColumn{}, IdColumn{}))),
        std::make_tuple(WithoutRowId{})
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, CategoryColumn, decltype(NameColumn)>(1, "Electronics", "Laptop");
    testTable.Insert<IdColumn, CategoryColumn, decltype(NameColumn)>(1, "Books", "Novel");

    // 相同的 category 和 id 組合應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, CategoryColumn, decltype(NameColumn)>(1, "Electronics", "Phone")
    ));

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[CategoryColumn{}],
        testTable[NameColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試 STRICT 與多個約束組合
TEST_F(TableOptionTest, TableOptionStrictWithConstraints) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using PhoneColumn = Column<"phone", ExprResultType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_strict_constraints">(
        std::make_tuple(IdColumn{}, NameColumn, EmailColumn{}, PhoneColumn{}),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{})),
                        TableUnique(std::make_tuple(EmailColumn{})),
                        TableUnique(std::make_tuple(PhoneColumn{}))
        ),
        std::make_tuple(
            Strict{}
        )
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn>(
        1, "Alice", "alice@example.com", "123-4567"
    );

    // 測試 PRIMARY KEY
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn>(
            1, "Bob", "bob@example.com", "234-5678"
        )
    ));

    // 測試 UNIQUE email
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn>(
            2, "Bob", "alice@example.com", "234-5678"
        )
    ));

    // 測試 UNIQUE phone
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn>(
            2, "Bob", "bob@example.com", "123-4567"
        )
    ));

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 1);
}

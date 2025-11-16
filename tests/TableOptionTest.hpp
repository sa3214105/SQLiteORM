#pragma once
#include "Common.hpp"

// ============ Table Options 測試 ============

// 測試 WITHOUT ROWID 選項
TEST(TableOptionTest, TableOptionWithoutRowId) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_without_rowid",
        IdColumn, NameColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        WithoutRowId>;
    Database<TestTable> db("test_database.db", true);

    // WITHOUT ROWID 表必須有 PRIMARY KEY
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >(1, "Alice");

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >(2, "Bob");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

// 測試 STRICT 選項
TEST(TableOptionTest, TableOptionStrict) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_strict",
        IdColumn, NameColumn, AgeColumn,
        Strict>;
    Database<TestTable> db("test_database.db", true);

    // STRICT 模式下，型別檢查更嚴格
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >(1, "Alice", 30);

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<AgeColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<2>(results[0]), 30);
}

// 測試 WITHOUT ROWID 與 STRICT 組合
TEST(TableOptionTest, TableOptionWithoutRowIdAndStrict) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using TestTable = Table<"test_without_rowid_strict",
        IdColumn, NameColumn, EmailColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        WithoutRowId,
        Strict>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >(1, "Alice", "alice@example.com");

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >(2, "Bob", "bob@example.com");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<EmailColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試帶有複合主鍵的 WITHOUT ROWID
TEST(TableOptionTest, TableOptionWithoutRowIdCompositeKey) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using CategoryColumn = Column<"category", ExprResultType::TEXT>;
    using TestTable = Table<"test_without_rowid_composite",
        IdColumn, CategoryColumn, NameColumn,
        TablePrimaryKey<TypeGroup<CategoryColumn, IdColumn> >,
        WithoutRowId>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<CategoryColumn>,
        TestTable::TableColumn<NameColumn>
    >(1, "Electronics", "Laptop");

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<CategoryColumn>,
        TestTable::TableColumn<NameColumn>
    >(1, "Books", "Novel");

    // 相同的 category 和 id 組合應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<CategoryColumn>,
            TestTable::TableColumn<NameColumn>
        >(1, "Electronics", "Phone"))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<CategoryColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試 STRICT 與多個約束組合
TEST(TableOptionTest, TableOptionStrictWithConstraints) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using PhoneColumn = Column<"phone", ExprResultType::TEXT>;
    using TestTable = Table<"test_strict_constraints",
        IdColumn, NameColumn, EmailColumn, PhoneColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        TableUnique<TypeGroup<EmailColumn> >,
        TableUnique<TypeGroup<PhoneColumn> >,
        Strict>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>
    >(1, "Alice", "alice@example.com", "123-4567");

    // 測試 PRIMARY KEY
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>
        >(1, "Bob", "bob@example.com", "234-5678"))
    );

    // 測試 UNIQUE email
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>
        >(2, "Bob", "alice@example.com", "234-5678"))
    );

    // 測試 UNIQUE phone
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>
        >(2, "Bob", "bob@example.com", "123-4567"))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 1);
}
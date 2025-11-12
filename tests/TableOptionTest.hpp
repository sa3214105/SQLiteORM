#pragma once
#include "Common.hpp"

// ============ Table Options 測試 ============

// 測試 WITHOUT ROWID 選項
TEST(TableOptionTest, TableOptionWithoutRowId) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_without_rowid",
        IdColumn, NameColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        WithoutRowId>;
    Database<TestTable> db("test_database.db", true);

    // WITHOUT ROWID 表必須有 PRIMARY KEY
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice")
    );

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(2),
        TestTable::MakeTableColumn<NameColumn>("Bob")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<TestTable::TableColumn<IdColumn>>(results[0]).value, 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

// 測試 STRICT 選項
TEST(TableOptionTest, TableOptionStrict) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_strict",
        IdColumn, NameColumn, AgeColumn,
        Strict>;
    Database<TestTable> db("test_database.db", true);

    // STRICT 模式下，型別檢查更嚴格
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<AgeColumn>(30)
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<IdColumn>>(results[0]).value, 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<AgeColumn>>(results[0]).value, 30);
}

// 測試 WITHOUT ROWID 與 STRICT 組合
TEST(TableOptionTest, TableOptionWithoutRowIdAndStrict) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_without_rowid_strict",
        IdColumn, NameColumn, EmailColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        WithoutRowId,
        Strict>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(2),
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<EmailColumn>("bob@example.com")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試帶有複合主鍵的 WITHOUT ROWID
TEST(TableOptionTest, TableOptionWithoutRowIdCompositeKey) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using CategoryColumn = Column<"category", column_type::TEXT>;
    using TestTable = Table<"test_without_rowid_composite",
        IdColumn, CategoryColumn, NameColumn,
        TablePrimaryKey<TypeGroup<CategoryColumn, IdColumn> >,
        WithoutRowId>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<CategoryColumn>("Electronics"),
        TestTable::MakeTableColumn<NameColumn>("Laptop")
    );

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<CategoryColumn>("Books"),
        TestTable::MakeTableColumn<NameColumn>("Novel")
    );

    // 相同的 category 和 id 組合應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1),
            TestTable::MakeTableColumn<CategoryColumn>("Electronics"),
            TestTable::MakeTableColumn<NameColumn>("Phone")
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<CategoryColumn>,
        TestTable::TableColumn<NameColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試 STRICT 與多個約束組合
TEST(TableOptionTest, TableOptionStrictWithConstraints) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using EmailColumn = Column<"email", column_type::TEXT>;
    using PhoneColumn = Column<"phone", column_type::TEXT>;
    using TestTable = Table<"test_strict_constraints",
        IdColumn, NameColumn, EmailColumn, PhoneColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        TableUnique<TypeGroup<EmailColumn> >,
        TableUnique<TypeGroup<PhoneColumn> >,
        Strict>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com"),
        TestTable::MakeTableColumn<PhoneColumn>("123-4567")
    );

    // 測試 PRIMARY KEY
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("234-5678")
        )
    );

    // 測試 UNIQUE email
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("234-5678")
        )
    );

    // 測試 UNIQUE phone
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("123-4567")
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
}
#pragma once
#include "Common.hpp"

// ============ Column Constraints 測試 ============

// 測試 PRIMARY KEY 約束
TEST(ColumnConstraintsTest, ConstraintPrimaryKey) {
    using IdColumn = Column<"id", ExprResultType::INTEGER, ColumnPrimaryKey<> >;
    using TestTable = Table<"test_pk", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn> >(1, "Alice");

    // 嘗試插入重複的 PRIMARY KEY，應該失敗
    EXPECT_THROW(
        (db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn> >(1, "Bob"
        )),
        std::runtime_error
    );

    // 驗證只有一筆資料
    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<IdColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
}

// 測試 PRIMARY KEY 與 DESC 排序
TEST(ColumnConstraintsTest, ConstraintPrimaryKeyDesc) {
    using IdColumn = Column<"id", ExprResultType::INTEGER, ColumnPrimaryKey<OrderType::DESC> >;
    using TestTable = Table<"test_pk_desc", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn> >(1, "First");
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn> >(2, "Second");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<IdColumn>()).Results();
    EXPECT_EQ(results.size(), 2);
}

// 測試 NOT NULL 約束
TEST(ColumnConstraintsTest, ConstraintNotNull) {
    using EmailColumn = Column<"email", ExprResultType::TEXT, ColumnNotNull<> >;
    using TestTable = Table<"test_not_null", NameColumn, EmailColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入有效資料
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn>, TestTable::TableColumn<EmailColumn> >(
        "Alice", "alice@example.com");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<EmailColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "alice@example.com");
}

// 測試 UNIQUE 約束
TEST(ColumnConstraintsTest, ConstraintUnique) {
    using UsernameColumn = Column<"username", ExprResultType::TEXT, ColumnUnique<> >;
    using TestTable = Table<"test_unique", UsernameColumn, AgeColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<UsernameColumn>, TestTable::TableColumn<AgeColumn> >(
        "alice123", 25);

    // 嘗試插入重複的 UNIQUE 值，應該失敗
    EXPECT_THROW(
        (db.GetTable<TestTable>().Insert<TestTable::TableColumn<UsernameColumn>,TestTable::TableColumn<AgeColumn> >(
            "alice123", 30)),
        std::runtime_error
    );

    // 插入不同的值應該成功
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<UsernameColumn>, TestTable::TableColumn<
        AgeColumn> >("bob456", 30);

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<UsernameColumn>()).Results();
    EXPECT_EQ(results.size(), 2);
}

// 測試 DEFAULT 約束（整數）
TEST(ColumnConstraintsTest, ConstraintDefaultInteger) {
    using StatusColumn = Column<"status", ExprResultType::INTEGER, Default<0> >;
    using TestTable = Table<"test_default_int", NameColumn, StatusColumn>;
    Database<TestTable> db("test_database.db", true);

    // 只插入 name，status 應該使用預設值
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn> >("Alice");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<NameColumn>(),
                                                   TestTable::TableColumn<StatusColumn>()).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), 0);
}

// 測試 DEFAULT 約束（字串）
TEST(ColumnConstraintsTest, ConstraintDefaultString) {
    using CountryColumn = Column<"country", ExprResultType::TEXT, Default<"Taiwan"> >;
    using TestTable = Table<"test_default_str", NameColumn, CountryColumn>;
    Database<TestTable> db("test_database.db", true);

    // 只插入 name
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn> >("Alice");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<NameColumn>(),
                                                   TestTable::TableColumn<CountryColumn>()).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), "Taiwan");
}

// 測試組合約束：PRIMARY KEY + NOT NULL
TEST(ColumnConstraintsTest, ConstraintCombinationPrimaryKeyNotNull) {
    using IdColumn = Column<"id", ExprResultType::INTEGER, ColumnPrimaryKey<>, ColumnNotNull<> >;
    using TestTable = Table<"test_pk_nn", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn> >(1, "Alice");

    // 嘗試插入重複 ID 應該失敗
    EXPECT_THROW(
        (db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>,TestTable::TableColumn<NameColumn> >(1, "Bob")),
        std::runtime_error
    );

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<IdColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
}

// 測試組合約束：UNIQUE + NOT NULL
TEST(ColumnConstraintsTest, ConstraintCombinationUniqueNotNull) {
    using EmailColumn = Column<"email", ExprResultType::TEXT, ColumnUnique<>, ColumnNotNull<> >;
    using TestTable = Table<"test_uniq_nn", NameColumn, EmailColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn>, TestTable::TableColumn<EmailColumn> >(
        "Alice", "alice@test.com");

    // 嘗試插入重複的 email
    EXPECT_THROW(
        (db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn>,TestTable::TableColumn<EmailColumn> >("Bob", "alice@test.com")),
        std::runtime_error
    );

    // 插入不同的 email 應該成功
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn>, TestTable::TableColumn<EmailColumn> >(
        "Bob", "bob@test.com");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<EmailColumn>()).Results();
    EXPECT_EQ(results.size(), 2);
}

// 測試 PRIMARY KEY 與 ON CONFLICT REPLACE
TEST(ColumnConstraintsTest, ConstraintPrimaryKeyConflictReplace) {
    using IdColumn = Column<"id", ExprResultType::INTEGER, ColumnPrimaryKey<OrderType::ASC, ConflictCause::REPLACE> >;
    using TestTable = Table<"test_pk_replace", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn> >(1, "Alice");

    // 插入重複 ID，應該替換舊資料
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn> >(1, "Bob");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<IdColumn>(),
                                                   TestTable::TableColumn<NameColumn>()).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Bob");
}

// 測試 UNIQUE 與 ON CONFLICT IGNORE
TEST(ColumnConstraintsTest, ConstraintUniqueConflictIgnore) {
    using EmailColumn = Column<"email", ExprResultType::TEXT, ColumnUnique<ConflictCause::IGNORE> >;
    using TestTable = Table<"test_uniq_ignore", NameColumn, EmailColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn>, TestTable::TableColumn<EmailColumn> >(
        "Alice", "same@test.com");

    // 插入重複值，應該被忽略（不拋出異常）
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn>, TestTable::TableColumn<EmailColumn> >(
        "Bob", "same@test.com");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<NameColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

// 測試 NOT NULL 與 ON CONFLICT FAIL
TEST(ColumnConstraintsTest, ConstraintNotNullConflictFail) {
    using RequiredColumn = Column<"required_field", ExprResultType::TEXT, ColumnNotNull<ConflictCause::FAIL> >;
    using TestTable = Table<"test_nn_fail", NameColumn, RequiredColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入有效資料
    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn>, TestTable::TableColumn<RequiredColumn> >(
        "Alice", "value");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<RequiredColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
}

// 測試複雜的組合約束
TEST(ColumnConstraintsTest, ConstraintComplexCombination) {
    using IdColumn = Column<"id", ExprResultType::INTEGER, ColumnPrimaryKey<> >;
    using EmailColumn = Column<"email", ExprResultType::TEXT, ColumnUnique<>, ColumnNotNull<> >;
    using StatusColumn = Column<"status", ExprResultType::INTEGER, Default<1> >;
    using TestTable = Table<"test_complex", IdColumn, NameColumn, EmailColumn, StatusColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入完整資料
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<StatusColumn>
    >(1, "Alice", "alice@test.com", 5);

    // 插入部分資料，status 使用預設值
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >(2, "Bob", "bob@test.com");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<EmailColumn>(),
        TestTable::TableColumn<StatusColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);

    // 驗證第一筆
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<3>(results[0]), 5);

    // 驗證第二筆的預設值
    EXPECT_EQ(std::get<0>(results[1]), 2);
    EXPECT_EQ(std::get<3>(results[1]), 1);
}

// 測試實數型的 DEFAULT 約束
TEST(ColumnConstraintsTest, ConstraintDefaultReal) {
    using RatingColumn = Column<"rating", ExprResultType::REAL, Default<5.0> >;
    using TestTable = Table<"test_default_real", NameColumn, RatingColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<TestTable::TableColumn<NameColumn> >("Product1");

    auto results = db.GetTable<TestTable>().Select(TestTable::TableColumn<NameColumn>(),
                                                   TestTable::TableColumn<RatingColumn>()).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_DOUBLE_EQ(std::get<1>(results[0]), 5.0);
}

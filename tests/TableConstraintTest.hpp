#pragma once
#include "Common.hpp"

// ============ Table Constraint 測試 ============

// 測試 PrimaryKey (單欄位主鍵)
TEST(TableConstraintTest, TableConstraintPrimaryKeySingle) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_pk_single", IdColumn, NameColumn,
        TablePrimaryKey<TypeGroup<IdColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn>>(1, "Alice");

    // 嘗試插入重複的主鍵，應該失敗
    EXPECT_ANY_THROW(
        (db.GetTable<TestTable>().Insert<TestTable::TableColumn<IdColumn>, TestTable::TableColumn<NameColumn>>(1, "Bob"))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

// 測試 PrimaryKey (複合主鍵)
TEST(TableConstraintTest, TableConstraintPrimaryKeyComposite) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_pk_composite", IdColumn, NameColumn, AgeColumn,
        TablePrimaryKey<TypeGroup<IdColumn, NameColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >(1, "Alice", 30);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >(1, "Bob", 25);

    // 嘗試插入重複的複合主鍵，應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<AgeColumn>
        >(1, "Alice", 35))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<AgeColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試 Unique (單欄位唯一約束)
TEST(TableConstraintTest, TableConstraintUniqueSingle) {
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using TestTable = Table<"test_unique_single", NameColumn, EmailColumn,
        TableUnique<TypeGroup<EmailColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >("Alice", "alice@example.com");

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>
        >("Bob", "alice@example.com"))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<EmailColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "alice@example.com");
}

// 測試 Unique (複合唯一約束)
TEST(TableConstraintTest, TableConstraintUniqueComposite) {
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using TestTable = Table<"test_unique_composite", NameColumn, EmailColumn,
        TableUnique<TypeGroup<NameColumn, EmailColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >("Alice", "alice@example.com");

    // 相同的 name 但不同的 email，應該成功
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >("Alice", "alice2@example.com");

    // 嘗試插入重複的複合唯一鍵，應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>
        >("Alice", "alice@example.com"))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<EmailColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合使用欄位約束和表約束
TEST(TableConstraintTest, TableConstraintMixed) {
    using IdColumn = Column<"id", ExprResultType::INTEGER, ColumnPrimaryKey<> >;
    using EmailColumn = Column<"email", ExprResultType::TEXT, ColumnNotNull<> >;
    using TestTable = Table<"test_mixed_constraints", IdColumn, NameColumn, EmailColumn,
        TableUnique<TypeGroup<EmailColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >(1, "Alice", "alice@example.com");

    // 測試主鍵唯一性
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>
        >(1, "Bob", "bob@example.com"))
    );

    // 測試 NOT NULL 約束
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>
        >(2, "Bob"))
    );

    // 測試表級 UNIQUE 約束
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>
        >(2, "Bob", "alice@example.com"))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<EmailColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 1);
}

// 測試 PrimaryKeyX 與 DESC 排序
TEST(TableConstraintTest, TableConstraintPrimaryKeyDesc) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_pk_desc_table", IdColumn, NameColumn,
        TablePrimaryKey<TypeGroup<ColumnWithOrder<IdColumn, OrderType::DESC> > > >;
    Database<TestTable> db("test_database.db", true);

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
}

// 測試 UniqueX 與衝突處理
TEST(TableConstraintTest, TableConstraintUniqueWithConflict) {
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using TestTable = Table<"test_unique_conflict", NameColumn, EmailColumn,
        TableUnique<TypeGroup<EmailColumn>, ConflictCause::IGNORE> >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >("Alice", "alice@example.com");

    // 由於設定 IGNORE，插入重複值不會拋出異常，而是被忽略
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >("Bob", "alice@example.com");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<EmailColumn>()
    ).Results();

    // 只有第一筆成功插入
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

// 測試 PrimaryKeyX 複合主鍵每個欄位不同排序
TEST(TableConstraintTest, TableConstraintPrimaryKeyMixedOrder) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using TestTable = Table<"test_pk_mixed_order", IdColumn, NameColumn, AgeColumn,
        TablePrimaryKey<TypeGroup<
            ColumnWithOrder<IdColumn, OrderType::ASC>,
            ColumnWithOrder<NameColumn, OrderType::DESC>
        > > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >(1, "Alice", 30);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >(1, "Bob", 25);

    // 嘗試插入重複的複合主鍵
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<AgeColumn>
        >(1, "Alice", 35))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<AgeColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試 UniqueX 複合唯一約束每個欄位不同排序
TEST(TableConstraintTest, TableConstraintUniqueMixedOrder) {
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using PhoneColumn = Column<"phone", ExprResultType::TEXT>;
    using TestTable = Table<"test_unique_mixed_order", NameColumn, EmailColumn, PhoneColumn,
        TableUnique<TypeGroup<
            ColumnWithOrder<EmailColumn, OrderType::DESC>,
            ColumnWithOrder<PhoneColumn, OrderType::ASC>
        > > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>
    >("Alice", "alice@example.com", "123-4567");

    // 不同的 email 和 phone 組合，應該成功
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>
    >("Bob", "bob@example.com", "123-4567");

    // 嘗試插入重複的複合唯一鍵
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>
        >("Charlie", "alice@example.com", "123-4567"))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<EmailColumn>(),
        TestTable::TableColumn<PhoneColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合使用有序和無序的欄位
TEST(TableConstraintTest, TableConstraintMixedOrderedUnordered) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using TestTable = Table<"test_mixed_ordered_unordered", IdColumn, NameColumn, EmailColumn,
        TablePrimaryKey<TypeGroup<
            ColumnWithOrder<IdColumn, OrderType::DESC>,
            NameColumn // 未指定排序，使用預設
        > > >;
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
    >(1, "Bob", "bob@example.com");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試多個 UNIQUE 約束
TEST(TableConstraintTest, TableConstraintMultipleUnique) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using PhoneColumn = Column<"phone", ExprResultType::TEXT>;
    using UsernameColumn = Column<"username", ExprResultType::TEXT>;

    // 一個表可以有多個 UNIQUE 約束
    using TestTable = Table<"test_multiple_unique",
        IdColumn, UsernameColumn, EmailColumn, PhoneColumn,
        TableUnique<TypeGroup<EmailColumn> >,
        TableUnique<TypeGroup<PhoneColumn> >,
        TableUnique<TypeGroup<UsernameColumn> > >;
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<UsernameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>
    >(1, "alice", "alice@example.com", "123-4567");

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<UsernameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>
        >(2, "bob", "alice@example.com", "234-5678"))
    );

    // 嘗試插入重複的 phone，應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<UsernameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>
        >(2, "bob", "bob@example.com", "123-4567"))
    );

    // 嘗試插入重複的 username，應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<UsernameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>
        >(2, "alice", "bob@example.com", "234-5678"))
    );

    // 插入完全不重複的資料，應該成功
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<UsernameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>
    >(2, "bob", "bob@example.com", "234-5678");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<UsernameColumn>(),
        TestTable::TableColumn<EmailColumn>(),
        TestTable::TableColumn<PhoneColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), "alice");
    EXPECT_EQ(std::get<1>(results[1]), "bob");
}

// 測試複合 UNIQUE 約束組合
TEST(TableConstraintTest, TableConstraintMultipleCompositeUnique) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using FirstNameColumn = Column<"first_name", ExprResultType::TEXT>;
    using LastNameColumn = Column<"last_name", ExprResultType::TEXT>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;

    // 多個複合 UNIQUE 約束
    using TestTable = Table<"test_multiple_composite_unique",
        IdColumn, FirstNameColumn, LastNameColumn, EmailColumn,
        TableUnique<TypeGroup<FirstNameColumn, LastNameColumn> >, // 名字組合唯一
        TableUnique<TypeGroup<EmailColumn> > >; // email 也要唯一
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<FirstNameColumn>,
        TestTable::TableColumn<LastNameColumn>,
        TestTable::TableColumn<EmailColumn>
    >(1, "John", "Doe", "john.doe@example.com");

    // 相同的 first_name 但不同的 last_name，應該成功
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<FirstNameColumn>,
        TestTable::TableColumn<LastNameColumn>,
        TestTable::TableColumn<EmailColumn>
    >(2, "John", "Smith", "john.smith@example.com");

    // 嘗試插入重複的名字組合，應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<FirstNameColumn>,
            TestTable::TableColumn<LastNameColumn>,
            TestTable::TableColumn<EmailColumn>
        >(3, "John", "Doe", "another@example.com"))
    );

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<FirstNameColumn>,
            TestTable::TableColumn<LastNameColumn>,
            TestTable::TableColumn<EmailColumn>
        >(3, "Jane", "Doe", "john.doe@example.com"))
    );

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<FirstNameColumn>(),
        TestTable::TableColumn<LastNameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試 PRIMARY KEY 與多個 UNIQUE 約束組合
TEST(TableConstraintTest, TableConstraintPrimaryKeyWithMultipleUnique) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;
    using PhoneColumn = Column<"phone", ExprResultType::TEXT>;
    using SsnColumn = Column<"ssn", ExprResultType::TEXT>; // Social Security Number

    using TestTable = Table<"test_pk_multi_unique",
        IdColumn, NameColumn, EmailColumn, PhoneColumn, SsnColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        TableUnique<TypeGroup<EmailColumn> >,
        TableUnique<TypeGroup<PhoneColumn> >,
        TableUnique<TypeGroup<SsnColumn> > >;
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>,
        TestTable::TableColumn<SsnColumn>
    >(1, "Alice", "alice@example.com", "123-4567", "111-11-1111");

    // 測試 PRIMARY KEY 唯一性
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>,
            TestTable::TableColumn<SsnColumn>
        >(1, "Bob", "bob@example.com", "234-5678", "222-22-2222")) // 重複的 id
    );

    // 測試第一個 UNIQUE 約束（email）
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>,
            TestTable::TableColumn<SsnColumn>
        >(2, "Bob", "alice@example.com", "234-5678", "222-22-2222")) // 重複的 email
    );

    // 測試第二個 UNIQUE 約束（phone）
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>,
            TestTable::TableColumn<SsnColumn>
        >(2, "Bob", "bob@example.com", "123-4567", "222-22-2222")) // 重複的 phone
    );

    // 測試第三個 UNIQUE 約束（ssn）
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<EmailColumn>,
            TestTable::TableColumn<PhoneColumn>,
            TestTable::TableColumn<SsnColumn>
        >(2, "Bob", "bob@example.com", "234-5678", "111-11-1111")) // 重複的 ssn
    );

    // 插入完全不重複的資料，應該成功
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>,
        TestTable::TableColumn<SsnColumn>
    >(2, "Bob", "bob@example.com", "234-5678", "222-22-2222");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合單一和複合 UNIQUE 約束
TEST(TableConstraintTest, TableConstraintMixedSingleCompositeUnique) {
    using IdColumn = Column<"id", ExprResultType::INTEGER>;
    using CountryColumn = Column<"country", ExprResultType::TEXT>;
    using CityColumn = Column<"city", ExprResultType::TEXT>;
    using EmailColumn = Column<"email", ExprResultType::TEXT>;

    using TestTable = Table<"test_mixed_unique",
        IdColumn, NameColumn, CountryColumn, CityColumn, EmailColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        TableUnique<TypeGroup<EmailColumn> >, // 單一欄位
        TableUnique<TypeGroup<CountryColumn, CityColumn, NameColumn> > >; // 複合欄位
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<CountryColumn>,
        TestTable::TableColumn<CityColumn>,
        TestTable::TableColumn<EmailColumn>
    >(1, "Alice", "USA", "New York", "alice@example.com");

    // 相同的名字和地點但不同的 email，由於複合約束相同應該失敗
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<CountryColumn>,
            TestTable::TableColumn<CityColumn>,
            TestTable::TableColumn<EmailColumn>
        >(2, "Alice", "USA", "New York", "alice2@example.com"))
    );

    // 不同的名字但相同的 email，應該失敗（email unique 約束）
    EXPECT_ANY_THROW((
        db.GetTable<TestTable>().Insert<
            TestTable::TableColumn<IdColumn>,
            TestTable::TableColumn<NameColumn>,
            TestTable::TableColumn<CountryColumn>,
            TestTable::TableColumn<CityColumn>,
            TestTable::TableColumn<EmailColumn>
        >(2, "Bob", "USA", "Boston", "alice@example.com"))
    );

    // 相同的名字但不同的地點和 email，應該成功
    db.GetTable<TestTable>().Insert<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<CountryColumn>,
        TestTable::TableColumn<CityColumn>,
        TestTable::TableColumn<EmailColumn>
    >(2, "Alice", "USA", "Boston", "alice.boston@example.com");

    auto results = db.GetTable<TestTable>().Select(
        TestTable::TableColumn<IdColumn>(),
        TestTable::TableColumn<NameColumn>(),
        TestTable::TableColumn<CityColumn>()
    ).Results();

    EXPECT_EQ(results.size(), 2);
}
#pragma once
#include "Common.hpp"

// ============ Table Constraint 測試 ============

// 測試 PrimaryKey (單欄位主鍵)
TEST(TableConstraintTest, TableConstraintPrimaryKeySingle) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_pk_single", IdColumn, NameColumn,
        TablePrimaryKey<TypeGroup<IdColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice")
    );

    // 嘗試插入重複的主鍵，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1),
            TestTable::MakeTableColumn<NameColumn>("Bob")
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<IdColumn>>(results[0]).value, 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

// 測試 PrimaryKey (複合主鍵)
TEST(TableConstraintTest, TableConstraintPrimaryKeyComposite) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_pk_composite", IdColumn, NameColumn, AgeColumn,
        TablePrimaryKey<TypeGroup<IdColumn, NameColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<AgeColumn>(30)
    );

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<AgeColumn>(25)
    );

    // 嘗試插入重複的複合主鍵，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1),
            TestTable::MakeTableColumn<NameColumn>("Alice"),
            TestTable::MakeTableColumn<AgeColumn>(35)
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試 Unique (單欄位唯一約束)
TEST(TableConstraintTest, TableConstraintUniqueSingle) {
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_unique_single", NameColumn, EmailColumn,
        TableUnique<TypeGroup<EmailColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<EmailColumn>>(results[0]).value, "alice@example.com");
}

// 測試 Unique (複合唯一約束)
TEST(TableConstraintTest, TableConstraintUniqueComposite) {
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_unique_composite", NameColumn, EmailColumn,
        TableUnique<TypeGroup<NameColumn, EmailColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    // 相同的 name 但不同的 email，應該成功
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice2@example.com")
    );

    // 嘗試插入重複的複合唯一鍵，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<NameColumn>("Alice"),
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合使用欄位約束和表約束
TEST(TableConstraintTest, TableConstraintMixed) {
    using IdColumn = Column<"id", column_type::INTEGER, ColumnPrimaryKey<> >;
    using EmailColumn = Column<"email", column_type::TEXT, ColumnNotNull<> >;
    using TestTable = Table<"test_mixed_constraints", IdColumn, NameColumn, EmailColumn,
        TableUnique<TypeGroup<EmailColumn> > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    // 測試主鍵唯一性
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("bob@example.com")
        )
    );

    // 測試 NOT NULL 約束
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Bob")
        )
    );

    // 測試表級 UNIQUE 約束
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
}

// 測試 PrimaryKeyX 與 DESC 排序
TEST(TableConstraintTest, TableConstraintPrimaryKeyDesc) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_pk_desc_table", IdColumn, NameColumn,
        TablePrimaryKey<TypeGroup<ColumnWithOrder<IdColumn, OrderType::DESC> > > >;
    Database<TestTable> db("test_database.db", true);

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
}

// 測試 UniqueX 與衝突處理
TEST(TableConstraintTest, TableConstraintUniqueWithConflict) {
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_unique_conflict", NameColumn, EmailColumn,
        TableUnique<TypeGroup<EmailColumn>, ConflictCause::IGNORE> >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    // 由於設定 IGNORE，插入重複值不會拋出異常，而是被忽略
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>
    >().Results();

    // 只有第一筆成功插入
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

// 測試 PrimaryKeyX 複合主鍵每個欄位不同排序
TEST(TableConstraintTest, TableConstraintPrimaryKeyMixedOrder) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_pk_mixed_order", IdColumn, NameColumn, AgeColumn,
        TablePrimaryKey<TypeGroup<
            ColumnWithOrder<IdColumn, OrderType::ASC>,
            ColumnWithOrder<NameColumn, OrderType::DESC>
        > > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<AgeColumn>(30)
    );

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<AgeColumn>(25)
    );

    // 嘗試插入重複的複合主鍵
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1),
            TestTable::MakeTableColumn<NameColumn>("Alice"),
            TestTable::MakeTableColumn<AgeColumn>(35)
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<AgeColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試 UniqueX 複合唯一約束每個欄位不同排序
TEST(TableConstraintTest, TableConstraintUniqueMixedOrder) {
    using EmailColumn = Column<"email", column_type::TEXT>;
    using PhoneColumn = Column<"phone", column_type::TEXT>;
    using TestTable = Table<"test_unique_mixed_order", NameColumn, EmailColumn, PhoneColumn,
        TableUnique<TypeGroup<
            ColumnWithOrder<EmailColumn, OrderType::DESC>,
            ColumnWithOrder<PhoneColumn, OrderType::ASC>
        > > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com"),
        TestTable::MakeTableColumn<PhoneColumn>("123-4567")
    );

    // 不同的 email 和 phone 組合，應該成功
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
        TestTable::MakeTableColumn<PhoneColumn>("123-4567")
    );

    // 嘗試插入重複的複合唯一鍵
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<NameColumn>("Charlie"),
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("123-4567")
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合使用有序和無序的欄位
TEST(TableConstraintTest, TableConstraintMixedOrderedUnordered) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_mixed_ordered_unordered", IdColumn, NameColumn, EmailColumn,
        TablePrimaryKey<TypeGroup<
            ColumnWithOrder<IdColumn, OrderType::DESC>,
            NameColumn // 未指定排序，使用預設
        > > >;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<EmailColumn>("bob@example.com")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試多個 UNIQUE 約束
TEST(TableConstraintTest, TableConstraintMultipleUnique) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using EmailColumn = Column<"email", column_type::TEXT>;
    using PhoneColumn = Column<"phone", column_type::TEXT>;
    using UsernameColumn = Column<"username", column_type::TEXT>;

    // 一個表可以有多個 UNIQUE 約束
    using TestTable = Table<"test_multiple_unique",
        IdColumn, UsernameColumn, EmailColumn, PhoneColumn,
        TableUnique<TypeGroup<EmailColumn> >,
        TableUnique<TypeGroup<PhoneColumn> >,
        TableUnique<TypeGroup<UsernameColumn> > >;
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<UsernameColumn>("alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com"),
        TestTable::MakeTableColumn<PhoneColumn>("123-4567")
    );

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<UsernameColumn>("bob"),
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("234-5678")
        )
    );

    // 嘗試插入重複的 phone，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<UsernameColumn>("bob"),
            TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("123-4567")
        )
    );

    // 嘗試插入重複的 username，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<UsernameColumn>("alice"),
            TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("234-5678")
        )
    );

    // 插入完全不重複的資料，應該成功
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(2),
        TestTable::MakeTableColumn<UsernameColumn>("bob"),
        TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
        TestTable::MakeTableColumn<PhoneColumn>("234-5678")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<UsernameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<PhoneColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<TestTable::TableColumn<UsernameColumn>>(results[0]).value, "alice");
    EXPECT_EQ(std::get<TestTable::TableColumn<UsernameColumn>>(results[1]).value, "bob");
}

// 測試複合 UNIQUE 約束組合
TEST(TableConstraintTest, TableConstraintMultipleCompositeUnique) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using FirstNameColumn = Column<"first_name", column_type::TEXT>;
    using LastNameColumn = Column<"last_name", column_type::TEXT>;
    using EmailColumn = Column<"email", column_type::TEXT>;

    // 多個複合 UNIQUE 約束
    using TestTable = Table<"test_multiple_composite_unique",
        IdColumn, FirstNameColumn, LastNameColumn, EmailColumn,
        TableUnique<TypeGroup<FirstNameColumn, LastNameColumn> >, // 名字組合唯一
        TableUnique<TypeGroup<EmailColumn> > >; // email 也要唯一
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<FirstNameColumn>("John"),
        TestTable::MakeTableColumn<LastNameColumn>("Doe"),
        TestTable::MakeTableColumn<EmailColumn>("john.doe@example.com")
    );

    // 相同的 first_name 但不同的 last_name，應該成功
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(2),
        TestTable::MakeTableColumn<FirstNameColumn>("John"),
        TestTable::MakeTableColumn<LastNameColumn>("Smith"),
        TestTable::MakeTableColumn<EmailColumn>("john.smith@example.com")
    );

    // 嘗試插入重複的名字組合，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(3),
            TestTable::MakeTableColumn<FirstNameColumn>("John"),
            TestTable::MakeTableColumn<LastNameColumn>("Doe"),
            TestTable::MakeTableColumn<EmailColumn>("another@example.com")
        )
    );

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(3),
            TestTable::MakeTableColumn<FirstNameColumn>("Jane"),
            TestTable::MakeTableColumn<LastNameColumn>("Doe"),
            TestTable::MakeTableColumn<EmailColumn>("john.doe@example.com")
        )
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<FirstNameColumn>,
        TestTable::TableColumn<LastNameColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試 PRIMARY KEY 與多個 UNIQUE 約束組合
TEST(TableConstraintTest, TableConstraintPrimaryKeyWithMultipleUnique) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using EmailColumn = Column<"email", column_type::TEXT>;
    using PhoneColumn = Column<"phone", column_type::TEXT>;
    using SsnColumn = Column<"ssn", column_type::TEXT>; // Social Security Number

    using TestTable = Table<"test_pk_multi_unique",
        IdColumn, NameColumn, EmailColumn, PhoneColumn, SsnColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        TableUnique<TypeGroup<EmailColumn> >,
        TableUnique<TypeGroup<PhoneColumn> >,
        TableUnique<TypeGroup<SsnColumn> > >;
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com"),
        TestTable::MakeTableColumn<PhoneColumn>("123-4567"),
        TestTable::MakeTableColumn<SsnColumn>("111-11-1111")
    );

    // 測試 PRIMARY KEY 唯一性
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1), // 重複的 id
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("234-5678"),
            TestTable::MakeTableColumn<SsnColumn>("222-22-2222")
        )
    );

    // 測試第一個 UNIQUE 約束（email）
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com"), // 重複的 email
            TestTable::MakeTableColumn<PhoneColumn>("234-5678"),
            TestTable::MakeTableColumn<SsnColumn>("222-22-2222")
        )
    );

    // 測試第二個 UNIQUE 約束（phone）
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("123-4567"), // 重複的 phone
            TestTable::MakeTableColumn<SsnColumn>("222-22-2222")
        )
    );

    // 測試第三個 UNIQUE 約束（ssn）
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
            TestTable::MakeTableColumn<PhoneColumn>("234-5678"),
            TestTable::MakeTableColumn<SsnColumn>("111-11-1111") // 重複的 ssn
        )
    );

    // 插入完全不重複的資料，應該成功
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(2),
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<EmailColumn>("bob@example.com"),
        TestTable::MakeTableColumn<PhoneColumn>("234-5678"),
        TestTable::MakeTableColumn<SsnColumn>("222-22-2222")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合單一和複合 UNIQUE 約束
TEST(TableConstraintTest, TableConstraintMixedSingleCompositeUnique) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using CountryColumn = Column<"country", column_type::TEXT>;
    using CityColumn = Column<"city", column_type::TEXT>;
    using EmailColumn = Column<"email", column_type::TEXT>;

    using TestTable = Table<"test_mixed_unique",
        IdColumn, NameColumn, CountryColumn, CityColumn, EmailColumn,
        TablePrimaryKey<TypeGroup<IdColumn> >,
        TableUnique<TypeGroup<EmailColumn> >, // 單一欄位
        TableUnique<TypeGroup<CountryColumn, CityColumn, NameColumn> > >; // 複合欄位
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<CountryColumn>("USA"),
        TestTable::MakeTableColumn<CityColumn>("New York"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    // 相同的名字和地點但不同的 email，由於複合約束相同應該失敗
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Alice"),
            TestTable::MakeTableColumn<CountryColumn>("USA"),
            TestTable::MakeTableColumn<CityColumn>("New York"),
            TestTable::MakeTableColumn<EmailColumn>("alice2@example.com")
        )
    );

    // 不同的名字但相同的 email，應該失敗（email unique 約束）
    EXPECT_ANY_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(2),
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<CountryColumn>("USA"),
            TestTable::MakeTableColumn<CityColumn>("Boston"),
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
        )
    );

    // 相同的名字但不同的地點和 email，應該成功
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(2),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<CountryColumn>("USA"),
        TestTable::MakeTableColumn<CityColumn>("Boston"),
        TestTable::MakeTableColumn<EmailColumn>("alice.boston@example.com")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<CityColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);
}
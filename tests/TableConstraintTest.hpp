#pragma once
#include "Common.hpp"

// ============ Table Constraint 測試 ============

class TableConstraintTest : public ::testing::Test {
protected:
    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 測試 PrimaryKey (單欄位主鍵)
TEST_F(TableConstraintTest, TableConstraintPrimaryKeySingle) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_pk_single">(
        std::make_tuple(IdColumn{}, NameColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, decltype(NameColumn)>(1, "Alice");

    // 嘗試插入重複的主鍵，應該失敗
    EXPECT_ANY_THROW(
        (testTable.Insert<IdColumn, decltype(NameColumn)>(1, "Bob"))
    );

    auto results = testTable.Select(testTable[IdColumn{}], testTable[NameColumn]).Results().ToVector();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

// 測試 PrimaryKey (複合主鍵)
TEST_F(TableConstraintTest, TableConstraintPrimaryKeyComposite) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_pk_composite">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(IdColumn{}, NameColumn))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 30);
    testTable.Insert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Bob", 25);

    // 嘗試插入重複的複合主鍵，應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 35)
    ));

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[AgeColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試 Unique (單欄位唯一約束)
TEST_F(TableConstraintTest, TableConstraintUniqueSingle) {
    using EmailColumn = Column<"email", DataType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_unique_single">(
        std::make_tuple(NameColumn, EmailColumn{}),
        std::make_tuple(TableUnique(std::make_tuple(EmailColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<decltype(NameColumn), EmailColumn>("Alice", "alice@example.com");

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<decltype(NameColumn), EmailColumn>("Bob", "alice@example.com")
    ));

    auto results = testTable.Select(testTable[NameColumn], testTable[EmailColumn{}]).Results().ToVector();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "alice@example.com");
}

// 測試 Unique (複合唯一約束)
TEST_F(TableConstraintTest, TableConstraintUniqueComposite) {
    using EmailColumn = Column<"email", DataType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_unique_composite">(
        std::make_tuple(NameColumn, EmailColumn{}),
        std::make_tuple(TableUnique(std::make_tuple(NameColumn, EmailColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<decltype(NameColumn), EmailColumn>("Alice", "alice@example.com");

    // 相同的 name 但不同的 email，應該成功
    testTable.Insert<decltype(NameColumn), EmailColumn>("Alice", "alice2@example.com");

    // 嘗試插入重複的複合唯一鍵，應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<decltype(NameColumn), EmailColumn>("Alice", "alice@example.com")
    ));

    auto results = testTable.Select(testTable[NameColumn], testTable[EmailColumn{}]).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合使用欄位約束和表約束
TEST_F(TableConstraintTest, TableConstraintMixed) {
    using IdColumn = Column<"id", DataType::INTEGER, ColumnPrimaryKey<>>;
    using EmailColumn = Column<"email", DataType::TEXT, ColumnNotNull<>>;
    auto testTableDef = MakeTableDefinition<"test_mixed_constraints">(
        std::make_tuple(IdColumn{}, NameColumn, EmailColumn{}),
        std::make_tuple(TableUnique(std::make_tuple(EmailColumn{}))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn>(1, "Alice", "alice@example.com");

    // 測試主鍵唯一性
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn>(1, "Bob", "bob@example.com")
    ));

    // 測試 NOT NULL 約束
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn)>(2, "Bob")
    ));

    // 測試表級 UNIQUE 約束
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn>(2, "Bob", "alice@example.com")
    ));

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[EmailColumn{}]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 1);
}

// 測試 PrimaryKey 與 DESC 排序
TEST_F(TableConstraintTest, TableConstraintPrimaryKeyDesc) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_pk_desc_table">(
        std::make_tuple(IdColumn{}, NameColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(ColumnWithOrder(IdColumn(), OrderType::DESC)))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, decltype(NameColumn)>(1, "Alice");
    testTable.Insert<IdColumn, decltype(NameColumn)>(2, "Bob");

    auto results = testTable.Select(testTable[IdColumn{}], testTable[NameColumn]).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試 Unique 與衝突處理
TEST_F(TableConstraintTest, TableConstraintUniqueWithConflict) {
    using EmailColumn = Column<"email", DataType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_unique_conflict">(
        std::make_tuple(NameColumn, EmailColumn{}),
        std::make_tuple(TableUnique(std::make_tuple(EmailColumn{}), ConflictCause::IGNORE)),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<decltype(NameColumn), EmailColumn>("Alice", "alice@example.com");

    // 由於設定 IGNORE，插入重複值不會拋出異常，而是被忽略
    testTable.Insert<decltype(NameColumn), EmailColumn>("Bob", "alice@example.com");

    auto results = testTable.Select(testTable[NameColumn], testTable[EmailColumn{}]).Results().ToVector();

    // 只有第一筆成功插入
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

// 測試 PrimaryKey 複合主鍵每個欄位不同排序
TEST_F(TableConstraintTest, TableConstraintPrimaryKeyMixedOrder) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    auto testTableDef = MakeTableDefinition<"test_pk_mixed_order">(
        std::make_tuple(IdColumn{}, NameColumn, AgeColumn),
        std::make_tuple(TablePrimaryKey(std::make_tuple(
            ColumnWithOrder(IdColumn(), OrderType::ASC),
            ColumnWithOrder(NameColumn, OrderType::DESC)
        ))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 30);
    testTable.Insert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Bob", 25);

    // 嘗試插入重複的複合主鍵
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), decltype(AgeColumn)>(1, "Alice", 35)
    ));

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[AgeColumn]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試 Unique 複合唯一約束每個欄位不同排序
TEST_F(TableConstraintTest, TableConstraintUniqueMixedOrder) {
    using EmailColumn = Column<"email", DataType::TEXT>;
    using PhoneColumn = Column<"phone", DataType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_unique_mixed_order">(
        std::make_tuple(NameColumn, EmailColumn{}, PhoneColumn{}),
        std::make_tuple(TableUnique(std::make_tuple(
            ColumnWithOrder(EmailColumn(), OrderType::DESC),
            ColumnWithOrder(PhoneColumn(), OrderType::ASC)
        ))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<decltype(NameColumn), EmailColumn, PhoneColumn>("Alice", "alice@example.com", "123-4567");

    // 不同的 email 和 phone 組合，應該成功
    testTable.Insert<decltype(NameColumn), EmailColumn, PhoneColumn>("Bob", "bob@example.com", "123-4567");

    // 嘗試插入重複的複合唯一鍵
    EXPECT_ANY_THROW((
        testTable.Insert<decltype(NameColumn), EmailColumn, PhoneColumn>("Charlie", "alice@example.com", "123-4567")
    ));

    auto results = testTable.Select(
        testTable[NameColumn],
        testTable[EmailColumn{}],
        testTable[PhoneColumn{}]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合使用有序和無序的欄位
TEST_F(TableConstraintTest, TableConstraintMixedOrderedUnordered) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    using EmailColumn = Column<"email", DataType::TEXT>;
    auto testTableDef = MakeTableDefinition<"test_mixed_ordered_unordered">(
        std::make_tuple(IdColumn{}, NameColumn, EmailColumn{}),
        std::make_tuple(TablePrimaryKey(std::make_tuple(
            ColumnWithOrder(IdColumn(), OrderType::DESC),
            NameColumn // 未指定排序，使用預設
        ))),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn>(1, "Alice", "alice@example.com");
    testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn>(1, "Bob", "bob@example.com");

    auto results = testTable.Select(testTable[IdColumn{}], testTable[NameColumn]).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試多個 UNIQUE 約束
TEST_F(TableConstraintTest, TableConstraintMultipleUnique) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    using EmailColumn = Column<"email", DataType::TEXT>;
    using PhoneColumn = Column<"phone", DataType::TEXT>;
    using UsernameColumn = Column<"username", DataType::TEXT>;

    // 一個表可以有多個 UNIQUE 約束
    auto testTableDef = MakeTableDefinition<"test_multiple_unique">(
        std::make_tuple(IdColumn{}, UsernameColumn{}, EmailColumn{}, PhoneColumn{}),
        std::make_tuple(
            TableUnique(std::make_tuple(EmailColumn{})),
            TableUnique(std::make_tuple(PhoneColumn{})),
            TableUnique(std::make_tuple(UsernameColumn{}))
        ),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 插入第一筆資料
    testTable.Insert<IdColumn, UsernameColumn, EmailColumn, PhoneColumn>(
        1, "alice", "alice@example.com", "123-4567"
    );

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, UsernameColumn, EmailColumn, PhoneColumn>(
            2, "bob", "alice@example.com", "234-5678"
        )
    ));

    // 嘗試插入重複的 phone，應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, UsernameColumn, EmailColumn, PhoneColumn>(
            2, "bob", "bob@example.com", "123-4567"
        )
    ));

    // 嘗試插入重複的 username，應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, UsernameColumn, EmailColumn, PhoneColumn>(
            2, "alice", "bob@example.com", "234-5678"
        )
    ));

    // 插入完全不重複的資料，應該成功
    testTable.Insert<IdColumn, UsernameColumn, EmailColumn, PhoneColumn>(
        2, "bob", "bob@example.com", "234-5678"
    );

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[UsernameColumn{}],
        testTable[EmailColumn{}],
        testTable[PhoneColumn{}]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), "alice");
    EXPECT_EQ(std::get<1>(results[1]), "bob");
}

// 測試複合 UNIQUE 約束組合
TEST_F(TableConstraintTest, TableConstraintMultipleCompositeUnique) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    using FirstNameColumn = Column<"first_name", DataType::TEXT>;
    using LastNameColumn = Column<"last_name", DataType::TEXT>;
    using EmailColumn = Column<"email", DataType::TEXT>;

    // 多個複合 UNIQUE 約束
    auto testTableDef = MakeTableDefinition<"test_multiple_composite_unique">(
        std::make_tuple(IdColumn{}, FirstNameColumn{}, LastNameColumn{}, EmailColumn{}),
        std::make_tuple(
            TableUnique(std::make_tuple(FirstNameColumn{}, LastNameColumn{})), // 名字組合唯一
            TableUnique(std::make_tuple(EmailColumn{})) // email 也要唯一
        ),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 插入第一筆資料
    testTable.Insert<IdColumn, FirstNameColumn, LastNameColumn, EmailColumn>(
        1, "John", "Doe", "john.doe@example.com"
    );

    // 相同的 first_name 但不同的 last_name，應該成功
    testTable.Insert<IdColumn, FirstNameColumn, LastNameColumn, EmailColumn>(
        2, "John", "Smith", "john.smith@example.com"
    );

    // 嘗試插入重複的名字組合，應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, FirstNameColumn, LastNameColumn, EmailColumn>(
            3, "John", "Doe", "another@example.com"
        )
    ));

    // 嘗試插入重複的 email，應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, FirstNameColumn, LastNameColumn, EmailColumn>(
            3, "Jane", "Doe", "john.doe@example.com"
        )
    ));

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[FirstNameColumn{}],
        testTable[LastNameColumn{}]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試 PRIMARY KEY 與多個 UNIQUE 約束組合
TEST_F(TableConstraintTest, TableConstraintPrimaryKeyWithMultipleUnique) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    using EmailColumn = Column<"email", DataType::TEXT>;
    using PhoneColumn = Column<"phone", DataType::TEXT>;
    using SsnColumn = Column<"ssn", DataType::TEXT>; // Social Security Number

    auto testTableDef = MakeTableDefinition<"test_pk_multi_unique">(
        std::make_tuple(IdColumn{}, NameColumn, EmailColumn{}, PhoneColumn{}, SsnColumn{}),
        std::make_tuple(
            TablePrimaryKey(std::make_tuple(IdColumn{})),
            TableUnique(std::make_tuple(EmailColumn{})),
            TableUnique(std::make_tuple(PhoneColumn{})),
            TableUnique(std::make_tuple(SsnColumn{}))
        ),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 插入第一筆資料
    testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn, SsnColumn>(
        1, "Alice", "alice@example.com", "123-4567", "111-11-1111"
    );

    // 測試 PRIMARY KEY 唯一性
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn, SsnColumn>(
            1, "Bob", "bob@example.com", "234-5678", "222-22-2222"
        )
    ));

    // 測試第一個 UNIQUE 約束（email）
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn, SsnColumn>(
            2, "Bob", "alice@example.com", "234-5678", "222-22-2222"
        )
    ));

    // 測試第二個 UNIQUE 約束（phone）
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn, SsnColumn>(
            2, "Bob", "bob@example.com", "123-4567", "222-22-2222"
        )
    ));

    // 測試第三個 UNIQUE 約束（ssn）
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn, SsnColumn>(
            2, "Bob", "bob@example.com", "234-5678", "111-11-1111"
        )
    ));

    // 插入完全不重複的資料，應該成功
    testTable.Insert<IdColumn, decltype(NameColumn), EmailColumn, PhoneColumn, SsnColumn>(
        2, "Bob", "bob@example.com", "234-5678", "222-22-2222"
    );

    auto results = testTable.Select(testTable[IdColumn{}], testTable[NameColumn]).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}

// 測試混合單一和複合 UNIQUE 約束
TEST_F(TableConstraintTest, TableConstraintMixedSingleCompositeUnique) {
    using IdColumn = Column<"id", DataType::INTEGER>;
    using CountryColumn = Column<"country", DataType::TEXT>;
    using CityColumn = Column<"city", DataType::TEXT>;
    using EmailColumn = Column<"email", DataType::TEXT>;

    auto testTableDef = MakeTableDefinition<"test_mixed_unique">(
        std::make_tuple(IdColumn{}, NameColumn, CountryColumn{}, CityColumn{}, EmailColumn{}),
        std::make_tuple(
            TablePrimaryKey(std::make_tuple(IdColumn{})),
            TableUnique(std::make_tuple(EmailColumn{})), // 單一欄位
            TableUnique(std::make_tuple(CountryColumn{}, CityColumn{}, NameColumn)) // 複合欄位
        ),
        std::make_tuple()
    );
    Database<decltype(testTableDef)> db("test_database.db", testTableDef);
    auto &testTable = db.GetTable<decltype(testTableDef)>();

    // 插入第一筆資料
    testTable.Insert<IdColumn, decltype(NameColumn), CountryColumn, CityColumn, EmailColumn>(
        1, "Alice", "USA", "New York", "alice@example.com"
    );

    // 相同的名字和地點但不同的 email，由於複合約束相同應該失敗
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), CountryColumn, CityColumn, EmailColumn>(
            2, "Alice", "USA", "New York", "alice2@example.com"
        )
    ));

    // 不同的名字但相同的 email，應該失敗（email unique 約束）
    EXPECT_ANY_THROW((
        testTable.Insert<IdColumn, decltype(NameColumn), CountryColumn, CityColumn, EmailColumn>(
            2, "Bob", "USA", "Boston", "alice@example.com"
        )
    ));

    // 相同的名字但不同的地點和 email，應該成功
    testTable.Insert<IdColumn, decltype(NameColumn), CountryColumn, CityColumn, EmailColumn>(
        2, "Alice", "USA", "Boston", "alice.boston@example.com"
    );

    auto results = testTable.Select(
        testTable[IdColumn{}],
        testTable[NameColumn],
        testTable[CityColumn{}]
    ).Results().ToVector();

    EXPECT_EQ(results.size(), 2);
}


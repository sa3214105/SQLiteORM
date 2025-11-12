#include <iostream>
#include <gtest/gtest.h>
#include "../src/SQLiteHelper.hpp"

using namespace SQLiteHelper;

// 定義測試用的列結構
using NameColumn = Column<"name", column_type::TEXT>;
using AgeColumn = Column<"age", column_type::INTEGER>;
using ScoreColumn = Column<"score", column_type::REAL>;
using DeptColumn = Column<"dept", column_type::TEXT>;
using CityColumn = Column<"city", column_type::TEXT>;
using CountryColumn = Column<"country", column_type::TEXT>;

// 定義表類型
using UserTable = Table<"users", NameColumn, AgeColumn, ScoreColumn>;
using DeptTable = Table<"departments", DeptColumn, NameColumn>;
using CityTable = Table<"cities", NameColumn, CityColumn>;
using CountryTable = Table<"countries", CityColumn, CountryColumn>;

class SQLiteHelperTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 測試開始前清理數據庫
    }

    void TearDown() override {
        // 測試結束後清理
    }
};

// ============ Insert 測試 ============
TEST_F(SQLiteHelperTest, InsertSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

TEST_F(SQLiteHelperTest, InsertMultipleColumns) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(
        UserTable::MakeTableColumn<NameColumn>("Bob"),
        UserTable::MakeTableColumn<AgeColumn>(30),
        UserTable::MakeTableColumn<ScoreColumn>(95.5)
    );
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>
        , UserTable::TableColumn<ScoreColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Bob");
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 30);
    EXPECT_EQ(std::get<UserTable::TableColumn<ScoreColumn>>(results[0]).value, 95.5);
}

TEST_F(SQLiteHelperTest, InsertMultipleRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 3);
}

// ============ Select 測試 ============
TEST_F(SQLiteHelperTest, SelectAllRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("User1"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("User2"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
        AgeColumn> >().Results();
    EXPECT_EQ(results.size(), 2);
}

TEST_F(SQLiteHelperTest, SelectSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Test"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Test");
}

// ============ Where 條件測試 ============
TEST_F(SQLiteHelperTest, WhereEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("Alice")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

TEST_F(SQLiteHelperTest, WhereNotEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >()
            .Where(NotEqual<UserTable::TableColumn<NameColumn> >("Alice")).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST_F(SQLiteHelperTest, WhereGreaterThan) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >()
            .Where(GreaterThan<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 30);
}

TEST_F(SQLiteHelperTest, WhereLessThan) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >()
            .Where(LessThan<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 20);
}

TEST_F(SQLiteHelperTest, WhereGreaterThanEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >()
            .Where(GreaterThanEqual<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST_F(SQLiteHelperTest, WhereLessThanEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >()
            .Where(LessThanEqual<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results.size(), 2);
}

// ============ 邏輯運算符測試 (AND / OR) ============
TEST_F(SQLiteHelperTest, WhereAND) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(35));

    auto cond = Equal<UserTable::TableColumn<NameColumn> >("Alice") &&
                GreaterThan<UserTable::TableColumn<AgeColumn> >(25);
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(cond).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 35);
}

TEST_F(SQLiteHelperTest, WhereOR) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(35));

    auto cond = Equal<UserTable::TableColumn<NameColumn> >("Alice") ||
                Equal<UserTable::TableColumn<NameColumn> >("Bob");
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >()
            .Where(cond).Results();
    EXPECT_EQ(results.size(), 2);
}

// ============ Update 測試 ============
TEST_F(SQLiteHelperTest, UpdateSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("OldName"));

    db.GetTable<UserTable>().Update(UserTable::MakeTableColumn<NameColumn>("NewName"))
            .Where(Equal<UserTable::TableColumn<NameColumn> >("OldName")).Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "NewName");
}

TEST_F(SQLiteHelperTest, UpdateMultipleColumns) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(
        UserTable::MakeTableColumn<NameColumn>("Alice"),
        UserTable::MakeTableColumn<AgeColumn>(20)
    );

    db.GetTable<UserTable>().Update(
        UserTable::MakeTableColumn<NameColumn>("Bob"),
        UserTable::MakeTableColumn<AgeColumn>(30)
    ).Where(Equal<UserTable::TableColumn<NameColumn> >("Alice")).Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
        AgeColumn> >().Results();
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Bob");
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 30);
}

TEST_F(SQLiteHelperTest, UpdateMultipleRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));

    db.GetTable<UserTable>().Update(UserTable::MakeTableColumn<AgeColumn>(30))
            .Where(Equal<UserTable::TableColumn<AgeColumn> >(25)).Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >().Results();
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 30);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[1]).value, 30);
}

// ============ Delete 測試 ============
TEST_F(SQLiteHelperTest, DeleteSingleRow) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));

    db.GetTable<UserTable>().Delete()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("Alice")).Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Bob");
}

TEST_F(SQLiteHelperTest, DeleteMultipleRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    db.GetTable<UserTable>().Delete()
            .Where(Equal<UserTable::TableColumn<AgeColumn> >(20)).Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
}

TEST_F(SQLiteHelperTest, DeleteAllRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));

    db.GetTable<UserTable>().Delete().Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 0);
}

// ============ 複合操作測試 ============
TEST_F(SQLiteHelperTest, ComplexOperations) {
    Database<UserTable> db("test_database.db");

    // 插入數據
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(25),
                                    UserTable::MakeTableColumn<ScoreColumn>(90.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30),
                                    UserTable::MakeTableColumn<ScoreColumn>(85.0));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(28),
                                    UserTable::MakeTableColumn<ScoreColumn>(95.0));

    // 查詢年齡 > 25 的用戶
    auto results1 = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(GreaterThan<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results1.size(), 2);

    // 更新 Bob 的分數
    db.GetTable<UserTable>().Update(UserTable::MakeTableColumn<ScoreColumn>(92.0))
            .Where(Equal<UserTable::TableColumn<NameColumn> >("Bob")).Execute();

    // 查詢 Bob 的新分數
    auto results2 = db.GetTable<UserTable>().Select<UserTable::TableColumn<ScoreColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("Bob")).Results();
    EXPECT_EQ(std::get<UserTable::TableColumn<ScoreColumn>>(results2[0]).value, 92.0);

    // 刪除年齡 >= 30 的用戶
    db.GetTable<UserTable>().Delete()
            .Where(GreaterThanEqual<UserTable::TableColumn<AgeColumn> >(30)).Execute();

    auto results3 = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results3.size(), 2);
}

// ============ Transaction 測試 ============
TEST_F(SQLiteHelperTest, TransactionCommit) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transation) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("TxUser"),
                                        UserTable::MakeTableColumn<AgeColumn>(99));
    });
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("TxUser")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 99);
}

TEST_F(SQLiteHelperTest, TransactionRollback) {
    Database<UserTable> db("test_database.db");
    EXPECT_ANY_THROW(db.CreateTransaction([&db](auto &transation ) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("TxRollback"), UserTable::MakeTableColumn
            <AgeColumn>(88));
        throw std::runtime_error("Force rollback");
        }));
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("TxRollback")).Results();
    EXPECT_EQ(results.size(), 0);
}

TEST_F(SQLiteHelperTest, TransactionExplicitCommit) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transaction) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("ExplicitCommit"),
                                        UserTable::MakeTableColumn<AgeColumn>(77));
        transaction.Commit(); // 明確呼叫 Commit
    });
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("ExplicitCommit")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 77);
}

TEST_F(SQLiteHelperTest, TransactionExplicitRollback) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transaction) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("ExplicitRollback"),
                                        UserTable::MakeTableColumn<AgeColumn>(66));
        transaction.Rollback(); // 明確呼叫 Rollback
    });
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("ExplicitRollback")).Results();
    EXPECT_EQ(results.size(), 0);
}

// ============ Join 測試 ============
TEST_F(SQLiteHelperTest, InnerJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(40));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"),
                                    UserTable::MakeTableColumn<AgeColumn>(50));
    // DeptTable: Alice, David (只有交集部分)
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("David"));

    // InnerJoin on NameColumn - 只返回交集 (Alice, David)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().InnerJoin<DeptTable>(Equal<_NameColumn, __NameColumn>()).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // INNER JOIN 只有交集: Alice, David
    EXPECT_EQ(results.size(), 2);
    std::vector<std::string> names;
    for (const auto &row: results) {
        names.push_back(std::get<_NameColumn>(row).value);
    }
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Alice") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "David") != names.end());
}

TEST_F(SQLiteHelperTest, LeftJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(40));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"),
                                    UserTable::MakeTableColumn<AgeColumn>(50));
    // DeptTable: Alice, David, Eve
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("David"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("Sales"),
                                    DeptTable::MakeTableColumn<NameColumn>("Eve"));

    // LeftJoin - 返回左表的所有行 (Alice, Bob, Charlie, David)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().LeftJoin<DeptTable>(Equal<_NameColumn, __NameColumn>()).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // LEFT JOIN 應返回左表的所有行
    EXPECT_EQ(results.size(), 4);
    std::vector<std::string> names;
    for (const auto &row: results) {
        names.push_back(std::get<_NameColumn>(row).value);
    }
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Alice") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Bob") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Charlie") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "David") != names.end());
}

TEST_F(SQLiteHelperTest, RightJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    // DeptTable: Alice, David, Eve (比左表多)
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("David"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("Sales"),
                                    DeptTable::MakeTableColumn<NameColumn>("Eve"));

    // RightJoin - 返回右表的所有行 (Alice, David, Eve)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().RightJoin<DeptTable>(Equal<_NameColumn, __NameColumn>()).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // RIGHT JOIN 應返回右表的所有行 - 至少應該包含右表中的所有行
    EXPECT_GE(results.size(), 3);
}

TEST_F(SQLiteHelperTest, CrossJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    // DeptTable: HR, IT
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("X"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("Y"));

    // CrossJoin - 笛卡爾乘積 (2 x 2 = 4)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;

    auto results = db.GetTable<UserTable>().CrossJoin<DeptTable>(Equal(1, 1)).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // CROSS JOIN 應返回笛卡爾乘積的結果 (2 x 2 = 4)
    EXPECT_EQ(results.size(), 4);
}

TEST_F(SQLiteHelperTest, FullJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(40));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"),
                                    UserTable::MakeTableColumn<AgeColumn>(50));
    // DeptTable: Alice, David
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("David"));
    // FullJoin on NameColumn
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;
    auto results = db.GetTable<UserTable>().FullJoin<DeptTable>(Equal<_NameColumn, __NameColumn>()).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();
    // 應該有 4 筆: Alice(交集), Bob(左), Charlie(左), David(右)
    std::vector<std::string> names;
    std::vector<std::string> depts;
    for (const auto &row: results) {
        names.push_back(std::get<_NameColumn>(row).value);
        depts.push_back(std::get<_DeptColumn>(row).value);
    }
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Alice") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Bob") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Charlie") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "David") != names.end());
    EXPECT_TRUE(std::find(depts.begin(), depts.end(), "HR") != depts.end());
    EXPECT_TRUE(std::find(depts.begin(), depts.end(), "IT") != depts.end());
    EXPECT_EQ(results.size(), 4);
}

// ============ 多重 Join 測試 ============
TEST_F(SQLiteHelperTest, MultiJoin_Inner_Inner_ThreeTables) {
    Database<UserTable, CityTable, CountryTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<CityTable>().Delete().Execute();
    db.GetTable<CountryTable>().Delete().Execute();

    // users
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Eve"));

    // cities
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Alice"),
                                    CityTable::MakeTableColumn<CityColumn>("Paris"));
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Bob"),
                                    CityTable::MakeTableColumn<CityColumn>("Tokyo"));
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Mallory"),
                                    CityTable::MakeTableColumn<CityColumn>("Berlin"));

    // countries
    db.GetTable<CountryTable>().Insert(CountryTable::MakeTableColumn<CityColumn>("Paris"),
                                       CountryTable::MakeTableColumn<CountryColumn>("France"));
    db.GetTable<CountryTable>().Insert(CountryTable::MakeTableColumn<CityColumn>("Tokyo"),
                                       CountryTable::MakeTableColumn<CountryColumn>("Japan"));
    db.GetTable<CountryTable>().Insert(CountryTable::MakeTableColumn<CityColumn>("Berlin"),
                                       CountryTable::MakeTableColumn<CountryColumn>("Germany"));

    using UName = UserTable::TableColumn<NameColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using UCity = CityTable::TableColumn<CityColumn>;
    using CCty = CountryTable::TableColumn<CityColumn>;
    using UCountry = CountryTable::TableColumn<CountryColumn>;

    auto results = db.GetTable<UserTable>()
            .InnerJoin<CityTable>(Equal<UName, CName>())
            .InnerJoin<CountryTable>(Equal<UCity, CCty>())
            .Select<UName, UCity, UCountry>()
            .Results();

    ASSERT_EQ(results.size(), 2);
    std::vector<std::string> countries;
    for (auto &row: results) {
        countries.push_back(std::get<UCountry>(row).value);
    }
    EXPECT_NE(std::find(countries.begin(), countries.end(), "France"), countries.end());
    EXPECT_NE(std::find(countries.begin(), countries.end(), "Japan"), countries.end());
}

TEST_F(SQLiteHelperTest, MultiJoin_Left_Left_ThreeTables) {
    Database<UserTable, CityTable, CountryTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<CityTable>().Delete().Execute();
    db.GetTable<CountryTable>().Delete().Execute();

    // users
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));

    // cities (Charlie has no city)
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Alice"),
                                    CityTable::MakeTableColumn<CityColumn>("Paris"));
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Bob"),
                                    CityTable::MakeTableColumn<CityColumn>("Tokyo"));

    // countries (Tokyo has no country)
    db.GetTable<CountryTable>().Insert(CountryTable::MakeTableColumn<CityColumn>("Paris"),
                                       CountryTable::MakeTableColumn<CountryColumn>("France"));

    using UName = UserTable::TableColumn<NameColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using UCity = CityTable::TableColumn<CityColumn>;
    using CCty = CountryTable::TableColumn<CityColumn>;
    using UCountry = CountryTable::TableColumn<CountryColumn>;

    auto results = db.GetTable<UserTable>()
            .LeftJoin<CityTable>(Equal<UName, CName>())
            .LeftJoin<CountryTable>(Equal<UCity, CCty>())
            .Select<UName, UCity, UCountry>()
            .Results();

    ASSERT_EQ(results.size(), 3);
    // Ensure Charlie exists with empty city/country
    bool hasCharlie = false;
    bool hasEmptyForCharlie = false;
    for (auto &row: results) {
        if (std::get<UName>(row).value == "Charlie") {
            hasCharlie = true;
            hasEmptyForCharlie = std::get<UCity>(row).value.empty() && std::get<UCountry>(row).value.empty();
        }
    }
    EXPECT_TRUE(hasCharlie);
    EXPECT_TRUE(hasEmptyForCharlie);
}

TEST_F(SQLiteHelperTest, MultiJoin_Inner_Then_Left) {
    Database<UserTable, DeptTable, CityTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    db.GetTable<CityTable>().Delete().Execute();

    // users
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Eve"));

    // departments (only Alice and Bob)
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("Bob"));

    // cities (only Alice)
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Alice"),
                                    CityTable::MakeTableColumn<CityColumn>("Paris"));

    using UName = UserTable::TableColumn<NameColumn>;
    using DName = DeptTable::TableColumn<NameColumn>;
    using Dept = DeptTable::TableColumn<DeptColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using City = CityTable::TableColumn<CityColumn>;

    auto results = db.GetTable<UserTable>()
            .InnerJoin<DeptTable>(Equal<UName, DName>())
            .LeftJoin<CityTable>(Equal<UName, CName>())
            .Select<UName, Dept, City>()
            .Results();

    // Only Alice and Bob (inner with Dept), with city only for Alice
    ASSERT_EQ(results.size(), 2);
    for (auto &row: results) {
        if (std::get<UName>(row).value == "Alice") {
            EXPECT_EQ(std::get<Dept>(row).value, std::string("HR"));
            EXPECT_EQ(std::get<City>(row).value, std::string("Paris"));
        }
        if (std::get<UName>(row).value == "Bob") {
            EXPECT_EQ(std::get<Dept>(row).value, std::string("IT"));
            EXPECT_TRUE(std::get<City>(row).value.empty());
        }
    }
}

// ============ Column Constraints 測試 ============

// 測試 PRIMARY KEY 約束
TEST_F(SQLiteHelperTest, ConstraintPrimaryKey) {
    using IdColumn = Column<"id", column_type::INTEGER, ColumnPrimaryKey<>>;
    using TestTable = Table<"test_pk", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice")
    );

    // 嘗試插入重複的 PRIMARY KEY，應該失敗
    EXPECT_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1),
            TestTable::MakeTableColumn<NameColumn>("Bob")
        ),
        std::runtime_error
    );

    // 驗證只有一筆資料
    auto results = db.GetTable<TestTable>().Select<TestTable::TableColumn<IdColumn>>().Results();
    EXPECT_EQ(results.size(), 1);
}

// 測試 PRIMARY KEY 與 DESC 排序
TEST_F(SQLiteHelperTest, ConstraintPrimaryKeyDesc) {
    using IdColumn = Column<"id", column_type::INTEGER, ColumnPrimaryKey<OrderType::DESC>>;
    using TestTable = Table<"test_pk_desc", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("First")
    );
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(2),
        TestTable::MakeTableColumn<NameColumn>("Second")
    );

    auto results = db.GetTable<TestTable>().Select<TestTable::TableColumn<IdColumn>>().Results();
    EXPECT_EQ(results.size(), 2);
}

// 測試 NOT NULL 約束
TEST_F(SQLiteHelperTest, ConstraintNotNull) {
    using EmailColumn = Column<"email", column_type::TEXT, ColumnNotNull<>>;
    using TestTable = Table<"test_not_null", NameColumn, EmailColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入有效資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@example.com")
    );

    auto results = db.GetTable<TestTable>().Select<TestTable::TableColumn<EmailColumn>>().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<EmailColumn>>(results[0]).value, "alice@example.com");
}

// 測試 UNIQUE 約束
TEST_F(SQLiteHelperTest, ConstraintUnique) {
    using UsernameColumn = Column<"username", column_type::TEXT, ColumnUnique<>>;
    using TestTable = Table<"test_unique", UsernameColumn, AgeColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入第一筆資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<UsernameColumn>("alice123"),
        TestTable::MakeTableColumn<AgeColumn>(25)
    );

    // 嘗試插入重複的 UNIQUE 值，應該失敗
    EXPECT_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<UsernameColumn>("alice123"),
            TestTable::MakeTableColumn<AgeColumn>(30)
        ),
        std::runtime_error
    );

    // 插入不同的值應該成功
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<UsernameColumn>("bob456"),
        TestTable::MakeTableColumn<AgeColumn>(30)
    );

    auto results = db.GetTable<TestTable>().Select<TestTable::TableColumn<UsernameColumn>>().Results();
    EXPECT_EQ(results.size(), 2);
}

// 測試 DEFAULT 約束（整數）
TEST_F(SQLiteHelperTest, ConstraintDefaultInteger) {
    using StatusColumn = Column<"status", column_type::INTEGER, Default<0>>;
    using TestTable = Table<"test_default_int", NameColumn, StatusColumn>;
    Database<TestTable> db("test_database.db", true);

    // 只插入 name，status 應該使用預設值
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<StatusColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
    EXPECT_EQ(std::get<TestTable::TableColumn<StatusColumn>>(results[0]).value, 0);
}

// 測試 DEFAULT 約束（字串）
TEST_F(SQLiteHelperTest, ConstraintDefaultString) {
    using CountryColumn = Column<"country", column_type::TEXT, Default<"Taiwan">>;
    using TestTable = Table<"test_default_str", NameColumn, CountryColumn>;
    Database<TestTable> db("test_database.db", true);

    // 只插入 name
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<CountryColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
    EXPECT_EQ(std::get<TestTable::TableColumn<CountryColumn>>(results[0]).value, "Taiwan");
}

// 測試組合約束：PRIMARY KEY + NOT NULL
TEST_F(SQLiteHelperTest, ConstraintCombinationPrimaryKeyNotNull) {
    using IdColumn = Column<"id", column_type::INTEGER, ColumnPrimaryKey<>, ColumnNotNull<>>;
    using TestTable = Table<"test_pk_nn", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice")
    );

    // 嘗試插入重複 ID 應該失敗
    EXPECT_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<IdColumn>(1),
            TestTable::MakeTableColumn<NameColumn>("Bob")
        ),
        std::runtime_error
    );

    auto results = db.GetTable<TestTable>().Select<TestTable::TableColumn<IdColumn>>().Results();
    EXPECT_EQ(results.size(), 1);
}

// 測試組合約束：UNIQUE + NOT NULL
TEST_F(SQLiteHelperTest, ConstraintCombinationUniqueNotNull) {
    using EmailColumn = Column<"email", column_type::TEXT, ColumnUnique<>, ColumnNotNull<>>;
    using TestTable = Table<"test_uniq_nn", NameColumn, EmailColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@test.com")
    );

    // 嘗試插入重複的 email
    EXPECT_THROW(
        db.GetTable<TestTable>().Insert(
            TestTable::MakeTableColumn<NameColumn>("Bob"),
            TestTable::MakeTableColumn<EmailColumn>("alice@test.com")
        ),
        std::runtime_error
    );

    // 插入不同的 email 應該成功
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<EmailColumn>("bob@test.com")
    );

    auto results = db.GetTable<TestTable>().Select<TestTable::TableColumn<EmailColumn>>().Results();
    EXPECT_EQ(results.size(), 2);
}

// 測試 PRIMARY KEY 與 ON CONFLICT REPLACE
TEST_F(SQLiteHelperTest, ConstraintPrimaryKeyConflictReplace) {
    using IdColumn = Column<"id", column_type::INTEGER, ColumnPrimaryKey<OrderType::ASC, ConflictCause::REPLACE>>;
    using TestTable = Table<"test_pk_replace", IdColumn, NameColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice")
    );

    // 插入重複 ID，應該替換舊資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Bob")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<NameColumn>>(results[0]).value, "Bob");
}

// 測試 UNIQUE 與 ON CONFLICT IGNORE
TEST_F(SQLiteHelperTest, ConstraintUniqueConflictIgnore) {
    using EmailColumn = Column<"email", column_type::TEXT, ColumnUnique<ConflictCause::IGNORE>>;
    using TestTable = Table<"test_uniq_ignore", NameColumn, EmailColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("same@test.com")
    );

    // 插入重複值，應該被忽略（不拋出異常）
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<EmailColumn>("same@test.com")
    );

    auto results = db.GetTable<TestTable>().Select<TestTable::TableColumn<NameColumn>>().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

// 測試 NOT NULL 與 ON CONFLICT FAIL
TEST_F(SQLiteHelperTest, ConstraintNotNullConflictFail) {
    using RequiredColumn = Column<"required_field", column_type::TEXT, ColumnNotNull<ConflictCause::FAIL>>;
    using TestTable = Table<"test_nn_fail", NameColumn, RequiredColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入有效資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<RequiredColumn>("value")
    );

    auto results = db.GetTable<TestTable>().Select<TestTable::TableColumn<RequiredColumn>>().Results();
    EXPECT_EQ(results.size(), 1);
}

// 測試複雜的組合約束
TEST_F(SQLiteHelperTest, ConstraintComplexCombination) {
    using IdColumn = Column<"id", column_type::INTEGER, ColumnPrimaryKey<>>;
    using EmailColumn = Column<"email", column_type::TEXT, ColumnUnique<>, ColumnNotNull<>>;
    using StatusColumn = Column<"status", column_type::INTEGER, Default<1>>;
    using TestTable = Table<"test_complex", IdColumn, NameColumn, EmailColumn, StatusColumn>;
    Database<TestTable> db("test_database.db", true);

    // 插入完整資料
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(1),
        TestTable::MakeTableColumn<NameColumn>("Alice"),
        TestTable::MakeTableColumn<EmailColumn>("alice@test.com"),
        TestTable::MakeTableColumn<StatusColumn>(5)
    );

    // 插入部分資料，status 使用預設值
    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<IdColumn>(2),
        TestTable::MakeTableColumn<NameColumn>("Bob"),
        TestTable::MakeTableColumn<EmailColumn>("bob@test.com")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<IdColumn>,
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<EmailColumn>,
        TestTable::TableColumn<StatusColumn>
    >().Results();

    EXPECT_EQ(results.size(), 2);

    // 驗證第一筆
    EXPECT_EQ(std::get<TestTable::TableColumn<IdColumn>>(results[0]).value, 1);
    EXPECT_EQ(std::get<TestTable::TableColumn<StatusColumn>>(results[0]).value, 5);

    // 驗證第二筆的預設值
    EXPECT_EQ(std::get<TestTable::TableColumn<IdColumn>>(results[1]).value, 2);
    EXPECT_EQ(std::get<TestTable::TableColumn<StatusColumn>>(results[1]).value, 1);
}

// 測試實數型的 DEFAULT 約束
TEST_F(SQLiteHelperTest, ConstraintDefaultReal) {
    using RatingColumn = Column<"rating", column_type::REAL, Default<5.0>>;
    using TestTable = Table<"test_default_real", NameColumn, RatingColumn>;
    Database<TestTable> db("test_database.db", true);

    db.GetTable<TestTable>().Insert(
        TestTable::MakeTableColumn<NameColumn>("Product1")
    );

    auto results = db.GetTable<TestTable>().Select<
        TestTable::TableColumn<NameColumn>,
        TestTable::TableColumn<RatingColumn>
    >().Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_DOUBLE_EQ(std::get<TestTable::TableColumn<RatingColumn>>(results[0]).value, 5.0);
}

// ============ Table Constraint 測試 ============

// 測試 PrimaryKeyX (單欄位主鍵)
TEST_F(SQLiteHelperTest, TableConstraintPrimaryKeySingle) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_pk_single", IdColumn, NameColumn,
                           TablePrimaryKey<TypeGroup<IdColumn>>>;
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

// 測試 PrimaryKeyX (複合主鍵)
TEST_F(SQLiteHelperTest, TableConstraintPrimaryKeyComposite) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_pk_composite", IdColumn, NameColumn, AgeColumn,
                           TablePrimaryKey<TypeGroup<IdColumn, NameColumn>>>;
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

// 測試 UniqueX (單欄位唯一約束)
TEST_F(SQLiteHelperTest, TableConstraintUniqueSingle) {
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_unique_single", NameColumn, EmailColumn,
                           TableUnique<TypeGroup<EmailColumn>>>;
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

// 測試 UniqueX (複合唯一約束)
TEST_F(SQLiteHelperTest, TableConstraintUniqueComposite) {
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_unique_composite", NameColumn, EmailColumn,
                           TableUnique<TypeGroup<NameColumn, EmailColumn>>>;
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
TEST_F(SQLiteHelperTest, TableConstraintMixed) {
    using IdColumn = Column<"id", column_type::INTEGER, ColumnPrimaryKey<>>;
    using EmailColumn = Column<"email", column_type::TEXT, ColumnNotNull<>>;
    using TestTable = Table<"test_mixed_constraints", IdColumn, NameColumn, EmailColumn,
                           TableUnique<TypeGroup<EmailColumn>>>;
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
TEST_F(SQLiteHelperTest, TableConstraintPrimaryKeyDesc) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_pk_desc_table", IdColumn, NameColumn,
                           TablePrimaryKey<TypeGroup<ColumnWithOrder<IdColumn, OrderType::DESC>>>>;
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
TEST_F(SQLiteHelperTest, TableConstraintUniqueWithConflict) {
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_unique_conflict", NameColumn, EmailColumn,
                           TableUnique<TypeGroup<EmailColumn>, ConflictCause::IGNORE>>;
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
TEST_F(SQLiteHelperTest, TableConstraintPrimaryKeyMixedOrder) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using TestTable = Table<"test_pk_mixed_order", IdColumn, NameColumn, AgeColumn,
                           TablePrimaryKey<TypeGroup<
                               ColumnWithOrder<IdColumn, OrderType::ASC>,
                               ColumnWithOrder<NameColumn, OrderType::DESC>
                           >>>;
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
TEST_F(SQLiteHelperTest, TableConstraintUniqueMixedOrder) {
    using EmailColumn = Column<"email", column_type::TEXT>;
    using PhoneColumn = Column<"phone", column_type::TEXT>;
    using TestTable = Table<"test_unique_mixed_order", NameColumn, EmailColumn, PhoneColumn,
                           TableUnique<TypeGroup<
                               ColumnWithOrder<EmailColumn, OrderType::DESC>,
                               ColumnWithOrder<PhoneColumn, OrderType::ASC>
                           >>>;
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
TEST_F(SQLiteHelperTest, TableConstraintMixedOrderedUnordered) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using EmailColumn = Column<"email", column_type::TEXT>;
    using TestTable = Table<"test_mixed_ordered_unordered", IdColumn, NameColumn, EmailColumn,
                           TablePrimaryKey<TypeGroup<
                               ColumnWithOrder<IdColumn, OrderType::DESC>,
                               NameColumn  // 未指定排序，使用預設
                           >>>;
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
TEST_F(SQLiteHelperTest, TableConstraintMultipleUnique) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using EmailColumn = Column<"email", column_type::TEXT>;
    using PhoneColumn = Column<"phone", column_type::TEXT>;
    using UsernameColumn = Column<"username", column_type::TEXT>;

    // 一個表可以有多個 UNIQUE 約束
    using TestTable = Table<"test_multiple_unique",
                           IdColumn, UsernameColumn, EmailColumn, PhoneColumn,
                           TableUnique<TypeGroup<EmailColumn>>,
                           TableUnique<TypeGroup<PhoneColumn>>,
                           TableUnique<TypeGroup<UsernameColumn>>>;
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
TEST_F(SQLiteHelperTest, TableConstraintMultipleCompositeUnique) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using FirstNameColumn = Column<"first_name", column_type::TEXT>;
    using LastNameColumn = Column<"last_name", column_type::TEXT>;
    using EmailColumn = Column<"email", column_type::TEXT>;

    // 多個複合 UNIQUE 約束
    using TestTable = Table<"test_multiple_composite_unique",
                           IdColumn, FirstNameColumn, LastNameColumn, EmailColumn,
                           TableUnique<TypeGroup<FirstNameColumn, LastNameColumn>>,  // 名字組合唯一
                           TableUnique<TypeGroup<EmailColumn>>>;  // email 也要唯一
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
TEST_F(SQLiteHelperTest, TableConstraintPrimaryKeyWithMultipleUnique) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using EmailColumn = Column<"email", column_type::TEXT>;
    using PhoneColumn = Column<"phone", column_type::TEXT>;
    using SsnColumn = Column<"ssn", column_type::TEXT>;  // Social Security Number

    using TestTable = Table<"test_pk_multi_unique",
                           IdColumn, NameColumn, EmailColumn, PhoneColumn, SsnColumn,
                           TablePrimaryKey<TypeGroup<IdColumn>>,
                           TableUnique<TypeGroup<EmailColumn>>,
                           TableUnique<TypeGroup<PhoneColumn>>,
                           TableUnique<TypeGroup<SsnColumn>>>;
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
            TestTable::MakeTableColumn<IdColumn>(1),  // 重複的 id
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
            TestTable::MakeTableColumn<EmailColumn>("alice@example.com"),  // 重複的 email
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
            TestTable::MakeTableColumn<PhoneColumn>("123-4567"),  // 重複的 phone
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
            TestTable::MakeTableColumn<SsnColumn>("111-11-1111")  // 重複的 ssn
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
TEST_F(SQLiteHelperTest, TableConstraintMixedSingleCompositeUnique) {
    using IdColumn = Column<"id", column_type::INTEGER>;
    using CountryColumn = Column<"country", column_type::TEXT>;
    using CityColumn = Column<"city", column_type::TEXT>;
    using EmailColumn = Column<"email", column_type::TEXT>;

    using TestTable = Table<"test_mixed_unique",
                           IdColumn, NameColumn, CountryColumn, CityColumn, EmailColumn,
                           TablePrimaryKey<TypeGroup<IdColumn>>,
                           TableUnique<TypeGroup<EmailColumn>>,  // 單一欄位
                           TableUnique<TypeGroup<CountryColumn, CityColumn, NameColumn>>>;  // 複合欄位
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

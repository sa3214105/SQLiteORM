#include <iostream>
#include <gtest/gtest.h>
#include "../SQLiteHelper.hpp"

using namespace SQLiteHelper;

// 定義測試用的列結構
struct NameColumn {
    constexpr static FixedString name = "name";
    constexpr static column_type type = column_type::TEXT;
    inline static column_constraint constraint = {};
    std::string value;
};

struct AgeColumn {
    constexpr static FixedString name = "age";
    constexpr static column_type type = column_type::INTEGER;
    inline static column_constraint constraint = {};
    int value;
};

struct ScoreColumn {
    constexpr static FixedString name = "score";
    constexpr static column_type type = column_type::REAL;
    inline static column_constraint constraint = {};
    double value;
};

struct DeptColumn {
    constexpr static FixedString name = "dept";
    constexpr static column_type type = column_type::TEXT;
    inline static column_constraint constraint = {};
    std::string value;
};

struct CityColumn {
    constexpr static FixedString name = "city";
    constexpr static column_type type = column_type::TEXT;
    inline static column_constraint constraint = {};
    std::string value;
};

struct CountryColumn {
    constexpr static FixedString name = "country";
    constexpr static column_type type = column_type::TEXT;
    inline static column_constraint constraint = {};
    std::string value;
};

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <iostream>
#include <gtest/gtest.h>
#include "../SQLiteHelper.hpp"

// 定義測試用的列結構
struct NameColumn {
    constexpr static SQLiteHelper::FixedString name = "name";
    constexpr static SQLiteHelper::column_type type = SQLiteHelper::column_type::TEXT;
    inline static SQLiteHelper::column_constraint constraint = {};
    std::string value;
};

struct AgeColumn {
    constexpr static SQLiteHelper::FixedString name = "age";
    constexpr static SQLiteHelper::column_type type = SQLiteHelper::column_type::INTEGER;
    inline static SQLiteHelper::column_constraint constraint = {};
    int value;
};

struct ScoreColumn {
    constexpr static SQLiteHelper::FixedString name = "score";
    constexpr static SQLiteHelper::column_type type = SQLiteHelper::column_type::REAL;
    inline static SQLiteHelper::column_constraint constraint = {};
    double value;
};

struct DeptColumn {
    constexpr static SQLiteHelper::FixedString name = "dept";
    constexpr static SQLiteHelper::column_type type = SQLiteHelper::column_type::TEXT;
    inline static SQLiteHelper::column_constraint constraint = {};
    std::string value;
};

// 定義表類型
using UserTable = SQLiteHelper::Table<"users", NameColumn, AgeColumn, ScoreColumn>;
using DeptTable = SQLiteHelper::Table<"departments", DeptColumn, NameColumn>;

class SQLiteHelperTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 測試開始前清理數據庫
        SQLiteHelper::Database<UserTable> db("test_database.db");
        db.GetTable<UserTable>().Delete().Execute();
    }

    void TearDown() override {
        // 測試結束後清理
    }
};

// ============ Insert 測試 ============
TEST_F(SQLiteHelperTest, InsertSingleColumn) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"});
    auto results = db.GetTable<UserTable>().Select<NameColumn>().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<NameColumn>(results[0]).value, "Alice");
}

TEST_F(SQLiteHelperTest, InsertMultipleColumns) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(
        NameColumn{.value = "Bob"},
        AgeColumn{.value = 30},
        ScoreColumn{.value = 95.5}
    );
    auto results = db.GetTable<UserTable>().Select<NameColumn, AgeColumn, ScoreColumn>().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<NameColumn>(results[0]).value, "Bob");
    EXPECT_EQ(std::get<AgeColumn>(results[0]).value, 30);
    EXPECT_EQ(std::get<ScoreColumn>(results[0]).value, 95.5);
}

TEST_F(SQLiteHelperTest, InsertMultipleRows) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Charlie"});

    auto results = db.GetTable<UserTable>().Select<NameColumn>().Results();
    EXPECT_EQ(results.size(), 3);
}

// ============ Select 測試 ============
TEST_F(SQLiteHelperTest, SelectAllRows) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "User1"}, AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "User2"}, AgeColumn{.value = 25});

    auto results = db.GetTable<UserTable>().Select<NameColumn, AgeColumn>().Results();
    EXPECT_EQ(results.size(), 2);
}

TEST_F(SQLiteHelperTest, SelectSingleColumn) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Test"}, AgeColumn{.value = 25});

    auto results = db.GetTable<UserTable>().Select<NameColumn>().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<NameColumn>(results[0]).value, "Test");
}

// ============ Where 條件測試 ============
TEST_F(SQLiteHelperTest, WhereEqual) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"});

    auto results = db.GetTable<UserTable>().Select<NameColumn>()
            .Where(SQLiteHelper::Equal<NameColumn>("Alice")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<NameColumn>(results[0]).value, "Alice");
}

TEST_F(SQLiteHelperTest, WhereNotEqual) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Charlie"});

    auto results = db.GetTable<UserTable>().Select<NameColumn>()
            .Where(SQLiteHelper::NotEqual<NameColumn>("Alice")).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST_F(SQLiteHelperTest, WhereGreaterThan) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 25});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 30});

    auto results = db.GetTable<UserTable>().Select<AgeColumn>()
            .Where(SQLiteHelper::GreaterThan<AgeColumn>("25")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<AgeColumn>(results[0]).value, 30);
}

TEST_F(SQLiteHelperTest, WhereLessThan) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 25});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 30});

    auto results = db.GetTable<UserTable>().Select<AgeColumn>()
            .Where(SQLiteHelper::LessThan<AgeColumn>("25")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<AgeColumn>(results[0]).value, 20);
}

TEST_F(SQLiteHelperTest, WhereGreaterThanEqual) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 25});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 30});

    auto results = db.GetTable<UserTable>().Select<AgeColumn>()
            .Where(SQLiteHelper::GreaterThanEqual<AgeColumn>("25")).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST_F(SQLiteHelperTest, WhereLessThanEqual) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 25});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 30});

    auto results = db.GetTable<UserTable>().Select<AgeColumn>()
            .Where(SQLiteHelper::LessThanEqual<AgeColumn>("25")).Results();
    EXPECT_EQ(results.size(), 2);
}

// ============ 邏輯運算符測試 (AND / OR) ============
TEST_F(SQLiteHelperTest, WhereAND) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 25});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 30});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 35});

    auto cond = SQLiteHelper::Equal<NameColumn>("Alice") &&
                SQLiteHelper::GreaterThan<AgeColumn>("25");
    auto results = db.GetTable<UserTable>().Select<NameColumn, AgeColumn>()
            .Where(cond).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<AgeColumn>(results[0]).value, 35);
}

TEST_F(SQLiteHelperTest, WhereOR) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 25});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 30});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Charlie"}, AgeColumn{.value = 35});

    auto cond = SQLiteHelper::Equal<NameColumn>("Alice") ||
                SQLiteHelper::Equal<NameColumn>("Bob");
    auto results = db.GetTable<UserTable>().Select<NameColumn>()
            .Where(cond).Results();
    EXPECT_EQ(results.size(), 2);
}

// ============ Update 測試 ============
TEST_F(SQLiteHelperTest, UpdateSingleColumn) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "OldName"});

    db.GetTable<UserTable>().Update(NameColumn{.value = "NewName"})
            .Where(SQLiteHelper::Equal<NameColumn>("OldName")).Execute();

    auto results = db.GetTable<UserTable>().Select<NameColumn>().Results();
    EXPECT_EQ(std::get<NameColumn>(results[0]).value, "NewName");
}

TEST_F(SQLiteHelperTest, UpdateMultipleColumns) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(
        NameColumn{.value = "Alice"},
        AgeColumn{.value = 20}
    );

    db.GetTable<UserTable>().Update(
        NameColumn{.value = "Bob"},
        AgeColumn{.value = 30}
    ).Where(SQLiteHelper::Equal<NameColumn>("Alice")).Execute();

    auto results = db.GetTable<UserTable>().Select<NameColumn, AgeColumn>().Results();
    EXPECT_EQ(std::get<NameColumn>(results[0]).value, "Bob");
    EXPECT_EQ(std::get<AgeColumn>(results[0]).value, 30);
}

TEST_F(SQLiteHelperTest, UpdateMultipleRows) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 25});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 25});

    db.GetTable<UserTable>().Update(AgeColumn{.value = 30})
            .Where(SQLiteHelper::Equal<AgeColumn>("25")).Execute();

    auto results = db.GetTable<UserTable>().Select<AgeColumn>().Results();
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<AgeColumn>(results[0]).value, 30);
    EXPECT_EQ(std::get<AgeColumn>(results[1]).value, 30);
}

// ============ Delete 測試 ============
TEST_F(SQLiteHelperTest, DeleteSingleRow) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"});

    db.GetTable<UserTable>().Delete()
            .Where(SQLiteHelper::Equal<NameColumn>("Alice")).Execute();

    auto results = db.GetTable<UserTable>().Select<NameColumn>().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<NameColumn>(results[0]).value, "Bob");
}

TEST_F(SQLiteHelperTest, DeleteMultipleRows) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(AgeColumn{.value = 30});

    db.GetTable<UserTable>().Delete()
            .Where(SQLiteHelper::Equal<AgeColumn>("20")).Execute();

    auto results = db.GetTable<UserTable>().Select<AgeColumn>().Results();
    EXPECT_EQ(results.size(), 1);
}

TEST_F(SQLiteHelperTest, DeleteAllRows) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"});

    db.GetTable<UserTable>().Delete().Execute();

    auto results = db.GetTable<UserTable>().Select<NameColumn>().Results();
    EXPECT_EQ(results.size(), 0);
}

// ============ 複合操作測試 ============
TEST_F(SQLiteHelperTest, ComplexOperations) {
    SQLiteHelper::Database<UserTable> db("test_database.db");

    // 插入數據
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 25}, ScoreColumn{.value = 90.0});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 30}, ScoreColumn{.value = 85.0});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Charlie"}, AgeColumn{.value = 28}, ScoreColumn{.value = 95.0});

    // 查詢年齡 > 25 的用戶
    auto results1 = db.GetTable<UserTable>().Select<NameColumn, AgeColumn>()
            .Where(SQLiteHelper::GreaterThan<AgeColumn>("25")).Results();
    EXPECT_EQ(results1.size(), 2);

    // 更新 Bob 的分數
    db.GetTable<UserTable>().Update(ScoreColumn{.value = 92.0})
            .Where(SQLiteHelper::Equal<NameColumn>("Bob")).Execute();

    // 查詢 Bob 的新分數
    auto results2 = db.GetTable<UserTable>().Select<ScoreColumn>()
            .Where(SQLiteHelper::Equal<NameColumn>("Bob")).Results();
    EXPECT_EQ(std::get<ScoreColumn>(results2[0]).value, 92.0);

    // 刪除年齡 >= 30 的用戶
    db.GetTable<UserTable>().Delete()
            .Where(SQLiteHelper::GreaterThanEqual<AgeColumn>("30")).Execute();

    auto results3 = db.GetTable<UserTable>().Select<NameColumn>().Results();
    EXPECT_EQ(results3.size(), 2);
}

// ============ Transaction 測試 ============
TEST_F(SQLiteHelperTest, TransactionCommit) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transation) {
        db.GetTable<UserTable>().Insert(NameColumn{.value = "TxUser"}, AgeColumn{.value = 99});
    });
    auto results = db.GetTable<UserTable>().Select<NameColumn, AgeColumn>().Where(
        SQLiteHelper::Equal<NameColumn>("TxUser")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<AgeColumn>(results[0]).value, 99);
}

TEST_F(SQLiteHelperTest, TransactionRollback) {
    SQLiteHelper::Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transation ) {
        db.GetTable<UserTable>().Insert(NameColumn{.value = "TxRollback"}, AgeColumn{.value = 88});
        throw std::runtime_error("Force rollback");
    });
    auto results = db.GetTable<UserTable>().Select<NameColumn, AgeColumn>().Where(
        SQLiteHelper::Equal<NameColumn>("TxRollback")).Results();
    EXPECT_EQ(results.size(), 0);
}

// ============ Join 測試 ============
TEST_F(SQLiteHelperTest, InnerJoinBasic) {
    SQLiteHelper::Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 30});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Charlie"}, AgeColumn{.value = 40});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "David"}, AgeColumn{.value = 50});
    // DeptTable: Alice, David (只有交集部分)
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "HR"}, NameColumn{.value = "Alice"});
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "IT"}, NameColumn{.value = "David"});

    // InnerJoin on NameColumn - 只返回交集 (Alice, David)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().InnerJoin<DeptTable, SQLiteHelper::EqualCond<_NameColumn, __NameColumn> >().
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
    SQLiteHelper::Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 30});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Charlie"}, AgeColumn{.value = 40});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "David"}, AgeColumn{.value = 50});
    // DeptTable: Alice, David, Eve
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "HR"}, NameColumn{.value = "Alice"});
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "IT"}, NameColumn{.value = "David"});
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "Sales"}, NameColumn{.value = "Eve"});

    // LeftJoin - 返回左表的所有行 (Alice, Bob, Charlie, David)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().LeftJoin<DeptTable, SQLiteHelper::EqualCond<_NameColumn, __NameColumn> >().
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
    SQLiteHelper::Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 30});
    // DeptTable: Alice, David, Eve (比左表多)
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "HR"}, NameColumn{.value = "Alice"});
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "IT"}, NameColumn{.value = "David"});
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "Sales"}, NameColumn{.value = "Eve"});

    // RightJoin - 返回右表的所有行 (Alice, David, Eve)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().RightJoin<DeptTable, SQLiteHelper::EqualCond<_NameColumn, __NameColumn> >().
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // RIGHT JOIN 應返回右表的所有行 - 至少應該包含右表中的所有行
    EXPECT_GE(results.size(), 3);
}

TEST_F(SQLiteHelperTest, CrossJoinBasic) {
    SQLiteHelper::Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 30});
    // DeptTable: HR, IT
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "HR"}, NameColumn{.value = "X"});
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "IT"}, NameColumn{.value = "Y"});

    // CrossJoin - 笛卡爾乘積 (2 x 2 = 4)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;

    auto results = db.GetTable<UserTable>().CrossJoin<DeptTable, SQLiteHelper::EmptyCond>().
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // CROSS JOIN 應返回笛卡爾乘積的結果 (2 x 2 = 4)
    EXPECT_EQ(results.size(), 4);
}

TEST_F(SQLiteHelperTest, FullJoinBasic) {
    SQLiteHelper::Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Alice"}, AgeColumn{.value = 20});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Bob"}, AgeColumn{.value = 30});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "Charlie"}, AgeColumn{.value = 40});
    db.GetTable<UserTable>().Insert(NameColumn{.value = "David"}, AgeColumn{.value = 50});
    // DeptTable: Alice, David
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "HR"}, NameColumn{.value = "Alice"});
    db.GetTable<DeptTable>().Insert(DeptColumn{.value = "IT"}, NameColumn{.value = "David"});
    // FullJoin on NameColumn
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;
    auto results = db.GetTable<UserTable>().FullJoin<DeptTable, SQLiteHelper::EqualCond<_NameColumn, __NameColumn> >().
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

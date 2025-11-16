#pragma once
#include "Common.hpp"

// ============ Select 測試 ============
TEST(SelectTest, SelectAllRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
        AgeColumn> >("User1", 20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
        AgeColumn> >("User2", 25);

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(),
                                                   UserTable::TableColumn<AgeColumn>()).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST(SelectTest, SelectSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn> >("Test", 25);

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
}

// ============ Where 條件測試 ============
TEST(SelectTest, WhereEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn> >("Alice");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn> >("Bob");

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>())
            .Where(UserTable::TableColumn<NameColumn>() == "Alice"_expr).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

TEST(SelectTest, WhereNotEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn> >("Alice");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn> >("Bob");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn> >("Charlie");

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>())
            .Where(UserTable::TableColumn<NameColumn>() != "Alice"_expr).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST(SelectTest, WhereGreaterThan) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(25);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(30);

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<AgeColumn>())
            .Where(UserTable::TableColumn<AgeColumn>() > 25_expr).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 30);
}

TEST(SelectTest, WhereLessThan) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(25);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(30);

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<AgeColumn>())
            .Where(UserTable::TableColumn<AgeColumn>() < 25_expr).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 20);
}

TEST(SelectTest, WhereGreaterThanEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(25);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(30);

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<AgeColumn>())
            .Where(UserTable::TableColumn<AgeColumn>() >= 25_expr).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST(SelectTest, WhereLessThanEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(25);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn> >(30);

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<AgeColumn>())
            .Where(UserTable::TableColumn<AgeColumn>() <= 25_expr).Results();
    EXPECT_EQ(results.size(), 2);
}

// ============ 邏輯運算符測試 (AND / OR) ============
TEST(SelectTest, WhereAND) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn> >("Alice", 25);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn> >("Bob", 30);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn> >("Alice", 35);

    auto cond = UserTable::TableColumn<NameColumn>() == "Alice"_expr && UserTable::TableColumn<AgeColumn>() > 25_expr;
    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<
                                                       AgeColumn>())
            .Where(cond).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 35);
}

TEST(SelectTest, WhereOR) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn> >("Alice", 25);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn> >("Bob", 30);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn> >("Charlie", 35);

    auto cond = UserTable::TableColumn<NameColumn>() == "Alice"_expr || UserTable::TableColumn<NameColumn>() ==
                "Bob"_expr;
    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>())
            .Where(cond).Results();
    EXPECT_EQ(results.size(), 2);
}

// ============ 複合操作測試 ============
TEST(SelectTest, ComplexOperations) {
    Database<UserTable> db("test_database.db", true);

    // 插入數據
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn>,
        UserTable::TableColumn<ScoreColumn> >("Alice", 25, 90.0);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn>,
        UserTable::TableColumn<ScoreColumn> >("Bob", 30, 85.0);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn>,
        UserTable::TableColumn<ScoreColumn> >("Charlie", 28, 95.0);

    // 查詢年齡 > 25 的用戶
    auto results1 = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<
                                                        AgeColumn>())
            .Where(UserTable::TableColumn<AgeColumn>() > 25_expr).Results();
    EXPECT_EQ(results1.size(), 2);

    // 更新 Bob 的分數
    db.GetTable<UserTable>().Update<UserTable::TableColumn<ScoreColumn>>(92.0)
            .Where(UserTable::TableColumn<NameColumn>() == "Bob"_expr).Execute();

    // 查詢 Bob 的新分數
    auto results2 = db.GetTable<UserTable>().Select(UserTable::TableColumn<ScoreColumn>())
            .Where(UserTable::TableColumn<NameColumn>() == "Bob"_expr).Results();
    EXPECT_EQ(std::get<0>(results2[0]), 92.0);

    // 刪除年齡 >= 30 的用戶
    db.GetTable<UserTable>().Delete()
            .Where(UserTable::TableColumn<AgeColumn>() >= 30_expr).Execute();

    auto results3 = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>()).Results();
    EXPECT_EQ(results3.size(), 2);
}

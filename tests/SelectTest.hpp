#pragma once
#include "Common.hpp"

// ============ Select 測試 ============
TEST(SelectTest, SelectAllRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("User1"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("User2"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
        AgeColumn> >().Results();
    EXPECT_EQ(results.size(), 2);
}

TEST(SelectTest, SelectSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Test"),
                                    UserTable::MakeTableColumn<AgeColumn>(25));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
}

// ============ Where 條件測試 ============
TEST(SelectTest, WhereEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("Alice")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

TEST(SelectTest, WhereNotEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >()
            .Where(NotEqual<UserTable::TableColumn<NameColumn> >("Alice")).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST(SelectTest, WhereGreaterThan) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >()
            .Where(GreaterThan<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 30);
}

TEST(SelectTest, WhereLessThan) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >()
            .Where(LessThan<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 20);
}

TEST(SelectTest, WhereGreaterThanEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >()
            .Where(GreaterThanEqual<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results.size(), 2);
}

TEST(SelectTest, WhereLessThanEqual) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(25));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >()
            .Where(LessThanEqual<UserTable::TableColumn<AgeColumn> >(25)).Results();
    EXPECT_EQ(results.size(), 2);
}

// ============ 邏輯運算符測試 (AND / OR) ============
TEST(SelectTest, WhereAND) {
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

TEST(SelectTest, WhereOR) {
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

// ============ 複合操作測試 ============
TEST(SelectTest, ComplexOperations) {
    Database<UserTable> db("test_database.db", true);

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
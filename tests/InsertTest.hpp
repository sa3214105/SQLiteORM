#pragma once
#include "Common.hpp"

// ============ Insert 測試 ============
TEST(InsertTest, InsertSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn> >("Alice");
    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

TEST(InsertTest, InsertMultipleColumns) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>,
        UserTable::TableColumn<ScoreColumn> >("Bob", 30, 95.5);
    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<AgeColumn>()
        , UserTable::TableColumn<ScoreColumn>() ).Results();
    EXPECT_EQ(results.size(), 1);
    auto [name,age,score] = results[0];
    EXPECT_EQ(name, "Bob");
    EXPECT_EQ(age, 30);
    EXPECT_EQ(score, 95.5);
}

TEST(InsertTest, InsertMultipleRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Alice");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Bob");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Charlie");

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>()).Results();
    EXPECT_EQ(results.size(), 3);
}

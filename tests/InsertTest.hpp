#pragma once
#include "Common.hpp"

// ============ Insert 測試 ============
TEST(InsertTest, InsertSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Alice");
}

TEST(InsertTest, InsertMultipleColumns) {
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

TEST(InsertTest, InsertMultipleRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 3);
}
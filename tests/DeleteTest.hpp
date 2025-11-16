#pragma once
#include "Common.hpp"
// ============ Delete 測試 ============
TEST(DeleteTest, DeleteSingleRow) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Alice");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Bob");

    db.GetTable<UserTable>().Delete()
            .Where(UserTable::TableColumn<NameColumn>() == "Alice"_expr).Execute();

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Bob");
}

TEST(DeleteTest, DeleteMultipleRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn>>(20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn>>(20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<AgeColumn>>(30);

    db.GetTable<UserTable>().Delete().Where(UserTable::TableColumn<AgeColumn>() == 20_expr).Execute();

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<AgeColumn>()).Results();
    EXPECT_EQ(results.size(), 1);
}

TEST(DeleteTest, DeleteAllRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Alice");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Bob");

    db.GetTable<UserTable>().Delete().Execute();

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>()).Results();
    EXPECT_EQ(results.size(), 0);
}
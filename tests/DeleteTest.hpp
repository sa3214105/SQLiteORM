#pragma once
#include "Common.hpp"
// ============ Delete 測試 ============
TEST(DeleteTest, DeleteSingleRow) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));

    db.GetTable<UserTable>().Delete()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("Alice")).Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "Bob");
}

TEST(DeleteTest, DeleteMultipleRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<AgeColumn>(30));

    db.GetTable<UserTable>().Delete()
            .Where(Equal<UserTable::TableColumn<AgeColumn> >(20)).Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<AgeColumn> >().Results();
    EXPECT_EQ(results.size(), 1);
}

TEST(DeleteTest, DeleteAllRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));

    db.GetTable<UserTable>().Delete().Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(results.size(), 0);
}
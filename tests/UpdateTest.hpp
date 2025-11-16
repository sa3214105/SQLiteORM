#pragma once
#include "Common.hpp"
// ============ Update 測試 ============
TEST(UpdateTest, UpdateSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn> >("OldName");

    db.GetTable<UserTable>().Update<UserTable::TableColumn<NameColumn> >("NewName")
            .Where(UserTable::TableColumn<NameColumn>() == "OldName"_expr).Execute();

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>()).Results();
    EXPECT_EQ(std::get<0>(results[0]), "NewName");
}

TEST(UpdateTest, UpdateMultipleColumns) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
        AgeColumn> >("Alice", 20);

    db.GetTable<UserTable>().Update<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn> >("Bob", 30).
            Where(UserTable::TableColumn<NameColumn>() == "Alice"_expr).Execute();

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<
                                                       AgeColumn>()).Results();
    EXPECT_EQ(std::get<0>(results[0]), "Bob");
    EXPECT_EQ(std::get<1>(results[0]), 30);
}

TEST(UpdateTest, UpdateMultipleRows) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn> >("Alice", 25);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
        UserTable::TableColumn<AgeColumn> >("Bob", 25);

    db.GetTable<UserTable>().Update<UserTable::TableColumn<AgeColumn>>(30)
            .Where(UserTable::TableColumn<AgeColumn>() == 25_expr).Execute();

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<AgeColumn>()).Results();
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<0>(results[0]), 30);
    EXPECT_EQ(std::get<0>(results[1]), 30);
}

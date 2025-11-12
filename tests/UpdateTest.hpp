#pragma once
#include "Common.hpp"
// ============ Update 測試 ============
TEST(UpdateTest, UpdateSingleColumn) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("OldName"));

    db.GetTable<UserTable>().Update(UserTable::MakeTableColumn<NameColumn>("NewName"))
            .Where(Equal<UserTable::TableColumn<NameColumn> >("OldName")).Execute();

    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn> >().Results();
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "NewName");
}

TEST(UpdateTest, UpdateMultipleColumns) {
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

TEST(UpdateTest, UpdateMultipleRows) {
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
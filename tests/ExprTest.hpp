#pragma once
#include "Common.hpp"

TEST(ExprTest, SimpleTest) {
    Database<UserTable> db("test_database.db", true);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("User1",20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("User2",25);

    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(),
                                                   UserTable::TableColumn<AgeColumn>() + 1_expr).Results();
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), 21);
    EXPECT_EQ(std::get<1>(results[1]), 26);
}

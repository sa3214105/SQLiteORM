#pragma once
#include "Common.hpp"

class UpdateTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 每個測試前會重新建立資料庫
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// ============ Update 測試 ============
TEST_F(UpdateTest, UpdateSingleColumn) {
    userTable.Insert<decltype(NameColumn)>("OldName");

    userTable.Update<decltype(NameColumn)>("NewName")
            .Where(userTable[NameColumn] == "OldName"_expr).Execute();

    auto results = userTable.Select(userTable[NameColumn]).Results();
    EXPECT_EQ(std::get<0>(results[0]), "NewName");
}

TEST_F(UpdateTest, UpdateMultipleColumns) {
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 20);

    userTable.Update<decltype(NameColumn), decltype(AgeColumn)>("Bob", 30)
            .Where(userTable[NameColumn] == "Alice"_expr).Execute();

    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn]).Results();
    EXPECT_EQ(std::get<0>(results[0]), "Bob");
    EXPECT_EQ(std::get<1>(results[0]), 30);
}

TEST_F(UpdateTest, UpdateMultipleRows) {
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 25);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 25);

    userTable.Update<decltype(AgeColumn)>(30)
            .Where(userTable[AgeColumn] == 25_expr).Execute();

    auto results = userTable.Select(userTable[AgeColumn]).Results();
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<0>(results[0]), 30);
    EXPECT_EQ(std::get<0>(results[1]), 30);
}

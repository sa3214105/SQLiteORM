#pragma once
#include "Common.hpp"

class DeleteTest : public ::testing::Test {
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

// ============ Delete 測試 ============
TEST_F(DeleteTest, DeleteSingleRow) {
    userTable.Insert<decltype(NameColumn)>("Alice");
    userTable.Insert<decltype(NameColumn)>("Bob");

    userTable.Delete()
            .Where(userTable[NameColumn] == "Alice"_expr).Execute();

    auto results = userTable.Select(userTable[NameColumn]).Results().ToVector();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Bob");
}

TEST_F(DeleteTest, DeleteMultipleRows) {
    userTable.Insert<decltype(AgeColumn)>(20);
    userTable.Insert<decltype(AgeColumn)>(20);
    userTable.Insert<decltype(AgeColumn)>(30);

    userTable.Delete().Where(userTable[AgeColumn] == 20_expr).Execute();

    auto results = userTable.Select(userTable[AgeColumn]).Results().ToVector();
    EXPECT_EQ(results.size(), 1);
}

TEST_F(DeleteTest, DeleteAllRows) {
    userTable.Insert<decltype(NameColumn)>("Alice");
    userTable.Insert<decltype(NameColumn)>("Bob");

    userTable.Delete().WhereAll().Execute();

    auto results = userTable.Select(userTable[NameColumn]).Results().ToVector();
    EXPECT_EQ(results.size(), 0);
}
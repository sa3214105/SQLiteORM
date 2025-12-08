#pragma once
#include "Common.hpp"

class InsertTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// ============ Insert 測試 ============
TEST_F(InsertTest, InsertSingleColumn) {
    userTable.Insert<decltype(NameColumn)>("Alice");
    auto results = userTable.Select(userTable[NameColumn]).Results().ToVector();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

TEST_F(InsertTest, InsertMultipleColumns) {
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 95.5);
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn], userTable[ScoreColumn]).Results().ToVector();
    EXPECT_EQ(results.size(), 1);
    auto [name, age, score] = results[0];
    EXPECT_EQ(name, "Bob");
    EXPECT_EQ(age, 30);
    EXPECT_EQ(score, 95.5);
}

TEST_F(InsertTest, InsertMultipleRows) {
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Charlie", 25);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Diana", 28);
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn]).Results().ToVector();
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<0>(results[0]), "Charlie");
    EXPECT_EQ(std::get<1>(results[0]), 25);
    EXPECT_EQ(std::get<0>(results[1]), "Diana");
    EXPECT_EQ(std::get<1>(results[1]), 28);
}

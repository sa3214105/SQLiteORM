#pragma once
#include "Common.hpp"

class SelectTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("User1", 20, 90);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("User2", 25, 85);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 95);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 30, 88);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 40, 70);
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// ============ Select 測試 ============
TEST_F(SelectTest, SelectAllRows) {
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn]).Results();
    EXPECT_EQ(results.size(), 5);
}

// ============ Where 條件測試 ============
TEST_F(SelectTest, WhereEqual) {
    auto results = userTable.Select(userTable[NameColumn]).Where(userTable[NameColumn] == "Alice"_expr).Results();
    for (auto &[name]: results) {
        ASSERT_EQ(name, "Alice");
    }
}

TEST_F(SelectTest, WhereNotEqual) {
    auto results = userTable.Select(userTable[NameColumn]).Where(userTable[NameColumn] != "Alice"_expr).Results();
    for (auto &[name]: results) {
        ASSERT_NE(name, "Alice");
    }
}

TEST_F(SelectTest, WhereGreaterThan) {
    auto results = userTable.Select(userTable[AgeColumn])
            .Where(userTable[AgeColumn] > 25_expr).Results();
    for (auto &[age]: results) {
        ASSERT_GT(age, 25);
    }
}

TEST_F(SelectTest, WhereLessThan) {
    auto results = userTable.Select(userTable[AgeColumn])
            .Where(userTable[AgeColumn] < 25_expr).Results();
    for (auto &[age]: results) {
        ASSERT_LT(age, 25);
    }
}

TEST_F(SelectTest, WhereGreaterThanEqual) {
    auto results = userTable.Select(userTable[AgeColumn])
            .Where(userTable[AgeColumn] >= 25_expr).Results();
    for (auto &[age]: results) {
        ASSERT_GE(age, 25);
    }
}

TEST_F(SelectTest, WhereLessThanEqual) {
    auto results = userTable.Select(userTable[AgeColumn])
            .Where(userTable[AgeColumn] <= 25_expr).Results();
    for (auto &[age]: results) {
        ASSERT_LE(age, 25);
    }
}

// ============ 邏輯運算符測試 (AND / OR) ============
TEST_F(SelectTest, WhereAND) {
    auto results = userTable.Select(userTable[AgeColumn], userTable[NameColumn])
            .Where((userTable[AgeColumn] >= 25_expr) && (userTable[NameColumn] == "Alice"_expr)).Results();
    for (auto &[age, name]: results) {
        ASSERT_GE(age, 25);
        ASSERT_EQ(name, "Alice");
    }
}

TEST_F(SelectTest, WhereOR) {
    auto results = userTable.Select(userTable[AgeColumn], userTable[NameColumn])
            .Where((userTable[AgeColumn] < 25_expr) || (userTable[NameColumn] == "Bob"_expr)).Results();
    for (auto &[age, name]: results) {
        if (age >= 25) {
            ASSERT_EQ(name, "Bob");
        } else {
            ASSERT_LT(age, 25);
        }
    }
}

// ============ 複合操作測試 ============
TEST_F(SelectTest, ComplexOperations) {
    // 查詢年齡 > 25 的用戶
    auto results1 = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .Where(userTable[AgeColumn] > 25_expr).Results();
    EXPECT_EQ(results1.size(), 2);

    // 更新 Bob 的分數
    userTable.Update<decltype(ScoreColumn)>(92.0).Where(userTable[NameColumn] == "Bob"_expr).Execute();

    // 查詢 Bob 的新分數
    auto results2 = userTable.Select(userTable[ScoreColumn]).Where(userTable[NameColumn] == "Bob"_expr).Results();
    EXPECT_EQ(std::get<0>(results2[0]), 92.0);

    // 刪除年齡 >= 30 的用戶
    userTable.Delete().Where(userTable[AgeColumn] >= 30_expr).Execute();
    auto results3 = userTable.Select(userTable[NameColumn]).Results();
    EXPECT_EQ(results3.size(), 3);
}

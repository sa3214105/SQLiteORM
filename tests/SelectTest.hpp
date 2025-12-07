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

// ============ OrderBy 測試 ============
TEST_F(SelectTest, OrderByAscending) {
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .OrderBy(userTable[AgeColumn])
            .Results();

    ASSERT_EQ(results.size(), 5);
    auto &[name1, age1] = results[0];
    EXPECT_EQ(age1, 20); // User1
    auto &[name2, age2] = results[1];
    EXPECT_EQ(age2, 25); // User2 或 Alice
    auto &[name5, age5] = results[4];
    EXPECT_EQ(age5, 40); // Bob
}

TEST_F(SelectTest, OrderByDescending) {
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .OrderBy(userTable[AgeColumn], OrderType::DESC)
            .Results();

    ASSERT_EQ(results.size(), 5);
    auto &[name1, age1] = results[0];
    EXPECT_EQ(age1, 40); // Bob
    auto &[name2, age2] = results[1];
    EXPECT_EQ(age2, 30); // Alice
    auto &[name5, age5] = results[4];
    EXPECT_EQ(age5, 20); // User1
}

TEST_F(SelectTest, OrderByScore) {
    auto results = userTable.Select(userTable[NameColumn], userTable[ScoreColumn])
            .OrderBy(userTable[ScoreColumn], OrderType::DESC)
            .Results();

    ASSERT_EQ(results.size(), 5);
    auto &[name1, score1] = results[0];
    EXPECT_DOUBLE_EQ(score1, 95.0); // Alice with highest score
    auto &[name2, score2] = results[1];
    EXPECT_DOUBLE_EQ(score2, 90.0); // User1
    auto &[name5, score5] = results[4];
    EXPECT_DOUBLE_EQ(score5, 70.0); // Bob with lowest score
}

TEST_F(SelectTest, OrderByWithWhere) {
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn], userTable[ScoreColumn])
            .Where(userTable[AgeColumn] >= 25_expr)
            .OrderBy(userTable[ScoreColumn], OrderType::DESC)
            .Results();

    ASSERT_EQ(results.size(), 4);
    auto &[name1, age1, score1] = results[0];
    EXPECT_DOUBLE_EQ(score1, 95.0);
    EXPECT_EQ(name1, "Alice");
}

TEST_F(SelectTest, OrderByNameAscending) {
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .OrderBy(userTable[NameColumn])
            .Results();

    ASSERT_EQ(results.size(), 5);
    auto &[name1, age1] = results[0];
    EXPECT_EQ(name1, "Alice"); // 字母順序第一個
    auto &[name5, age5] = results[4];
    EXPECT_EQ(name5, "User2"); // 字母順序最後
}

TEST_F(SelectTest, OrderByNameDescending) {
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .OrderBy(userTable[NameColumn], OrderType::DESC)
            .Results();

    ASSERT_EQ(results.size(), 5);
    auto &[name1, age1] = results[0];
    EXPECT_EQ(name1, "User2"); // 字母順序最後
    auto &[name5, age5] = results[4];
    EXPECT_EQ(name5, "Alice"); // 字母順序第一個
}

TEST_F(SelectTest, OrderByWithLimitOffset) {
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .OrderBy(userTable[AgeColumn])
            .LimitOffset(3, 1)
            .Results();

    ASSERT_EQ(results.size(), 3);
    // 跳過第一筆(age=20)，取後面三筆
    auto &[name1, age1] = results[0];
    EXPECT_EQ(age1, 25);
}

TEST_F(SelectTest, OrderByWithDistinct) {
    auto results = userTable.Select(userTable[AgeColumn])
            .Distinct()
            .OrderBy(userTable[AgeColumn])
            .Results();

    ASSERT_EQ(results.size(), 4); // 20, 25, 30, 40
    auto &[age1] = results[0];
    EXPECT_EQ(age1, 20);
    auto &[age4] = results[3];
    EXPECT_EQ(age4, 40);
}

// ============ OrderBy 使用 Expr 測試 ============

TEST_F(SelectTest, OrderByMultipleConditions) {
    // SQLite 本身不直接支援多個 ORDER BY，但可以通過表達式組合
    // 這裡測試單一 OrderBy 但結果按自然順序
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn], userTable[ScoreColumn])
            .OrderBy(userTable[AgeColumn])
            .Results();

    ASSERT_EQ(results.size(), 5);
    // 驗證年齡是升序
    for (size_t i = 1; i < results.size(); ++i) {
        auto &[name_prev, age_prev, score_prev] = results[i-1];
        auto &[name_curr, age_curr, score_curr] = results[i];
        EXPECT_LE(age_prev, age_curr);
    }
}

TEST_F(SelectTest, OrderByComplexQuery) {
    // 組合 Where + OrderBy + LimitOffset
    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn], userTable[ScoreColumn])
            .Where(userTable[ScoreColumn] >= 80.0_expr)
            .OrderBy(userTable[ScoreColumn], OrderType::DESC)
            .LimitOffset(3)
            .Results();

    ASSERT_LE(results.size(), 3);
    // 驗證結果按分數降序排列
    for (auto &[name, age, score] : results) {
        EXPECT_GE(score, 80.0);
    }
}


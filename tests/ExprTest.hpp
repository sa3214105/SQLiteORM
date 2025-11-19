#pragma once
#include "Common.hpp"

// ============ 表達式測試 ============

class ExprTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入測試數據
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("User1", 20);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("User2", 25);
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 測試簡單的表達式（加法）
TEST_F(ExprTest, SimpleAdditionTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn] + 1_expr
    ).Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), 21);
    EXPECT_EQ(std::get<1>(results[1]), 26);
}

// 測試表達式減法
TEST_F(ExprTest, SubtractionTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn] - 5_expr
    ).Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), 15);
    EXPECT_EQ(std::get<1>(results[1]), 20);
}

// 測試表達式乘法
TEST_F(ExprTest, MultiplicationTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn] * 2_expr
    ).Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), 40);
    EXPECT_EQ(std::get<1>(results[1]), 50);
}

// 測試表達式除法
TEST_F(ExprTest, DivisionTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn] / 2_expr
    ).Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), 10);
    EXPECT_EQ(std::get<1>(results[1]), 12.5);
}

// 測試複合表達式
TEST_F(ExprTest, ComplexExpressionTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Brackets(userTable[AgeColumn] + 5_expr) * 2_expr
    ).Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), 50);  // (20 + 5) * 2 = 50
    EXPECT_EQ(std::get<1>(results[1]), 60);  // (25 + 5) * 2 = 60
}

// 測試表達式比較（在 WHERE 中）
TEST_F(ExprTest, ComparisonInWhereTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).Where(userTable[AgeColumn] > 20_expr).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "User2");
    EXPECT_EQ(std::get<1>(results[0]), 25);
}

// 測試邏輯運算符 AND
TEST_F(ExprTest, LogicalAndTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).Where((userTable[AgeColumn] >= 20_expr) && (userTable[AgeColumn] <= 25_expr)).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試邏輯運算符 OR
TEST_F(ExprTest, LogicalOrTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).Where((userTable[AgeColumn] == 20_expr) || (userTable[AgeColumn] == 25_expr)).Results();

    EXPECT_EQ(results.size(), 2);
}

// 測試等於比較
TEST_F(ExprTest, EqualityTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).Where(userTable[AgeColumn] == 25_expr).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "User2");
}

// 測試不等於比較
TEST_F(ExprTest, InequalityTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).Where(userTable[AgeColumn] != 25_expr).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "User1");
}

// 測試小於等於
TEST_F(ExprTest, LessOrEqualTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).Where(userTable[AgeColumn] <= 20_expr).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "User1");
}

// 測試大於等於
TEST_F(ExprTest, GreaterOrEqualTest) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).Where(userTable[AgeColumn] >= 25_expr).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "User2");
}

// 測試表達式與聚合函數結合
TEST_F(ExprTest, ExpressionWithAggregateTest) {
    auto results = userTable.Select(
        Count(userTable[NameColumn])
    ).Where(userTable[AgeColumn] > 18_expr).Results();

    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), 2);
}

// 測試多個欄位的表達式
TEST_F(ExprTest, MultipleColumnExpressionTest) {
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>(
        "User3", 30, 85.5
    );

    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn] + 10_expr,
        userTable[ScoreColumn] * 1.1_expr
    ).Results();

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<1>(results[2]), 40);  // 30 + 10
    EXPECT_NEAR(std::get<2>(results[2]), 94.05, 0.01);  // 85.5 * 1.1
}


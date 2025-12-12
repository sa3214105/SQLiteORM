#pragma once
#include "Common.hpp"

// ============ 數學函數測試 ============

class MathFunctionTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_math.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入測試數據
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 85.5);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, -92.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Charlie", 45, 78.5);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("David", 60, 100.0);
    }

    void TearDown() override {
        std::remove("test_math.db");
    }
};

// 測試 Abs 函數
TEST_F(MathFunctionTest, AbsFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Abs(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 4);
    EXPECT_EQ(std::get<0>(results[1]), "Bob");
    EXPECT_DOUBLE_EQ(std::get<1>(results[1]), -92.0);
    EXPECT_DOUBLE_EQ(std::get<2>(results[1]), 92.0);
}

// 測試三角函數
TEST_F(MathFunctionTest, TrigonometricFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Sin(Radians(userTable[AgeColumn])),
        Cos(Radians(userTable[AgeColumn])),
        Tan(Radians(userTable[AgeColumn]))
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");

    // 驗證 sin(25°) ≈ 0.4226
    double sin_val = std::get<1>(results[0]);
    EXPECT_NEAR(sin_val, 0.4226, 0.01);
}

// 測試反三角函數
TEST_F(MathFunctionTest, InverseTrigonometricFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Asin(0.5_expr),
        Acos(0.5_expr),
        Atan(1.0_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 asin(0.5) = π/6 ≈ 0.5236 radians
    double asin_val = std::get<1>(results[0]);
    EXPECT_NEAR(asin_val, 0.5236, 0.01);

    // 驗證 acos(0.5) = π/3 ≈ 1.0472 radians
    double acos_val = std::get<2>(results[0]);
    EXPECT_NEAR(acos_val, 1.0472, 0.01);

    // 驗證 atan(1) = π/4 ≈ 0.7854 radians
    double atan_val = std::get<3>(results[0]);
    EXPECT_NEAR(atan_val, 0.7854, 0.01);
}

// 測試雙曲三角函數
TEST_F(MathFunctionTest, HyperbolicFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Sinh(1.0_expr),
        Cosh(1.0_expr),
        Tanh(1.0_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 sinh(1) ≈ 1.1752
    double sinh_val = std::get<1>(results[0]);
    EXPECT_NEAR(sinh_val, 1.1752, 0.01);

    // 驗證 cosh(1) ≈ 1.5431
    double cosh_val = std::get<2>(results[0]);
    EXPECT_NEAR(cosh_val, 1.5431, 0.01);

    // 驗證 tanh(1) ≈ 0.7616
    double tanh_val = std::get<3>(results[0]);
    EXPECT_NEAR(tanh_val, 0.7616, 0.01);
}

// 測試 Atan2 函數
TEST_F(MathFunctionTest, Atan2Function) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Atan2(1.0_expr, 1.0_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 atan2(1, 1) = π/4 ≈ 0.7854 radians
    double atan2_val = std::get<1>(results[0]);
    EXPECT_NEAR(atan2_val, 0.7854, 0.01);
}

// 測試指數和對數函數
TEST_F(MathFunctionTest, ExpAndLogFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Exp(1.0_expr),
        Ln(Exp(1.0_expr)),
        Log10(100.0_expr),
        Log2(8.0_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 exp(1) = e ≈ 2.7183
    double exp_val = std::get<1>(results[0]);
    EXPECT_NEAR(exp_val, 2.7183, 0.01);

    // 驗證 ln(e) = 1
    double ln_val = std::get<2>(results[0]);
    EXPECT_NEAR(ln_val, 1.0, 0.01);

    // 驗證 log10(100) = 2
    double log10_val = std::get<3>(results[0]);
    EXPECT_NEAR(log10_val, 2.0, 0.01);

    // 驗證 log2(8) = 3
    double log2_val = std::get<4>(results[0]);
    EXPECT_NEAR(log2_val, 3.0, 0.01);
}

// 測試帶底數的 Log 函數
TEST_F(MathFunctionTest, LogWithBaseFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Log(2.0_expr, 8.0_expr),
        Log(10.0_expr, 1000.0_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 log(2, 8) = 3
    double log_2_8 = std::get<1>(results[0]);
    EXPECT_NEAR(log_2_8, 3.0, 0.01);

    // 驗證 log(10, 1000) = 3
    double log_10_1000 = std::get<2>(results[0]);
    EXPECT_NEAR(log_10_1000, 3.0, 0.01);
}

// 測試 Power 和 Sqrt 函數
TEST_F(MathFunctionTest, PowerAndSqrtFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Power(2.0_expr, 3.0_expr),
        Sqrt(16.0_expr),
        Power(userTable[AgeColumn], 2.0_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 2^3 = 8
    double power_val = std::get<1>(results[0]);
    EXPECT_DOUBLE_EQ(power_val, 8.0);

    // 驗證 sqrt(16) = 4
    double sqrt_val = std::get<2>(results[0]);
    EXPECT_DOUBLE_EQ(sqrt_val, 4.0);

    // 驗證 25^2 = 625
    double age_squared = std::get<3>(results[0]);
    EXPECT_DOUBLE_EQ(age_squared, 625.0);
}

// 測試取整函數
TEST_F(MathFunctionTest, RoundingFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Ceil(userTable[ScoreColumn]),
        Floor(userTable[ScoreColumn]),
        Round(userTable[ScoreColumn]),
        Trunc(userTable[ScoreColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_DOUBLE_EQ(std::get<1>(results[0]), 85.5);

    // 驗證 ceil(85.5) = 86
    EXPECT_DOUBLE_EQ(std::get<2>(results[0]), 86.0);

    // 驗證 floor(85.5) = 85
    EXPECT_DOUBLE_EQ(std::get<3>(results[0]), 85.0);

    // 驗證 round(85.5) = 86
    EXPECT_DOUBLE_EQ(std::get<4>(results[0]), 86.0);

    // 驗證 trunc(85.5) = 85
    EXPECT_DOUBLE_EQ(std::get<5>(results[0]), 85.0);
}

// 測試 Round 帶精度參數
TEST_F(MathFunctionTest, RoundWithPrecision) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Round(userTable[ScoreColumn], 0_expr),
        Round(userTable[ScoreColumn], 1_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 round(85.5, 0) = 86
    EXPECT_DOUBLE_EQ(std::get<2>(results[0]), 86.0);

    // 驗證 round(85.5, 1) = 85.5
    EXPECT_DOUBLE_EQ(std::get<3>(results[0]), 85.5);
}

// 測試角度轉換函數
TEST_F(MathFunctionTest, AngleConversionFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Degrees(Pi()),
        Radians(180.0_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 degrees(π) = 180
    double degrees_val = std::get<1>(results[0]);
    EXPECT_NEAR(degrees_val, 180.0, 0.01);

    // 驗證 radians(180) = π ≈ 3.14159
    double radians_val = std::get<2>(results[0]);
    EXPECT_NEAR(radians_val, 3.14159, 0.01);
}

// 測試 Sign 函數
TEST_F(MathFunctionTest, SignFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Sign(userTable[ScoreColumn])
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 4);

    // Alice: 85.5 -> sign = 1
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<2>(results[0]), 1);

    // Bob: -92.0 -> sign = -1
    EXPECT_EQ(std::get<0>(results[1]), "Bob");
    EXPECT_EQ(std::get<2>(results[1]), -1);
}

// 測試 Pi 常數
TEST_F(MathFunctionTest, PiConstant) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Pi()
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 π ≈ 3.14159
    double pi_val = std::get<1>(results[0]);
    EXPECT_NEAR(pi_val, 3.14159, 0.00001);
}

// 測試 Mod 函數
TEST_F(MathFunctionTest, ModFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn],
        Mod(userTable[AgeColumn], 10_expr)
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 4);

    // Alice: 25 % 10 = 5
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), 25);
    EXPECT_EQ(std::get<2>(results[0]), 5);

    // Bob: 30 % 10 = 0
    EXPECT_EQ(std::get<0>(results[1]), "Bob");
    EXPECT_EQ(std::get<1>(results[1]), 30);
    EXPECT_EQ(std::get<2>(results[1]), 0);
}

// 測試組合數學函數
TEST_F(MathFunctionTest, CombinedMathFunctions) {
    // 計算 sqrt(age^2 + score^2) - 類似勾股定理
    auto results = userTable.Select(
        userTable[NameColumn],
        Sqrt(Power(userTable[AgeColumn], 2.0_expr) + Power(userTable[ScoreColumn], 2.0_expr))
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);

    // 驗證 sqrt(25^2 + 85.5^2) = sqrt(625 + 7310.25) = sqrt(7935.25) ≈ 89.08
    double pythagoras = std::get<1>(results[0]);
    EXPECT_NEAR(pythagoras, 89.08, 0.1);
}

// 測試使用數學函數在 WHERE 子句
TEST_F(MathFunctionTest, MathFunctionInWhere) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn]
    ).Where(Abs(userTable[ScoreColumn]) > 90.0_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 2);

    // Bob: -92.0, abs = 92.0 > 90
    EXPECT_EQ(std::get<0>(results[0]), "Bob");

    // David: 100.0, abs = 100.0 > 90
    EXPECT_EQ(std::get<0>(results[1]), "David");
}

// 測試使用數學函數排序
TEST_F(MathFunctionTest, MathFunctionInOrderBy) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[ScoreColumn],
        Abs(userTable[ScoreColumn])
    ).OrderBy(Abs(userTable[ScoreColumn]), OrderType::DESC).Results().ToVector();

    ASSERT_EQ(results.size(), 4);

    // 按絕對值降序：David(100), Bob(92), Alice(85.5), Charlie(78.5)
    EXPECT_EQ(std::get<0>(results[0]), "David");
    EXPECT_EQ(std::get<0>(results[1]), "Bob");
    EXPECT_EQ(std::get<0>(results[2]), "Alice");
    EXPECT_EQ(std::get<0>(results[3]), "Charlie");
}

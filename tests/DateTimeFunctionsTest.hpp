#pragma once
#include "Common.hpp"

// ============ 日期時間函數測試 ============

class DateTimeFunctionTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_datetime.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入測試數據
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 85.5);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 90.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Charlie", 35, 78.5);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Diana", 28, 92.0);
    }

    void TearDown() override {
        std::remove("test_datetime.db");
    }
};

// 測試 DATE 函數
TEST_F(DateTimeFunctionTest, DateFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Date("2024-12-13 15:30:45"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "2024-12-13");
}

// 測試 DATE 函數帶修飾符
TEST_F(DateTimeFunctionTest, DateFunctionWithModifier) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Date("2024-12-13"_expr, "+1 day"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "2024-12-14");
}

// 測試 DATE 函數使用 'now'
TEST_F(DateTimeFunctionTest, DateFunctionWithNow) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Date("now"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    std::string date = std::get<1>(results[0]);
    EXPECT_FALSE(date.empty());
    EXPECT_EQ(date.length(), 10);  // YYYY-MM-DD format
}

// 測試 DATETIME 函數
TEST_F(DateTimeFunctionTest, DateTimeFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        DateTime("2024-12-13 15:30:45"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "2024-12-13 15:30:45");
}

// 測試 DATETIME 函數帶修飾符
TEST_F(DateTimeFunctionTest, DateTimeFunctionWithModifier) {
    auto results = userTable.Select(
        userTable[NameColumn],
        DateTime("2024-12-13 15:30:45"_expr, "+1 hour"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "2024-12-13 16:30:45");
}

// 測試 DATETIME 函數使用 'now'
TEST_F(DateTimeFunctionTest, DateTimeFunctionWithNow) {
    auto results = userTable.Select(
        userTable[NameColumn],
        DateTime("now"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    std::string datetime = std::get<1>(results[0]);
    EXPECT_FALSE(datetime.empty());
    EXPECT_EQ(datetime.length(), 19);  // YYYY-MM-DD HH:MM:SS format
}

// 測試 JULIANDAY 函數
TEST_F(DateTimeFunctionTest, JulianDayFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        JulianDay("2024-12-13"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    double julianDay = std::get<1>(results[0]);
    EXPECT_GT(julianDay, 2400000.0);  // Julian day should be large number
    EXPECT_LT(julianDay, 3000000.0);
}

// 測試 JULIANDAY 函數帶修飾符
TEST_F(DateTimeFunctionTest, JulianDayFunctionWithModifier) {
    auto results = userTable.Select(
        userTable[NameColumn],
        JulianDay("2024-12-13"_expr, "+1 day"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    double julianDay = std::get<1>(results[0]);
    EXPECT_GT(julianDay, 2400000.0);
}

// 測試 JULIANDAY 函數計算日期差
TEST_F(DateTimeFunctionTest, JulianDayFunctionDateDiff) {
    auto results = userTable.Select(
        userTable[NameColumn],
        JulianDay("2024-12-14"_expr) - JulianDay("2024-12-13"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    double diff = std::get<1>(results[0]);
    EXPECT_DOUBLE_EQ(diff, 1.0);  // One day difference
}

// 測試 STRFTIME 函數
TEST_F(DateTimeFunctionTest, StrftimeFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Strftime("%Y-%m-%d"_expr, "2024-12-13 15:30:45"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "2024-12-13");
}

// 測試 STRFTIME 函數格式化時間
TEST_F(DateTimeFunctionTest, StrftimeFunctionTimeFormat) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Strftime("%H:%M:%S"_expr, "2024-12-13 15:30:45"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "15:30:45");
}

// 測試 STRFTIME 函數帶修飾符
TEST_F(DateTimeFunctionTest, StrftimeFunctionWithModifier) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Strftime("%Y-%m-%d"_expr, "2024-12-13"_expr, "+1 month"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "2025-01-13");
}

// 測試 STRFTIME 函數使用 'now'
TEST_F(DateTimeFunctionTest, StrftimeFunctionWithNow) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Strftime("%Y"_expr, "now"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    std::string year = std::get<1>(results[0]);
    EXPECT_EQ(year.length(), 4);
    EXPECT_GE(std::stoi(year), 2024);
}

// 測試 TIME 函數
TEST_F(DateTimeFunctionTest, TimeFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Time("2024-12-13 15:30:45"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "15:30:45");
}

// 測試 TIME 函數帶修飾符
TEST_F(DateTimeFunctionTest, TimeFunctionWithModifier) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Time("15:30:45"_expr, "+30 minutes"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "16:00:45");
}

// 測試 TIME 函數使用 'now'
TEST_F(DateTimeFunctionTest, TimeFunctionWithNow) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Time("now"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    std::string time = std::get<1>(results[0]);
    EXPECT_FALSE(time.empty());
    EXPECT_EQ(time.length(), 8);  // HH:MM:SS format
}

// 測試 TIMEDIFF 函數
TEST_F(DateTimeFunctionTest, TimeDiffFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        TimeDiff("15:30:45"_expr, "14:20:30"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    auto diff = std::get<1>(results[0]);
    EXPECT_EQ(diff, "+0000-00-00 01:10:15.000");
}

// 測試 TIMEDIFF 函數計算日期時間差
TEST_F(DateTimeFunctionTest, TimeDiffFunctionDateTime) {
    auto results = userTable.Select(
        userTable[NameColumn],
        TimeDiff("2024-12-14 10:00:00"_expr, "2024-12-13 10:00:00"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    auto diff = std::get<1>(results[0]);
    EXPECT_EQ(diff, "+0000-00-01 00:00:00.000");
}

// 測試 UNIXEPOCH 函數
TEST_F(DateTimeFunctionTest, UnixEpochFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        UnixEpoch("2024-12-13 00:00:00"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    int64_t timestamp = std::get<1>(results[0]);
    EXPECT_GT(timestamp, 1700000000);  // After 2023
    EXPECT_LT(timestamp, 2000000000);  // Before 2033
}

// 測試 UNIXEPOCH 函數帶修飾符
TEST_F(DateTimeFunctionTest, UnixEpochFunctionWithModifier) {
    auto results = userTable.Select(
        userTable[NameColumn],
        UnixEpoch("2024-12-13 00:00:00"_expr, "+1 day"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    int64_t timestamp = std::get<1>(results[0]);
    EXPECT_GT(timestamp, 1700000000);
}

// 測試 UNIXEPOCH 函數使用 'now'
TEST_F(DateTimeFunctionTest, UnixEpochFunctionWithNow) {
    auto results = userTable.Select(
        userTable[NameColumn],
        UnixEpoch("now"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    int64_t timestamp = std::get<1>(results[0]);
    EXPECT_GT(timestamp, 1700000000);  // Should be recent timestamp
}

// 測試組合使用日期時間函數
TEST_F(DateTimeFunctionTest, CombinedDateTimeFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Date(DateTime("now"_expr))
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    std::string date = std::get<1>(results[0]);
    EXPECT_EQ(date.length(), 10);  // YYYY-MM-DD format
}

// 測試在 WHERE 子句中使用日期時間函數
TEST_F(DateTimeFunctionTest, DateTimeFunctionInWhere) {
    auto results = userTable.Select(
        userTable[NameColumn]
    ).Where(
        Date("now"_expr) == Date("now"_expr)
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 4);  // All records should match
}

// 測試在 ORDER BY 中使用日期時間函數
TEST_F(DateTimeFunctionTest, DateTimeFunctionInOrderBy) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).OrderBy(
        JulianDay("now"_expr, "+"_expr + Cast<DataType::TEXT>(userTable[AgeColumn]) + " years"_expr),
        OrderType::DESC
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 4);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}



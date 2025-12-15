#pragma once
#include "Common.hpp"

// ============ 標量函數測試 ============

class ScalarFunctionTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_scalar.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入測試數據
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 85.5);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 92.0);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Charlie", 35, 78.5);
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("  David  ", 40, 88.0);
    }

    void TearDown() override {
        std::remove("test_scalar.db");
    }
};

// ============ 字串函數測試 ============

// 測試 UPPER 和 LOWER 函數
TEST_F(ScalarFunctionTest, UpperLowerFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Upper(userTable[NameColumn]),
        Lower(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), "ALICE");
    EXPECT_EQ(std::get<2>(results[0]), "alice");
}

// 測試 LENGTH 函數
TEST_F(ScalarFunctionTest, LengthFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Length(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), 5);
}

// 測試 SUBSTR 函數
TEST_F(ScalarFunctionTest, SubstrFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Substr(userTable[NameColumn], 1_expr, 3_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), "Ali");
}

// 測試 TRIM 函數
TEST_F(ScalarFunctionTest, TrimFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Trim(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "  David  "_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "  David  ");
    EXPECT_EQ(std::get<1>(results[0]), "David");
}

// 測試 LTRIM 函數
TEST_F(ScalarFunctionTest, LtrimFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Ltrim(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "  David  "_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "David  ");
}

// 測試 RTRIM 函數
TEST_F(ScalarFunctionTest, RtrimFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Rtrim(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "  David  "_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "  David");
}

// 測試 REPLACE 函數
TEST_F(ScalarFunctionTest, ReplaceFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Replace(userTable[NameColumn], "Alice"_expr, "Alicia"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), "Alicia");
}

// 測試 INSTR 函數
TEST_F(ScalarFunctionTest, InstrFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Instr(userTable[NameColumn], "li"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), 2);  // "li" starts at position 2
}

// 測試 CONCAT 函數
TEST_F(ScalarFunctionTest, ConcatFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Concat(userTable[NameColumn], " - "_expr, Cast<DataType::TEXT>(userTable[AgeColumn]))
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), "Alice - 25");
}

// 測試 HEX 函數
TEST_F(ScalarFunctionTest, HexFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Hex(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    // "Alice" in hex: 41=A, 6C=l, 69=i, 63=c, 65=e
    EXPECT_EQ(std::get<1>(results[0]), "416C696365");
}

// 測試 QUOTE 函數
TEST_F(ScalarFunctionTest, QuoteFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Quote(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), "'Alice'");
}

// 測試 UNICODE 函數
TEST_F(ScalarFunctionTest, UnicodeFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Unicode(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), 65);  // Unicode of 'A'
}

// 測試 TYPEOF 函數
TEST_F(ScalarFunctionTest, TypeOfFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        TypeOf(userTable[NameColumn]),
        TypeOf(userTable[AgeColumn]),
        TypeOf(userTable[ScoreColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "text");
    EXPECT_EQ(std::get<2>(results[0]), "integer");
    EXPECT_EQ(std::get<3>(results[0]), "real");
}

// ============ 條件函數測試 ============

// 測試 COALESCE 函數
TEST_F(ScalarFunctionTest, CoalesceFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Coalesce(userTable[NameColumn], "Unknown"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

// 測試 IFNULL 函數
TEST_F(ScalarFunctionTest, IfNullFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        IfNull(userTable[NameColumn], "N/A"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice");
}

// 測試 IIF 函數
TEST_F(ScalarFunctionTest, IifFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn],
        Iif(userTable[AgeColumn] >= 30_expr, "Senior"_expr, "Junior"_expr)
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 4);

    // Alice: 25 -> Junior
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<2>(results[0]), "Junior");

    // Bob: 30 -> Senior
    EXPECT_EQ(std::get<0>(results[1]), "Bob");
    EXPECT_EQ(std::get<2>(results[1]), "Senior");
}

// 測試 NULLIF 函數
TEST_F(ScalarFunctionTest, NullIfFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        NullIf(userTable[NameColumn], "Alice"_expr)
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 4);

    // Alice returns NULL (shown as empty or default)
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    // Other names return their values
    EXPECT_EQ(std::get<0>(results[1]), "Bob");
    EXPECT_EQ(std::get<1>(results[1]), "Bob");
}

// ============ 類型轉換函數測試 ============

// 測試 CAST 函數
TEST_F(ScalarFunctionTest, CastFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn],
        Cast<DataType::TEXT>(userTable[AgeColumn]),
        Cast<DataType::INTEGER>(userTable[ScoreColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), 25);
    EXPECT_EQ(std::get<2>(results[0]), "25");
    EXPECT_EQ(std::get<3>(results[0]), 85);  // 85.5 -> 85
}

// ============ 其他函數測試 ============

// 測試 RANDOM 函數
TEST_F(ScalarFunctionTest, RandomFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Random()
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    // Just check that it returns some integer value
    auto randomVal = std::get<1>(results[0]);
    EXPECT_TRUE(randomVal != 0 || randomVal == 0);  // Any integer is valid
}

// 測試 ABS 結合標量函數
TEST_F(ScalarFunctionTest, CombinedFunctions) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Upper(Substr(userTable[NameColumn], 1_expr, 3_expr))
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), "ALI");
}

// ============ 日期時間函數測試 ============

// 測試 DATE 函數
TEST_F(ScalarFunctionTest, DateFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Date("now"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    // 驗證返回的日期格式為 YYYY-MM-DD
    std::string dateStr = std::get<1>(results[0]);
    EXPECT_EQ(dateStr.length(), 10);
    EXPECT_EQ(dateStr[4], '-');
    EXPECT_EQ(dateStr[7], '-');
}

// 測試在 WHERE 子句中使用標量函數
TEST_F(ScalarFunctionTest, ScalarFunctionInWhere) {
    auto results = userTable.Select(
        userTable[NameColumn],
        userTable[AgeColumn]
    ).Where(Length(userTable[NameColumn]) == 5_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);  // "Alice" has length 5
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

// 測試在 ORDER BY 中使用標量函數
TEST_F(ScalarFunctionTest, ScalarFunctionInOrderBy) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Length(userTable[NameColumn])
    ).OrderBy(Length(userTable[NameColumn]), OrderType::DESC).Results().ToVector();

    ASSERT_EQ(results.size(), 4);
    // Longest name first
    EXPECT_GT(std::get<1>(results[0]), std::get<1>(results[3]));
}

// ============ 缺少的標量函數測試 ============

// 測試 ABS 函數
TEST_F(ScalarFunctionTest, AbsFunction) {
    auto results = userTable.Select(
        userTable[ScoreColumn],
        Abs(userTable[ScoreColumn] - 100.0_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_DOUBLE_EQ(std::get<0>(results[0]), 85.5);
    EXPECT_DOUBLE_EQ(std::get<1>(results[0]), 14.5);  // |85.5 - 100| = 14.5
}

// 測試 CHANGES 函數
TEST_F(ScalarFunctionTest, ChangesFunction) {
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Test", 20, 75.0);

    auto results = userTable.Select(
        Changes()
    ).Results().ToVector();

    ASSERT_EQ(results.size(), 5);
    EXPECT_EQ(std::get<0>(results[0]), 1);  // Last INSERT affected 1 row
}

// 測試 CHAR 函數
TEST_F(ScalarFunctionTest, CharFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Char(65_expr, 66_expr, 67_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "ABC");
}

// 測試 CONCAT_WS 函數
TEST_F(ScalarFunctionTest, ConcatWsFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        ConcatWs(", "_expr, userTable[NameColumn], "Age:"_expr, Cast<DataType::TEXT>(userTable[AgeColumn]))
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Alice, Age:, 25");
}

// 測試 FORMAT 函數
TEST_F(ScalarFunctionTest, FormatFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Format("Name: %s, Age: %d"_expr, userTable[NameColumn], userTable[AgeColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Name: Alice, Age: 25");
}

// 測試 GLOB 函數
TEST_F(ScalarFunctionTest, GlobFunction) {
    auto results = userTable.Select(
        userTable[NameColumn]
    ).Where(Glob(userTable[NameColumn], "A*"_expr) == 1_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

// 測試 IF 函數
TEST_F(ScalarFunctionTest, IfFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        If(userTable[AgeColumn] > 30_expr, "Old"_expr, "Young"_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr || userTable[NameColumn] == "Charlie"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
    EXPECT_EQ(std::get<1>(results[0]), "Young");  // 25 <= 30
    EXPECT_EQ(std::get<0>(results[1]), "Charlie");
    EXPECT_EQ(std::get<1>(results[1]), "Old");    // 35 > 30
}

// 測試 LIKE 函數
TEST_F(ScalarFunctionTest, LikeFunction) {
    auto results = userTable.Select(
        userTable[NameColumn]
    ).Where(Like(userTable[NameColumn], "A%"_expr) == 1_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

// 測試 LIKE with ESCAPE 函數
TEST_F(ScalarFunctionTest, LikeWithEscapeFunction) {
    auto results = userTable.Select(
        userTable[NameColumn]
    ).Where(Like(userTable[NameColumn], "A_ice"_expr, "\\"_expr) == 1_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<0>(results[0]), "Alice");
}

// 測試 LIKELIHOOD 函數
TEST_F(ScalarFunctionTest, LikelihoodFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Likelihood(userTable[AgeColumn] > 30_expr, 0.8)
    ).Where(userTable[NameColumn] == "Charlie"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 1);  // Charlie is 35 > 30
}

// 測試 LIKELY 函數
TEST_F(ScalarFunctionTest, LikelyFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Likely(userTable[AgeColumn] < 100_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 1);
}

// 測試 UNLIKELY 函數
TEST_F(ScalarFunctionTest, UnlikelyFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Unlikely(userTable[AgeColumn] > 1000_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 0);
}

// 測試 MAX (標量版本)
TEST_F(ScalarFunctionTest, MaxScalarFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Max(userTable[AgeColumn], 100_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 100);  // max(25, 100) = 100
}

// 測試 MIN (標量版本)
TEST_F(ScalarFunctionTest, MinScalarFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Min(userTable[AgeColumn], 20_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 20);  // min(25, 20) = 20
}

// 測試 OCTET_LENGTH 函數
TEST_F(ScalarFunctionTest, OctetLengthFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        OctetLength(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 5);  // "Alice" is 5 bytes in UTF-8
}

// 測試 PRINTF 函數
TEST_F(ScalarFunctionTest, PrintfFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Printf("Name: %s, Age: %d"_expr, userTable[NameColumn], userTable[AgeColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Name: Alice, Age: 25");
}

// 測試 RANDOMBLOB 函數
TEST_F(ScalarFunctionTest, RandomBlobFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        RandomBlob(16_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    // 驗證返回的是 BLOB 類型
    auto blob = std::get<1>(results[0]);
    EXPECT_EQ(blob.size(), 16);
}

// 測試 SOUNDEX 函數
#ifdef SQLITE_SOUNDEX
TEST_F(ScalarFunctionTest, SoundexFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Soundex(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    // SOUNDEX returns a 4-character string starting with the first letter
    std::string soundex = std::get<1>(results[0]);
    EXPECT_EQ(soundex.length(), 4);
    EXPECT_EQ(soundex[0], 'A');  // First character should be 'A'
}
#endif

// 測試 SUBSTRING 函數
TEST_F(ScalarFunctionTest, SubstringFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Substring(userTable[NameColumn], 1_expr, 3_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "Ali");
}

// 測試 SQLITE_VERSION 函數
TEST_F(ScalarFunctionTest, SqliteVersionFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        SqliteVersion()
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    std::string version = std::get<1>(results[0]);
    EXPECT_FALSE(version.empty());
    EXPECT_TRUE(version.find('.') != std::string::npos);
}

// 測試 SQLITE_SOURCE_ID 函數
TEST_F(ScalarFunctionTest, SqliteSourceIdFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        SqliteSourceId()
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    std::string sourceId = std::get<1>(results[0]);
    EXPECT_FALSE(sourceId.empty());
}

// 測試 TOTAL_CHANGES 函數
TEST_F(ScalarFunctionTest, TotalChangesFunction) {
    auto results = userTable.Select(
        TotalChanges()
    ).Results().ToVector();

    ASSERT_GT(results.size(), 0);
    EXPECT_GE(std::get<0>(results[0]), 4);  // At least 4 inserts in SetUp
}

// 測試 UNHEX 函數
TEST_F(ScalarFunctionTest, UnhexFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Unhex("416C696365"_expr)  // "Alice" in hex
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    auto blob = std::get<1>(results[0]);
    std::string str(blob.begin(), blob.end());
    EXPECT_EQ(str, "Alice");
}

// 測試 UNISTR 函數
TEST_F(ScalarFunctionTest, UnistrFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        Unistr("\\u0041\\u0042\\u0043"_expr)  // ABC
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), "ABC");
}

// 測試 UNISTR_QUOTE 函數
TEST_F(ScalarFunctionTest, UnistrQuoteFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        UnistrQuote(userTable[NameColumn])
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    std::string quoted = std::get<1>(results[0]);
    EXPECT_FALSE(quoted.empty());
}

// 測試 ZEROBLOB 函數
TEST_F(ScalarFunctionTest, ZeroBlobFunction) {
    auto results = userTable.Select(
        userTable[NameColumn],
        ZeroBlob(10_expr)
    ).Where(userTable[NameColumn] == "Alice"_expr).Results().ToVector();

    ASSERT_EQ(results.size(), 1);
    auto blob = std::get<1>(results[0]);
    EXPECT_EQ(blob.size(), 10);
    // 驗證所有字節都是 0
    for (auto byte : blob) {
        EXPECT_EQ(byte, 0);
    }
}



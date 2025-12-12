#pragma once
#include "Expressions.hpp"

namespace TypeSQLite {
    // ============ SQLite Core Scalar Functions (Alphabetical Order) ============
    // Note: MATCH, REGEXP are in Expressions.hpp as pattern matching operators
    // Note: Date/Time functions (DATE, DATETIME, JULIANDAY, STRFTIME, TIME, UNIXEPOCH) are in DateTimeFunctions.hpp
    // Note: JSON functions are in JsonFunctions.hpp

    // ABS - Absolute value
    template<ExprOrColConcept T>
    auto Abs(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("ABS(" + expr.sql + ")", expr);
    }

    // CHANGES - Number of rows modified by recent INSERT, UPDATE or DELETE
    inline auto Changes() {
        return MakeExpr<DataType::INTEGER>("CHANGES()");
    }

    // CHAR - Convert integers to characters
    template<ExprOrColConcept... Args>
    auto Char(const Args &... args) {
        std::string sql = "CHAR(";
        std::vector<std::string> parts;
        (parts.push_back(args.sql), ...);
        for (size_t i = 0; i < parts.size(); ++i) {
            sql += parts[i];
            if (i < parts.size() - 1) sql += ", ";
        }
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, args...);
    }

    // COALESCE - Return first non-NULL value
    template<ExprOrColConcept... Args>
    auto Coalesce(const Args &... args) {
        std::string sql = "COALESCE(";
        std::vector<std::string> parts;
        (parts.push_back(args.sql), ...);
        for (size_t i = 0; i < parts.size(); ++i) {
            sql += parts[i];
            if (i < parts.size() - 1) sql += ", ";
        }
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, args...);
    }

    // CONCAT - Concatenate strings
    template<ExprOrColConcept... Args>
    auto Concat(const Args &... args) {
        std::string sql = "CONCAT(";
        std::vector<std::string> parts;
        (parts.push_back(args.sql), ...);
        for (size_t i = 0; i < parts.size(); ++i) {
            sql += parts[i];
            if (i < parts.size() - 1) sql += ", ";
        }
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, args...);
    }

    // CONCAT_WS - Concatenate with separator
    template<ExprOrColConcept Sep, ExprOrColConcept... Args>
    auto ConcatWs(const Sep &separator, const Args &... args) {
        std::string sql = "CONCAT_WS(" + separator.sql;
        ((sql += ", " + args.sql), ...);
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, separator, args...);
    }


    // FORMAT - Format string
    template<ExprOrColConcept FormatStr, ExprOrColConcept... Args>
    auto Format(const FormatStr &format, const Args &... args) {
        std::string sql = "FORMAT(" + format.sql;
        ((sql += ", " + args.sql), ...);
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, format, args...);
    }

    // GLOB - Pattern matching (case-sensitive)
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Glob(const T1 &str, const T2 &pattern) {
        return MakeExpr<DataType::INTEGER>(str.sql + " GLOB " + pattern.sql, str, pattern);
    }

    // HEX - Convert to hexadecimal
    template<ExprOrColConcept T>
    auto Hex(const T &expr) {
        return MakeExpr<DataType::TEXT>("HEX(" + expr.sql + ")", expr);
    }

    // IF - Conditional expression
    template<ExprOrColConcept... Args>
    auto If(const Args&... args) {
        std::string sql = "IF(";
        std::vector<std::string> parts;
        (parts.push_back(args.sql), ...);
        for (size_t i = 0; i < parts.size(); ++i) {
            sql += parts[i];
            if (i < parts.size() - 1) sql += ", ";
        }
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, args...);
    }

    // IFNULL - Return replacement if NULL
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto IfNull(const T1 &expr, const T2 &replacement) {
        return MakeExpr<DataType::TEXT>("IFNULL(" + expr.sql + ", " + replacement.sql + ")", expr, replacement);
    }

    // IIF - Inline IF
    template<ExprOrColConcept Cond, ExprOrColConcept TrueVal, ExprOrColConcept FalseVal>
    auto Iif(const Cond &condition, const TrueVal &trueValue, const FalseVal &falseValue) {
        return MakeExpr<DataType::TEXT>("IIF(" + condition.sql + ", " + trueValue.sql + ", " + falseValue.sql + ")",
                                        condition, trueValue, falseValue);
    }

    // INSTR - Find substring position
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Instr(const T1 &str, const T2 &substr) {
        return MakeExpr<DataType::INTEGER>("INSTR(" + str.sql + ", " + substr.sql + ")", str, substr);
    }


    // LAST_INSERT_ROWID - Last inserted rowid
    inline auto LastInsertRowid() {
        return MakeExpr<DataType::INTEGER>("LAST_INSERT_ROWID()");
    }

    // LENGTH - String length
    template<ExprOrColConcept T>
    auto Length(const T &expr) {
        return MakeExpr<DataType::INTEGER>("LENGTH(" + expr.sql + ")", expr);
    }

    // LIKE - Pattern matching
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Like(const T1 &str, const T2 &pattern) {
        return MakeExpr<DataType::INTEGER>(str.sql + " LIKE " + pattern.sql, str, pattern);
    }

    // LIKE with ESCAPE
    template<ExprOrColConcept T1, ExprOrColConcept T2, ExprOrColConcept T3>
    auto Like(const T1 &str, const T2 &pattern, const T3 &escape) {
        return MakeExpr<DataType::INTEGER>(str.sql + " LIKE " + pattern.sql + " ESCAPE " + escape.sql, str, pattern,
                                           escape);
    }

    // LIKELIHOOD - Provide hint to query planner
    template<ExprOrColConcept T>
    auto Likelihood(const T& expr, double probability) {
        return MakeExpr<DataType::INTEGER>("LIKELIHOOD(" + expr.sql + ", " + std::to_string(probability) + ")", expr);
    }

    // LIKELY - Mark expression as likely to be true
    template<ExprOrColConcept T>
    auto Likely(const T& expr) {
        return MakeExpr<DataType::INTEGER>("LIKELY(" + expr.sql + ")", expr);
    }

    // LOAD_EXTENSION - Load extension
    template<ExprOrColConcept T>
    auto LoadExtension(const T& path) {
        return MakeExpr<DataType::TEXT>("LOAD_EXTENSION(" + path.sql + ")", path);
    }

    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto LoadExtension(const T1& path, const T2& entryPoint) {
        return MakeExpr<DataType::TEXT>("LOAD_EXTENSION(" + path.sql + ", " + entryPoint.sql + ")", path, entryPoint);
    }

    // LOWER - Convert to lowercase
    template<ExprOrColConcept T>
    auto Lower(const T &expr) {
        return MakeExpr<DataType::TEXT>("LOWER(" + expr.sql + ")", expr);
    }

    // LTRIM - Trim left whitespace
    template<ExprOrColConcept T>
    auto Ltrim(const T &expr) {
        return MakeExpr<DataType::TEXT>("LTRIM(" + expr.sql + ")", expr);
    }

    // LTRIM with specific characters
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Ltrim(const T1 &expr, const T2 &chars) {
        return MakeExpr<DataType::TEXT>("LTRIM(" + expr.sql + ", " + chars.sql + ")", expr, chars);
    }

    // MAX - Maximum value (scalar version with multiple arguments)
    template<ExprOrColConcept... Args>
    auto Max(const Args&... args) {
        std::string sql = "MAX(";
        std::vector<std::string> parts;
        (parts.push_back(args.sql), ...);
        for (size_t i = 0; i < parts.size(); ++i) {
            sql += parts[i];
            if (i < parts.size() - 1) sql += ", ";
        }
        sql += ")";
        return MakeExpr<DataType::NUMERIC>(sql, args...);
    }

    // MIN - Minimum value (scalar version with multiple arguments)
    template<ExprOrColConcept... Args>
    auto Min(const Args&... args) {
        std::string sql = "MIN(";
        std::vector<std::string> parts;
        (parts.push_back(args.sql), ...);
        for (size_t i = 0; i < parts.size(); ++i) {
            sql += parts[i];
            if (i < parts.size() - 1) sql += ", ";
        }
        sql += ")";
        return MakeExpr<DataType::NUMERIC>(sql, args...);
    }

    // NULLIF - Return NULL if equal
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto NullIf(const T1 &expr1, const T2 &expr2) {
        return MakeExpr<DataType::TEXT>("NULLIF(" + expr1.sql + ", " + expr2.sql + ")", expr1, expr2);
    }

    // OCTET_LENGTH - Length in bytes
    template<ExprOrColConcept T>
    auto OctetLength(const T& expr) {
        return MakeExpr<DataType::INTEGER>("OCTET_LENGTH(" + expr.sql + ")", expr);
    }

    // PRINTF - Formatted output
    template<ExprOrColConcept Format, ExprOrColConcept... Args>
    auto Printf(const Format &format, const Args &... args) {
        std::string sql = "PRINTF(" + format.sql;
        ((sql += ", " + args.sql), ...);
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, format, args...);
    }

    // QUOTE - Quote string for SQL
    template<ExprOrColConcept T>
    auto Quote(const T &expr) {
        return MakeExpr<DataType::TEXT>("QUOTE(" + expr.sql + ")", expr);
    }

    // RANDOM - Random integer
    inline auto Random() {
        return MakeExpr<DataType::INTEGER>("RANDOM()");
    }

    // RANDOMBLOB - Random blob
    template<ExprOrColConcept T>
    auto RandomBlob(const T &n) {
        return MakeExpr<DataType::BLOB>("RANDOMBLOB(" + n.sql + ")", n);
    }

    // REPLACE - Replace substring
    template<ExprOrColConcept T1, ExprOrColConcept T2, ExprOrColConcept T3>
    auto Replace(const T1 &str, const T2 &old, const T3 &newStr) {
        return MakeExpr<DataType::TEXT>("REPLACE(" + str.sql + ", " + old.sql + ", " + newStr.sql + ")", str, old,
                                        newStr);
    }

    // ROUND - Round to nearest integer
    template<ExprOrColConcept T>
    auto Round(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("ROUND(" + expr.sql + ")", expr);
    }

    // ROUND with digits
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Round(const T1 &expr, const T2 &digits) {
        return MakeExpr<DataType::NUMERIC>("ROUND(" + expr.sql + ", " + digits.sql + ")", expr, digits);
    }

    // RTRIM - Trim right whitespace
    template<ExprOrColConcept T>
    auto Rtrim(const T &expr) {
        return MakeExpr<DataType::TEXT>("RTRIM(" + expr.sql + ")", expr);
    }

    // RTRIM with specific characters
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Rtrim(const T1 &expr, const T2 &chars) {
        return MakeExpr<DataType::TEXT>("RTRIM(" + expr.sql + ", " + chars.sql + ")", expr, chars);
    }

    // SIGN - Sign of number
    template<ExprOrColConcept T>
    auto Sign(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("SIGN(" + expr.sql + ")", expr);
    }

    // SOUNDEX - Soundex encoding
#ifdef SQLITE_SOUNDEX
    template<ExprOrColConcept T>
    auto Soundex(const T &expr) {
        return MakeExpr<DataType::TEXT>("SOUNDEX(" + expr.sql + ")", expr);
    }
#endif

    // SQLITE_COMPILEOPTION_GET - Get compile-time option
    template<ExprOrColConcept T>
    auto SqliteCompileoptionGet(const T& n) {
        return MakeExpr<DataType::TEXT>("SQLITE_COMPILEOPTION_GET(" + n.sql + ")", n);
    }

    // SQLITE_COMPILEOPTION_USED - Check if compile-time option was used
    template<ExprOrColConcept T>
    auto SqliteCompileoptionUsed(const T& name) {
        return MakeExpr<DataType::INTEGER>("SQLITE_COMPILEOPTION_USED(" + name.sql + ")", name);
    }

    // SQLITE_OFFSET - Byte offset in database file
    template<ExprOrColConcept T>
    auto SqliteOffset(const T& expr) {
        return MakeExpr<DataType::INTEGER>("SQLITE_OFFSET(" + expr.sql + ")", expr);
    }

    // SQLITE_SOURCE_ID - Source ID of SQLite library
    inline auto SqliteSourceId() {
        return MakeExpr<DataType::TEXT>("SQLITE_SOURCE_ID()");
    }

    // SQLITE_VERSION - SQLite version
    inline auto SqliteVersion() {
        return MakeExpr<DataType::TEXT>("SQLITE_VERSION()");
    }


    // SUBSTR - Substring
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Substr(const T1 &str, const T2 &start) {
        return MakeExpr<DataType::TEXT>("SUBSTR(" + str.sql + ", " + start.sql + ")", str, start);
    }

    // SUBSTR with length
    template<ExprOrColConcept T1, ExprOrColConcept T2, ExprOrColConcept T3>
    auto Substr(const T1 &str, const T2 &start, const T3 &length) {
        return MakeExpr<DataType::TEXT>("SUBSTR(" + str.sql + ", " + start.sql + ", " + length.sql + ")", str, start,
                                        length);
    }

    // SUBSTRING - Alias for SUBSTR
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Substring(const T1 &str, const T2 &start) {
        return Substr(str, start);
    }

    template<ExprOrColConcept T1, ExprOrColConcept T2, ExprOrColConcept T3>
    auto Substring(const T1 &str, const T2 &start, const T3 &length) {
        return Substr(str, start, length);
    }

    // TOTAL_CHANGES - Total number of row changes
    inline auto TotalChanges() {
        return MakeExpr<DataType::INTEGER>("TOTAL_CHANGES()");
    }

    // TRIM - Trim whitespace
    template<ExprOrColConcept T>
    auto Trim(const T &expr) {
        return MakeExpr<DataType::TEXT>("TRIM(" + expr.sql + ")", expr);
    }

    // TRIM with specific characters
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Trim(const T1 &expr, const T2 &chars) {
        return MakeExpr<DataType::TEXT>("TRIM(" + expr.sql + ", " + chars.sql + ")", expr, chars);
    }

    // TYPEOF - Return type name
    template<ExprOrColConcept T>
    auto TypeOf(const T &expr) {
        return MakeExpr<DataType::TEXT>("TYPEOF(" + expr.sql + ")", expr);
    }

    // UNHEX - Convert hexadecimal to BLOB
    template<ExprOrColConcept T>
    auto Unhex(const T& expr) {
        return MakeExpr<DataType::BLOB>("UNHEX(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Unhex(const T1& expr, const T2& ignoreChars) {
        return MakeExpr<DataType::BLOB>("UNHEX(" + expr.sql + ", " + ignoreChars.sql + ")", expr, ignoreChars);
    }

    // UNICODE - Unicode code point
    template<ExprOrColConcept T>
    auto Unicode(const T &expr) {
        return MakeExpr<DataType::INTEGER>("UNICODE(" + expr.sql + ")", expr);
    }

    // UNISTR - Create string from Unicode code points
    template<ExprOrColConcept... Args>
    auto Unistr(const Args&... args) {
        std::string sql = "UNISTR(";
        std::vector<std::string> parts;
        (parts.push_back(args.sql), ...);
        for (size_t i = 0; i < parts.size(); ++i) {
            sql += parts[i];
            if (i < parts.size() - 1) sql += ", ";
        }
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, args...);
    }

    // UNISTR_QUOTE - Quote string using Unicode escapes
    template<ExprOrColConcept T>
    auto UnistrQuote(const T& expr) {
        return MakeExpr<DataType::TEXT>("UNISTR_QUOTE(" + expr.sql + ")", expr);
    }

    // UNLIKELY - Mark expression as unlikely to be true
    template<ExprOrColConcept T>
    auto Unlikely(const T& expr) {
        return MakeExpr<DataType::INTEGER>("UNLIKELY(" + expr.sql + ")", expr);
    }


    // UPPER - Convert to uppercase
    template<ExprOrColConcept T>
    auto Upper(const T &expr) {
        return MakeExpr<DataType::TEXT>("UPPER(" + expr.sql + ")", expr);
    }

    // ZEROBLOB - Create zero-filled blob
    template<ExprOrColConcept T>
    auto ZeroBlob(const T &n) {
        return MakeExpr<DataType::BLOB>("ZEROBLOB(" + n.sql + ")", n);
    }

} // namespace TypeSQLite

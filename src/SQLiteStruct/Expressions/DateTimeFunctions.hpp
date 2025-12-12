#pragma once
#include "Expressions.hpp"

namespace TypeSQLite {
    // ============ SQLite Date and Time Functions ============

    // DATE - Extract date (requires at least one time-value)
    template<ExprOrColConcept TimeValue, ExprOrColConcept... Modifiers>
    auto Date(const TimeValue &timeValue, const Modifiers &... modifiers) {
        std::string sql = "DATE(" + timeValue.sql;
        ((sql += ", " + modifiers.sql), ...);
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, timeValue, modifiers...);
    }

    // DATETIME - Extract datetime (requires at least one time-value)
    template<ExprOrColConcept TimeValue, ExprOrColConcept... Modifiers>
    auto DateTime(const TimeValue &timeValue, const Modifiers &... modifiers) {
        std::string sql = "DATETIME(" + timeValue.sql;
        ((sql += ", " + modifiers.sql), ...);
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, timeValue, modifiers...);
    }

    // JULIANDAY - Julian day number (requires at least one time-value)
    template<ExprOrColConcept TimeValue, ExprOrColConcept... Modifiers>
    auto JulianDay(const TimeValue &timeValue, const Modifiers &... modifiers) {
        std::string sql = "JULIANDAY(" + timeValue.sql;
        ((sql += ", " + modifiers.sql), ...);
        sql += ")";
        return MakeExpr<DataType::NUMERIC>(sql, timeValue, modifiers...);
    }

    // STRFTIME - Format time (requires format and at least one time-value)
    template<ExprOrColConcept Format, ExprOrColConcept TimeValue, ExprOrColConcept... Modifiers>
    auto Strftime(const Format &format, const TimeValue &timeValue, const Modifiers &... modifiers) {
        std::string sql = "STRFTIME(" + format.sql + ", " + timeValue.sql;
        ((sql += ", " + modifiers.sql), ...);
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, format, timeValue, modifiers...);
    }

    // TIME - Extract time (requires at least one time-value)
    template<ExprOrColConcept TimeValue, ExprOrColConcept... Modifiers>
    auto Time(const TimeValue &timeValue, const Modifiers &... modifiers) {
        std::string sql = "TIME(" + timeValue.sql;
        ((sql += ", " + modifiers.sql), ...);
        sql += ")";
        return MakeExpr<DataType::TEXT>(sql, timeValue, modifiers...);
    }

    // TIMEDIFF - Difference between two times
    template<ExprOrColConcept Time1, ExprOrColConcept Time2>
    auto TimeDiff(const Time1 &time1, const Time2 &time2) {
        std::string sql = "TIMEDIFF(" + time1.sql + ", " + time2.sql + ")";
        return MakeExpr<DataType::TEXT>(sql, time1, time2);
    }

    template<ExprOrColConcept TimeValue, ExprOrColConcept... Modifiers>
    auto UnixEpoch(const TimeValue &timeValue, const Modifiers &... modifiers) {
        std::string sql = "UNIXEPOCH(" + timeValue.sql;
        ((sql += ", " + modifiers.sql), ...);
        sql += ")";
        return MakeExpr<DataType::INTEGER>(sql, timeValue, modifiers...);
    }

} // namespace TypeSQLite

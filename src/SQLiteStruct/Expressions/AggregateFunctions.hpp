#pragma once
#include "../../TemplateHelper/FixedString.hpp"
#include "./Expressions.hpp"

namespace TypeSQLite {
    // AVG - Average value
    template<ExprOrColConcept T>
    auto Avg(const T &expr) {
        return MakeExpr("AVG(" + expr.sql + ")",expr);
    }

    // COUNT - Count rows
    template<ExprOrColConcept T>
    auto Count(const T &expr) {
        return MakeExpr("COUNT(" + expr.sql + ")",expr);
    }

    // MAX - Maximum value
    template<ExprOrColConcept T>
    auto Max(const T &expr) {
        return MakeExpr("MAX(" + expr.sql + ")",expr);
    }

    // MIN - Minimum value
    template<ExprOrColConcept T>
    auto Min(const T &expr) {
        return MakeExpr("MIN(" + expr.sql + ")",expr);
    }

    // SUM - Sum of values
    template<ExprOrColConcept T>
    auto Sum(const T &expr) {
        return MakeExpr("SUM(" + expr.sql + ")",expr);
    }

    // TOTAL - Total of values (returns 0.0 for empty set instead of NULL)
    template<ExprOrColConcept T>
    auto Total(const T &expr) {
        return MakeExpr("TOTAL(" + expr.sql + ")",expr);
    }

    // GROUP_CONCAT - Concatenate strings with separator
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto GroupConcat(const T1 &expr1, const T2 &expr2) {
        return MakeExpr<"GROUP_CONCAT(" + T1::sql + ", " + T2::sql + ")">(expr1, expr2);
    }

    // MEDIAN - Median value
    template<ExprOrColConcept T>
    auto Median(const T &expr) {
        return MakeExpr("MEDIAN(" + expr.sql + ")",expr);
    }

    // PERCENTILE - Percentile value
    template<double percent, ExprOrColConcept T>
    auto Percentile(const T &expr) {
        return MakeExpr("PERCENTILE(" + expr.sql + ", " + toFixedString<percent>() + ")",expr);
    }

    // PERCENTILE_CONT - Continuous percentile value
    template<double percent, ExprOrColConcept T>
    auto PercentileCont(const T &expr) {
        return MakeExpr("PERCENTILE_CONT(" + expr.sql + ", " + toFixedString<percent>() + ")",expr);
    }
}

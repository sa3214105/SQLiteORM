#pragma once
#include "../../TemplateHelper/FixedString.hpp"
#include "./Expressions.hpp"

namespace TypeSQLite {
    // AVG - Average value
    template<ExprConcept T>
    auto Avg(const T &expr) {
        return MakeExprWithOneOperands("AVG(" + expr.sql + ")",expr);
    }

    // COUNT - Count rows
    template<ExprConcept T>
    auto Count(const T &expr) {
        return MakeExprWithOneOperands("COUNT(" + expr.sql + ")",expr);
    }

    // MAX - Maximum value
    template<ExprConcept T>
    auto Max(const T &expr) {
        return MakeExprWithOneOperands("MAX(" + expr.sql + ")",expr);
    }

    // MIN - Minimum value
    template<ExprConcept T>
    auto Min(const T &expr) {
        return MakeExprWithOneOperands("MIN(" + expr.sql + ")",expr);
    }

    // SUM - Sum of values
    template<ExprConcept T>
    auto Sum(const T &expr) {
        return MakeExprWithOneOperands("SUM(" + expr.sql + ")",expr);
    }

    // TOTAL - Total of values (returns 0.0 for empty set instead of NULL)
    template<ExprConcept T>
    auto Total(const T &expr) {
        return MakeExprWithOneOperands("TOTAL(" + expr.sql + ")",expr);
    }

    // GROUP_CONCAT - Concatenate strings with separator
    template<ExprConcept T1, ExprConcept T2>
    auto GroupConcat(const T1 &expr1, const T2 &expr2) {
        return MakeExprWithTwoOperands<"GROUP_CONCAT(" + T1::sql + ", " + T2::sql + ")">(expr1, expr2);
    }

    // MEDIAN - Median value
    template<ExprConcept T>
    auto Median(const T &expr) {
        return MakeExprWithOneOperands("MEDIAN(" + expr.sql + ")",expr);
    }

    // PERCENTILE - Percentile value
    template<double percent, ExprConcept T>
    auto Percentile(const T &expr) {
        return MakeExprWithOneOperands("PERCENTILE(" + expr.sql + ", " + toFixedString<percent>() + ")",expr);
    }

    // PERCENTILE_CONT - Continuous percentile value
    template<double percent, ExprConcept T>
    auto PercentileCont(const T &expr) {
        return MakeExprWithOneOperands("PERCENTILE_CONT(" + expr.sql + ", " + toFixedString<percent>() + ")",expr);
    }
}

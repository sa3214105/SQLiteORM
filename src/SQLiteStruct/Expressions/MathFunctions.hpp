#pragma once
#include "Expressions.hpp"

namespace TypeSQLite {
    // Trigonometric functions
    template<ExprOrColConcept T>
    auto Acos(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("ACOS(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Acosh(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("ACOSH(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Asin(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("ASIN(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Asinh(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("ASINH(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Atan(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("ATAN(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Atan2(const T1 &y, const T2 &x) {
        return MakeExpr<DataType::NUMERIC>("ATAN2(" + y.sql + ", " + x.sql + ")", y, x);
    }

    template<ExprOrColConcept T>
    auto Atanh(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("ATANH(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Cos(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("COS(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Cosh(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("COSH(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Sin(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("SIN(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Sinh(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("SINH(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Tan(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("TAN(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Tanh(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("TANH(" + expr.sql + ")", expr);
    }

    // Exponential and logarithmic functions
    template<ExprOrColConcept T>
    auto Exp(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("EXP(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Ln(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("LN(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Log(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("LOG(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Log(const T1 &base, const T2 &x) {
        return MakeExpr<DataType::NUMERIC>("LOG(" + base.sql + ", " + x.sql + ")", base, x);
    }

    template<ExprOrColConcept T>
    auto Log10(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("LOG10(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Log2(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("LOG2(" + expr.sql + ")", expr);
    }

    // Power and root functions
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Power(const T1 &base, const T2 &exponent) {
        return MakeExpr<DataType::NUMERIC>("POWER(" + base.sql + ", " + exponent.sql + ")", base, exponent);
    }

    template<ExprOrColConcept T>
    auto Sqrt(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("SQRT(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Ceil(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("CEIL(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Ceiling(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("CEILING(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Floor(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("FLOOR(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Trunc(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("TRUNC(" + expr.sql + ")", expr);
    }

    // Note: ROUND and SIGN moved to ScalarFunctions.hpp

    // Other math functions
    template<ExprOrColConcept T>
    auto Degrees(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("DEGREES(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept T>
    auto Radians(const T &expr) {
        return MakeExpr<DataType::NUMERIC>("RADIANS(" + expr.sql + ")", expr);
    }


    // Pi constant
    inline auto Pi() {
        return MakeExpr<DataType::NUMERIC>("PI()");
    }

    // Modulo function
    template<ExprOrColConcept T1, ExprOrColConcept T2>
    auto Mod(const T1 &x, const T2 &y) {
        return MakeExpr<DataType::NUMERIC>("MOD(" + x.sql + ", " + y.sql + ")", x, y);
    }

} // namespace TypeSQLite
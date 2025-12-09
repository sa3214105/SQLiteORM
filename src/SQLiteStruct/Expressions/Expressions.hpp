// cpp
#pragma once
#include <tuple>
#include <type_traits>
#include <concepts>
#include <string>
#include "src/TemplateHelper/FixedString.hpp"

namespace TypeSQLite {
    enum class ExprResultType {
        TEXT,
        NUMERIC,
        INTEGER,
        REAL,
        BLOB
    };

    template<ExprResultType exprResultType, typename Columns, typename Parameters>
    struct Expressions {
        constexpr static ExprResultType resultType = exprResultType;
        const Columns cols;
        const std::string sql;
        const Parameters params;
    };

    template<typename T>
    struct IsExprBase {
    private:
        template<ExprResultType exprResultType, typename Columns, typename Parameters>
        static std::true_type test(const Expressions<exprResultType, Columns, Parameters> *) { return {}; }

        static std::false_type test(...) { return {}; }

    public:
        static constexpr bool value = decltype(test(std::declval<T *>()))::value;
    };

    template<typename T>
    concept ExprConcept = requires(T t)
    {
        // { T::resultType } -> std::same_as<ExprResultType>;
        { t.sql } -> std::convertible_to<std::string>;
        // { std::tuple_size<decltype(t.cols)>::value } -> std::convertible_to<std::size_t>;
        // { std::tuple_size<decltype(t.params)>::value } -> std::convertible_to<std::size_t>;
    };

    template<typename expr>
    using ExprResultValueType = std::conditional_t<expr::resultType == ExprResultType::TEXT, std::string,
        std::conditional_t<expr::resultType == ExprResultType::NUMERIC, double,
            std::conditional_t<expr::resultType == ExprResultType::INTEGER, int,
                std::conditional_t<expr::resultType == ExprResultType::REAL, double,
                    std::vector<uint8_t> > > > >;

    // sqlite literal
    inline auto operator""_expr(const char *str, size_t) {
        return Expressions<ExprResultType::TEXT, std::tuple<>, std::tuple<std::string> >{
            .cols = std::tuple<>{},
            .sql = "?",
            .params = std::make_tuple(std::string(str))
        };
    }

    inline auto operator""_expr(const long double value) {
        return Expressions<ExprResultType::NUMERIC, std::tuple<>, std::tuple<double> >{
            .cols = std::tuple<>{},
            .sql = "?",
            .params = std::make_tuple(static_cast<double>(value))
        };
    }

    inline auto operator""_expr(const unsigned long long value) {
        return Expressions<ExprResultType::NUMERIC, std::tuple<>, std::tuple<double> >{
            .cols = std::tuple<>{},
            .sql = "?",
            .params = std::make_tuple(static_cast<double>(value))
        };
    }

    // sqlite one operand operators
    template<ExprConcept T>
    auto MakeExprWithOneOperands(const std::string &newSQL, const T expr) {
        return Expressions<ExprResultType::NUMERIC, decltype(expr.cols), decltype(expr.params)>{
            .cols = expr.cols,
            .sql = newSQL,
            .params = expr.params
        };
    }

    template<ExprConcept expr>
    auto operator-(const expr &e) {
        return MakeExprWithOneOperands("-(" + e.sql + ")", e);
    }

    template<ExprConcept expr>
    auto operator+(const expr &e) {
        return MakeExprWithOneOperands("+(" + e.sql + ")", e);
    }

    template<ExprConcept expr>
    auto operator!(const expr &e) {
        return MakeExprWithOneOperands("NOT (" + e.sql + ")", e);
    }

    template<ExprConcept expr>
    auto operator~(const expr &e) {
        return MakeExprWithOneOperands("~ (" + e.sql + ")", e);
    }

    template<ExprConcept expr>
    auto Brackets(const expr &e) {
        return MakeExprWithOneOperands("(" + e.sql + ")", e);
    }

    // sqlite two operand operators
    template<ExprConcept lhs, ExprConcept rhs>
    auto MakeExprWithTwoOperands(std::string newSQL,const lhs &left, const rhs &right) {
        auto newCols = std::tuple_cat(left.cols, right.cols);
        auto newPara = std::tuple_cat(left.params, right.params);
        return Expressions<ExprResultType::NUMERIC, decltype(newCols), decltype(newPara)>{
            .cols = newCols,
            .sql = newSQL,
            .params = newPara
        };
    }


    template<ExprConcept lhs, ExprConcept rhs>
    auto operator+(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " + " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator-(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " - " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator*(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " * " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator/(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " / " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator%(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " % " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator^(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " ^ " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator&&(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands("(" + left.sql + ") AND (" + right.sql + ")", left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator||(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands("(" + left.sql + ") OR (" + right.sql + ")", left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator&(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " & " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator|(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " | " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator==(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " = " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator!=(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " <> " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator<(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " < " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator<=(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " <= " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator>(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " > " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator>=(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " >= " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator<<(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " << " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator>>(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " >> " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Like(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " LIKE " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Glob(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " GLOB " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Regexp(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " REGEXP " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Match(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " MATCH " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto IsNull(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " IS NULL " + right.sql, left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto IsNotNull(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands(left.sql + " IS NOT NULL " + right.sql, left, right);
    }

    //TODO 確認In功能

    template<ExprConcept Lhs, ExprConcept Mid, ExprConcept Rhs>
    auto Between(const Lhs &left, const Mid &mid, const Rhs &right) {
        constexpr auto newSQL = left.sql + " BETWEEN " + mid.sql + " AND " + right.sql;
        auto newCols = std::tuple_cat(left.cols, mid.cols, right.cols);
        auto newPara = std::tuple_cat(left.params, mid.params, right.params);
        return Expressions<ExprResultType::NUMERIC, decltype(newCols), decltype(newPara)>{
            .cols = newCols,
            .sql = newSQL,
            .params = newPara
        };
    }

    template<ExprConcept expr, ExprConcept... exprs>
    auto MakeExprList(const expr &first, const exprs &... rest) {
        if constexpr (sizeof...(rest) == 0) {
            return first.sql;
        } else {
            return MakeExprWithTwoOperands(first.sql + ", " + MakeExprList(rest...).sql, first, MakeExprList(rest...));
        }
    }

    template<ExprConcept expr, ExprConcept... exprs>
    auto GetExprParamTuple(const expr &first, const exprs &... rest) {
        if constexpr (sizeof...(rest) == 0) {
            return first.params;
        } else {
            return std::tuple_cat(first.params, GetExprParamTuple(rest...));
        }
    }
}

// cpp
#pragma once
#include <tuple>
#include <type_traits>
#include <concepts>
#include <string>
#include "../Column/Column.hpp"
#include "../DataType.hpp"

namespace TypeSQLite {
    template<DataType exprResultType, typename Columns, typename Parameters>
    struct Expressions {
        constexpr static DataType resultType = exprResultType;
        const Columns cols;
        const std::string sql;
        const Parameters params;
    };

    template<typename>
    struct IsExpressions : std::false_type {
    };

    template<DataType exprResultType, typename Columns, typename Parameters>
    struct IsExpressions<Expressions<exprResultType, Columns, Parameters> > : std::true_type {
    };

    template<typename T>
    concept ExpressionsConcept = IsExpressions<T>::value;

    template<typename T>
    concept ExprOrColConcept = ExpressionsConcept<T> || ColumnOrTableColumnConcept<T>;

    template<typename T>
    concept NullAbleExprOrColConcept = ExprOrColConcept<T> || std::is_same_v<T, nullptr_t>;

    template<ExprOrColConcept T>
    auto GetCols(const T &t) {
        if constexpr (ColumnOrTableColumnConcept<T>) {
            return std::make_tuple(t);
        } else {
            return t.cols;
        }
    }

    template<ExprOrColConcept T>
    auto GetParms(const T &t) {
        if constexpr (ColumnOrTableColumnConcept<T>) {
            return std::tuple<>{};
        } else {
            return t.params;
        }
    }

    template<DataType dataType, ExprOrColConcept ...Exps>
    auto MakeExpr(std::string newSQL, Exps... exps) {
        auto newCols = std::tuple_cat(GetCols(exps)...);
        auto newPara = std::tuple_cat(GetParms(exps)...);
        return Expressions<dataType, decltype(newCols), decltype(newPara)>{
            .cols = newCols,
            .sql = newSQL,
            .params = newPara
        };
    }

    template<typename expr>
    using ExprResultValueType = std::conditional_t<expr::resultType == DataType::TEXT, std::string,
        std::conditional_t<expr::resultType == DataType::NUMERIC, double,
            std::conditional_t<expr::resultType == DataType::INTEGER, int,
                std::conditional_t<expr::resultType == DataType::REAL, double,
                    std::vector<uint8_t> > > > >;

    // sqlite literal
    inline auto operator""_expr(const char *str, size_t) {
        return Expressions<DataType::TEXT, std::tuple<>, std::tuple<std::string> >{
            .cols = std::tuple<>{},
            .sql = "?",
            .params = std::make_tuple(std::string(str))
        };
    }

    inline auto operator""_expr(const long double value) {
        return Expressions<DataType::NUMERIC, std::tuple<>, std::tuple<double> >{
            .cols = std::tuple<>{},
            .sql = "?",
            .params = std::make_tuple(static_cast<double>(value))
        };
    }

    inline auto operator""_expr(const unsigned long long value) {
        return Expressions<DataType::NUMERIC, std::tuple<>, std::tuple<double> >{
            .cols = std::tuple<>{},
            .sql = "?",
            .params = std::make_tuple(static_cast<double>(value))
        };
    }

    template<ExprOrColConcept expr>
    auto operator-(const expr &e) {
        return MakeExpr<DataType::NUMERIC>("-(" + e.sql + ")", e);
    }

    template<ExprOrColConcept expr>
    auto operator+(const expr &e) {
        return MakeExpr<DataType::NUMERIC>("+(" + e.sql + ")", e);
    }

    template<ExprOrColConcept expr>
    auto operator!(const expr &e) {
        return MakeExpr<DataType::NUMERIC>("NOT (" + e.sql + ")", e);
    }

    template<ExprOrColConcept expr>
    auto operator~(const expr &e) {
        return MakeExpr<DataType::NUMERIC>("~ (" + e.sql + ")", e);
    }

    template<ExprOrColConcept expr>
    auto Brackets(const expr &e) {
        return MakeExpr<DataType::NUMERIC>("(" + e.sql + ")", e);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator+(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " + " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator-(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " - " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator*(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " * " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator/(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " / " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator%(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " % " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator^(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " ^ " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator&&(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>("(" + left.sql + ") AND (" + right.sql + ")", left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator||(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>("(" + left.sql + ") OR (" + right.sql + ")", left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator&(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " & " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator|(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " | " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator==(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " = " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator!=(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " <> " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator<(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " < " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator<=(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " <= " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator>(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " > " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator>=(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " >= " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator<<(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " << " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto operator>>(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " >> " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto Like(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " LIKE " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto Glob(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " GLOB " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto Regexp(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " REGEXP " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto Match(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " MATCH " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto IsNull(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " IS NULL " + right.sql, left, right);
    }

    template<ExprOrColConcept lhs, ExprOrColConcept rhs>
    auto IsNotNull(const lhs &left, const rhs &right) {
        return MakeExpr<DataType::NUMERIC>(left.sql + " IS NOT NULL " + right.sql, left, right);
    }

    //TODO 確認In功能

    template<ExprOrColConcept Lhs, ExprOrColConcept Mid, ExprOrColConcept Rhs>
    auto Between(const Lhs &left, const Mid &mid, const Rhs &right) {
        constexpr auto newSQL = left.sql + " BETWEEN " + mid.sql + " AND " + right.sql;
        auto newCols = std::tuple_cat(left.cols, mid.cols, right.cols);
        auto newPara = std::tuple_cat(left.params, mid.params, right.params);
        return Expressions<DataType::NUMERIC, decltype(newCols), decltype(newPara)>{
            .cols = newCols,
            .sql = newSQL,
            .params = newPara
        };
    }

    template<ExprOrColConcept expr, ExprOrColConcept... exprs>
    auto MakeExprList(const expr &first, const exprs &... rest) {
        if constexpr (sizeof...(rest) == 0) {
            return first.sql;
        } else {
            return MakeExpr<DataType::NUMERIC>(first.sql + ", " + MakeExprList(rest...).sql, first, MakeExprList(rest...));
        }
    }

    template<NullAbleExprOrColConcept expr, ExprOrColConcept... exprs>
    auto GetExprsColTuple(const expr &first, const exprs &... rest) {
        if constexpr (sizeof...(rest) == 0) {
            if constexpr (std::is_same_v<expr, std::nullptr_t>) {
                return std::tuple();
            } else if constexpr (ColumnOrTableColumnConcept<expr>) {
                return std::make_tuple(first);
            } else {
                return first.cols;
            }
        } else {
            return std::tuple_cat(first.cols, GetExprColTuple(rest...));
        }
    }

    template<typename ExprTuple>
    auto GetExprsTupleColTuple(const ExprTuple &exprs) {
        if constexpr (std::is_same_v<ExprTuple, nullptr_t>) {
            return std::tuple();
        } else {
            return std::apply(
                [](auto... expr) {
                    return GetExprsColTuple(expr...);
                },
                exprs
            );
        }
    }

    template<NullAbleExprOrColConcept expr, ExprOrColConcept... exprs>
    auto GetExprsParamTuple(const expr &first, const exprs &... rest) {
        if constexpr (sizeof...(rest) == 0) {
            if constexpr (std::is_same_v<expr, std::nullptr_t>) {
                return std::tuple();
            } else if constexpr (ColumnOrTableColumnConcept<expr>) {
                return std::tuple<>{};
            } else {
                return first.params;
            }
        } else {
            return std::tuple_cat(first.params, GetExprsParamTuple(rest...));
        }
    }

    template<typename ExprTuple>
    auto GetExprsTupleParamTuple(const ExprTuple &exprs) {
        if constexpr (std::is_same_v<ExprTuple, nullptr_t>) {
            return std::tuple();
        } else {
            return std::apply(
                [](auto... expr) {
                    return GetExprsParamTuple(expr...);
                },
                exprs
            );
        }
    }
}

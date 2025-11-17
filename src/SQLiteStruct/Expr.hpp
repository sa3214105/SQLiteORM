#pragma once
#include <tuple>
#include <type_traits>
#include "src/TemplateHelper/FixedString.hpp"

namespace TypeSQLite {
    enum class ExprResultType {
        TEXT,
        NUMERIC,
        INTEGER,
        REAL,
        BLOB
    };

    template<typename cols, ExprResultType exprResultType, FixedString partialSQL, typename...
        parameters>
    struct Expr {
        constexpr static FixedString sql = partialSQL;
        constexpr static ExprResultType resultType = exprResultType;
        using columns = cols;
        std::tuple<parameters...> params;

        explicit Expr(parameters... params) : params(std::forward<parameters>(params)...) {
        }
    };

    template<typename T>
    struct IsExprBase {
    private:
        template<typename cols, ExprResultType r, FixedString s, typename... P>
        static std::true_type test(const Expr<cols, r, s, P...> *) { return {}; }

        static std::false_type test(...) { return {}; }

    public:
        static constexpr bool value = decltype(test(std::declval<T *>()))::value;
    };

    template<typename T>
    concept ExprConcept = IsExprBase<T>::value;

    template<ExprConcept expr>
    using ExprResultValueType = std::conditional_t<expr::resultType == ExprResultType::TEXT, std::string,
        std::conditional_t<expr::resultType == ExprResultType::NUMERIC, double,
            std::conditional_t<expr::resultType == ExprResultType::INTEGER, int,
                std::conditional_t<expr::resultType == ExprResultType::REAL, double,
                    std::vector<uint8_t> > > > >;

    // sqlite literal
    inline auto operator""_expr(const char *str, size_t) {
        return Expr<TypeGroup<>, ExprResultType::TEXT, "?", std::string>(std::string(str));
    }

    inline auto operator""_expr(const long double value) {
        return Expr<TypeGroup<>, ExprResultType::NUMERIC, "?", double>(static_cast<double>(value));
    }

    inline auto operator""_expr(const unsigned long long value) {
        return Expr<TypeGroup<>, ExprResultType::NUMERIC, "?", int>(static_cast<int>(value));
    }

    // sqlite one operand operators
    template<FixedString newSQL, ExprConcept T>
    auto MakeExprWithOneOperands(const T &expr) {
        return std::apply([&](auto... para) {
            return Expr<typename T::columns, ExprResultType::NUMERIC, newSQL, std::remove_cvref_t<decltype(para)>
                ...>(para...);
        }, expr.params);
    }

    template<ExprConcept expr>
    auto operator-(const expr &e) {
        return MakeExprWithOneOperands<"-(" + expr::sql + ")">(e);
    }

    template<ExprConcept expr>
    auto operator+(const expr &e) {
        return MakeExprWithOneOperands<"+(" + expr::sql + ")">(e);
    }

    template<ExprConcept expr>
    auto operator!(const expr &e) {
        return MakeExprWithOneOperands<"NOT (" + expr::sql + ")">(e);
    }

    template<ExprConcept expr>
    auto operator~(const expr &e) {
        return MakeExprWithOneOperands<"~ (" + expr::sql + ")">(e);
    }

    // sqlite two operand operators
    template<FixedString newSQL, ExprConcept lhs, ExprConcept rhs>
    auto MakeExprWithTwoOperands(const lhs &left, const rhs &right) {
        using newCols = ConcatTypeGroup<typename lhs::columns, typename rhs::columns>::type;
        auto newPara = std::tuple_cat(left.params, right.params);
        return std::apply([&](auto... para) {
            return Expr<newCols, ExprResultType::NUMERIC, newSQL, std::remove_cvref_t<decltype(para)>...>(para...);
        }, newPara);
    }


    template<ExprConcept lhs, ExprConcept rhs>
    auto operator+(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " + " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator-(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " - " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator*(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " * " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator/(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " / " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator%(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " % " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator^(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " ^ " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator&&(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<"(" + lhs::sql + ") AND (" + rhs::sql + ")">(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator||(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<"(" + lhs::sql + ") OR (" + rhs::sql + ")">(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator&(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " & " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator|(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " | " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator==(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " = " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator!=(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " <> " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator<(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " < " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator<=(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " <= " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator>(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " > " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator>=(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " >= " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator<<(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " << " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto operator>>(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " >> " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Like(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " LIKE " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Glob(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " GLOB " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Regexp(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " REGEXP " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Match(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " MATCH " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto IsNull(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " IS NULL " + rhs::sql>(left, right);
    }

    template<ExprConcept lhs, ExprConcept rhs>
    auto IsNotNull(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<lhs::sql + " IS NOT NULL " + rhs::sql>(left, right);
    }

    //TODO 確認In功能

    // template<ExprConcept lhs, ExprConcept rhs>
    // auto In(const lhs &left, const rhs &right) {
    //     return MakeExprWithTwoOperands<lhs::sql + " IN " + rhs::sql>(left, right);
    // }

    template<ExprConcept lhs, ExprConcept rhs>
    auto Brackets(const lhs &left, const rhs &right) {
        return MakeExprWithTwoOperands<"(" + lhs::sql + " , " + rhs::sql + ")">(left, right);
    }

    template<ExprConcept Lhs, ExprConcept Mid, ExprConcept Rhs>
    auto Between(const Lhs &left, const Mid &mid, const Rhs &right) {
        constexpr auto newSQL = Lhs::sql + " BETWEEN " + Mid::sql + " AND " + Rhs::sql;
        using newCols = ConcatTypeGroup<typename ConcatTypeGroup<typename Lhs::columns, typename Mid::columns>::type,
            typename Rhs::columns>::type;
        auto newPara = std::tuple_cat(left.params, mid.params, right.params);
        return std::apply([&](auto... para) {
            return Expr<newCols, ExprResultType::NUMERIC, newSQL, std::remove_cvref_t<decltype(para)>...>(para...);
        }, newPara);
    }
}

#pragma once
#include "Column.hpp"

namespace SQLiteHelper {
    template<typename ColumnGroup, typename... Parameters>
    struct Condition {
        using Columns = ColumnGroup;
        std::string condition;
        std::tuple<Parameters...> params;

        template<typename ColumnOther, typename... ParametersOther>
        auto operator&&(const Condition<ColumnOther, ParametersOther...> &other) const {
            return Condition<typename ConcatTypeGroup<ColumnGroup, ColumnOther>::type, Parameters..., ParametersOther
                ...>{
                .condition = "(" + condition + ") AND (" + other.condition + ")",
                .params = std::tuple_cat(params, other.params)
            };
        }

        template<typename ColumnOther, typename... ParametersOther>
        auto operator||(const Condition<ColumnOther, ParametersOther...> &other) const {
            return Condition<typename ConcatTypeGroup<ColumnGroup, ColumnOther>::type, Parameters..., ParametersOther
                ...>{
                .condition = "(" + condition + ") OR (" + other.condition + ")",
                .params = std::tuple_cat(params, other.params)
            };
        }
    };

    template<typename T>
    concept IsSQLNum = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template<typename T>
    concept IsSQLLiteral = IsSQLNum<T> || std::is_same_v<T, std::string> || std::is_same_v<T, const char *>;

    template<FixedString Opt, IsTableColumn Col1, IsTableColumn Col2>
    auto MakeCondition() {
        return Condition<typeGroup<Col1, Col2> >{
            .condition = std::string(GetColumnName<Col1>()) + Opt + std::string(GetColumnName<Col2>()),
            .params = {}
        };
    }

    template<FixedString Opt, IsTableColumn Col, IsSQLLiteral V>
    auto MakeCondition(V v) {
        return Condition<typeGroup<Col>, std::conditional_t<std::is_same_v<const char *, V>, std::string, V> >{
            .condition = std::string(GetColumnName<Col>()) + Opt + "?",
            .params = {v}
        };
    }

    template<FixedString Opt, IsSQLLiteral V1, IsSQLLiteral V2>
    auto MakeCondition(V1 v1, V2 v2) {
        return Condition<void, std::conditional_t<std::is_same_v<const char *, V1>, std::string, V1>,
            std::conditional_t<std::is_same_v<const char *, V2>, std::string, V2> >{
            .condition = std::string("?") + Opt + "?",
            .params = {v1, v2}
        };
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto Equal() {
        return MakeCondition<" = ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto NotEqual() {
        return MakeCondition<" != ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto GreaterThan() {
        return MakeCondition<" > ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto GreaterThanEqual() {
        return MakeCondition<" >= ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto LessThan() {
        return MakeCondition<" < ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto LessThanEqual() {
        return MakeCondition<" <= ", Col1, Col2>();
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto Equal(V v) {
        return MakeCondition<" = ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto NotEqual(V v) {
        return MakeCondition<" != ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto GreaterThan(V v) {
        return MakeCondition<" > ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto GreaterThanEqual(V v) {
        return MakeCondition<" >= ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto LessThan(V v) {
        return MakeCondition<" < ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto LessThanEqual(V v) {
        return MakeCondition<" <= ", Col>(v);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto Equal(V1 v1, V2 v2) {
        return MakeCondition<" = ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto NotEqual(V1 v1, V2 v2) {
        return MakeCondition<" != ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto GreaterThan(V1 v1, V2 v2) {
        return MakeCondition<" > ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto GreaterThanEqual(V1 v1, V2 v2) {
        return MakeCondition<" >= ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto LessThan(V1 v1, V2 v2) {
        return MakeCondition<" < ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto LessThanEqual(V1 v1, V2 v2) {
        return MakeCondition<" <= ">(v1, v2);
    }
}
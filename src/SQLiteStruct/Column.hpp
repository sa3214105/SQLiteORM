#pragma once
#include "ColumnConstraints.hpp"
#include "../TemplateHelper/FixedString.hpp"
#include "Expr.hpp"

namespace TypeSQLite {
    template<ExprResultType type>
    constexpr auto ColumnTypeToString() {
        switch (type) {
            case ExprResultType::TEXT:
                return "TEXT";
            case ExprResultType::NUMERIC:
                return "NUMERIC";
            case ExprResultType::INTEGER:
                return "INTEGER";
            case ExprResultType::REAL:
                return "REAL";
            case ExprResultType::BLOB:
                return "BLOB";
            default:
                return "UNKNOWN";
        }
    }

    template<FixedString Name, ExprResultType Type, ColumnConstraintConcept... Constraints>
    struct Column : Expr<TypeGroup<Column<Name, Type, Constraints...> >, Type, Name> {
        constexpr static FixedString name = Name;
        constexpr static FixedString sql = Name;
        constexpr static ExprResultType resultType = Type;
        constexpr static ExprResultType type = Type;
        using columns = TypeGroup<Column>;
        using constraints = TypeGroup<Constraints...>;
    };

    template<typename>
    struct IsColumn : std::false_type {
    };

    template<FixedString Name, ExprResultType Type, typename... Constraints>
    struct IsColumn<Column<Name, Type, Constraints...> > : std::true_type {
    };

    template<typename T>
    concept ColumnConcept = IsColumn<T>::value;

    template<typename T, ColumnConcept U>
    struct TableColumn_Base : U {
        using TableType = T;
        constexpr static FixedString sql = T::name + FixedString(".") + U::name;
    };

    template<typename>
    struct IsTableColumn : std::false_type {
    };

    template<typename T, ColumnConcept U>
    struct IsTableColumn<TableColumn_Base<T, U> > : std::true_type {
    };

    template<typename T>
    concept TableColumnConcept = IsTableColumn<T>::value;

    template<typename T>
    concept ColumnOrTableColumnConcept = TableColumnConcept<T> || ColumnConcept<T>;

    template<typename>
    struct IsColumnOrTableColumnGroup : std::false_type {
    };

    template<ColumnOrTableColumnConcept ... Columns>
    struct IsColumnOrTableColumnGroup<TypeGroup<Columns...> > : std::true_type {
    };

    template<typename T>
    concept ColumnOrTableColumnGroupConcept = IsColumnOrTableColumnGroup<T>::value;

    //TODO 暫時先放寬約束
    template<typename/*ColumnOrTableColumnConcept*/ T>
    constexpr auto GetColumnName() {
        return T::sql;
        // if constexpr (TableColumnConcept<T>) {
        //     return T::TableType::name + FixedString(".") + T::name;
        // } else {
        //     return T::name;
        // }
    }

    //TODO 暫時先放寬約束
    template<typename/*ColumnOrTableColumnConcept*/ T, typename/*ColumnOrTableColumnConcept*/... Ts>
    constexpr auto GetColumnNames() {
        if constexpr (sizeof...(Ts) == 0) {
            return GetColumnName<T>();
        } else {
            return GetColumnName<T>() + "," + GetColumnNames<Ts...>();
        }
    }

    template<ColumnOrTableColumnConcept T, ColumnOrTableColumnConcept... Ts>
    std::string GetColumnNamesWithOutTableName() {
        if constexpr (sizeof...(Ts) == 0) {
            return std::string(T::name);
        } else {
            return std::string(T::name) + "," + GetColumnNamesWithOutTableName<Ts...>();
        }
    }

    template<ColumnOrTableColumnGroupConcept TG>
    constexpr auto GetNamesFromColumnOrTableColumnGroup() {
        if constexpr (std::is_same_v<TG, TypeGroup<> >) {
            return FixedString("");
        } else if constexpr (!std::is_same_v<typename TG::next, TypeGroup<> >) {
            return GetColumnName<typename TG::type>() + FixedString(",") +
                   GetNamesFromColumnOrTableColumnGroup<typename TG::next>();
        } else {
            return GetColumnName<typename TG::type>();
        }
    }

    template<ColumnConcept Column>
    constexpr auto GetColumnDefinition() {
        return FixedString(" " + Column::name + " ") +
               ColumnTypeToString<Column::type>() +
               GetColumnConstraintsSQL<typename Column::constraints>();
    }

    template<ColumnConcept... Columns>
    std::string GetColumnDefinitions() {
        std::string result;
        ((result += GetColumnDefinition<Columns>() + ","), ...);
        if (!result.empty()) result.pop_back(); // 去掉最後一個逗號
        return result;
    }
}

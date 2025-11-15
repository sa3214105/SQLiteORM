#pragma once
#include "ColumnConstraints.hpp"
#include "../TemplateHelper/FixedString.hpp"

namespace TypeSQLite {
    enum class column_type {
        TEXT,
        NUMERIC,
        INTEGER,
        REAL,
        BLOB
    };

    template<column_type type>
    constexpr auto ColumnTypeToString() {
        switch (type) {
            case column_type::TEXT:
                return "TEXT";
            case column_type::NUMERIC:
                return "NUMERIC";
            case column_type::INTEGER:
                return "INTEGER";
            case column_type::REAL:
                return "REAL";
            case column_type::BLOB:
                return "BLOB";
            default:
                return "UNKNOWN";
        }
    }

    template<FixedString Name, column_type Type, ColumnConstraintConcept... Constraints>
    struct Column {
        constexpr static FixedString name = Name;
        constexpr static column_type type = Type;
        using constraints = TypeGroup<Constraints...>;
        std::conditional_t<Type == column_type::TEXT, std::string,
            std::conditional_t<Type == column_type::NUMERIC, double,
                std::conditional_t<Type == column_type::INTEGER, int,
                    std::conditional_t<Type == column_type::REAL, double,
                        std::vector<uint8_t> > > > > value;
    };

    template<typename>
    struct IsColumn : std::false_type {
    };

    template<FixedString Name, column_type Type, typename... Constraints>
    struct IsColumn<Column<Name, Type, Constraints...> > : std::true_type {
    };

    template<typename T>
    concept ColumnConcept = IsColumn<T>::value;

    template<typename T, ColumnConcept U>
    struct TableColumn_Base : U {
        using TableType = T;
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
        if constexpr (TableColumnConcept<T>) {
            return T::TableType::name + FixedString(".") + T::name;
        } else {
            return T::name;
        }
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

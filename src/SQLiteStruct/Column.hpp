#pragma once
#include "ColumnConstraints.hpp"

namespace SQLiteHelper {
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

    template<FixedString Name, column_type Type, typename... Constraints>
    struct Column {
        constexpr static FixedString name = Name;
        constexpr static column_type type = Type;
        using constraints = typeGroup<Constraints...>;
        std::conditional_t<Type == column_type::TEXT, std::string,
            std::conditional_t<Type == column_type::NUMERIC, double,
                std::conditional_t<Type == column_type::INTEGER, int,
                    std::conditional_t<Type == column_type::REAL, double,
                        std::vector<uint8_t> > > > > value;
    };

    template<typename T>
    concept ColumnConcept = requires()
    {
        { T::type } -> std::convertible_to<column_type>;
        { T::name } -> std::convertible_to<std::string_view>;
    };

    template<typename T, ColumnConcept U>
    struct TableColumn_Base : U {
        using TableType = T;
    };

    template<typename T>
    concept IsTableColumn = requires()
    {
        typename T::TableType;
    };

    template<typename T>
    constexpr auto GetColumnName() {
        if constexpr (IsTableColumn<T>) {
            return T::TableType::name + FixedString(".") + T::name;
        } else {
            return T::name;
        }
    }

    template<typename T, typename... Ts>
    std::string GetColumnNamesWithOutTableName() {
        if constexpr (sizeof...(Ts) == 0) {
            return std::string(T::name);
        } else {
            return std::string(T::name) + "," + GetColumnNamesWithOutTableName<Ts...>();
        }
    }

    template<typename Column>
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
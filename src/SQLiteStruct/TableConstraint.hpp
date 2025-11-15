#pragma once
#include "Column.hpp"
#include "../TemplateHelper/FixedString.hpp"

namespace TypeSQLite {
    // Helper structure to pair a column with its order
    template<ColumnOrTableColumnConcept Column, OrderType order = OrderType::ASC>
    struct ColumnWithOrder {
        using column = Column;
        constexpr static OrderType orderType = order;
    };

    // Helper to check if type is ColumnWithOrder
    template<typename>
    struct IsColumnWithOrder : std::false_type {
    };

    template<ColumnOrTableColumnConcept Col, OrderType order>
    struct IsColumnWithOrder<ColumnWithOrder<Col, order>> : std::true_type {
    };

    // Helper to convert a single column/ColumnWithOrder to its name with order
    template<typename T>
    constexpr auto GetColumnNameWithOrder() {
        if constexpr (IsColumnWithOrder<T>::value) {
            return GetColumnName<typename T::column>() + OrderTypeToString<T::orderType>();
        } else if constexpr (ColumnOrTableColumnConcept<T>) {
            return GetColumnName<T>(); // No order specified, default behavior
        } else {
            static_assert(ColumnOrTableColumnConcept<T> || IsColumnWithOrder<T>::value,
                         "Type must be Column or ColumnWithOrder");
        }
    }

    // Helper to generate column names with orders from TypeGroup
    template<typename TG>
    constexpr auto GetNamesWithOrdersFromTypeGroup() {
        if constexpr (std::is_same_v<TG, TypeGroup<>>) {
            return FixedString("");
        } else if constexpr (!std::is_same_v<typename TG::next, TypeGroup<>>) {
            return GetColumnNameWithOrder<typename TG::type>() + FixedString(",") +
                   GetNamesWithOrdersFromTypeGroup<typename TG::next>();
        } else {
            return GetColumnNameWithOrder<typename TG::type>();
        }
    }

    //TODO COLLATE 暫時不實作
    template<typename Columns, ConflictCause conflictCause = ConflictCause::ABORT>
    struct TablePrimaryKey {
        constexpr static FixedString value = FixedString("PRIMARY KEY(") +
                                            GetNamesWithOrdersFromTypeGroup<Columns>() +
                                            FixedString(")") +
                                            ConflictCauseToString<conflictCause>();
    };

    template<typename Columns, ConflictCause conflictCause = ConflictCause::ABORT>
    struct TableUnique {
        constexpr static FixedString value = FixedString("UNIQUE(") +
                                            GetNamesWithOrdersFromTypeGroup<Columns>() +
                                            FixedString(")") +
                                            ConflictCauseToString<conflictCause>();
    };

    //TODO CHECK暫時不實作
    //TODO FOREIGN KEY暫時不實作

    // Table options (applied after table definition)
    struct WithoutRowId {
        constexpr static FixedString value = FixedString(" WITHOUT ROWID");
    };

    struct Strict {
        constexpr static FixedString value = FixedString(" STRICT");
    };

    // Table constraint concept
    template<typename>
    struct IsTableConstraint : std::false_type {
    };

    template<typename Columns, ConflictCause conflictCause>
    struct IsTableConstraint<TablePrimaryKey<Columns, conflictCause>> : std::true_type {
    };

    template<typename Columns, ConflictCause conflictCause>
    struct IsTableConstraint<TableUnique<Columns, conflictCause>> : std::true_type {
    };

    template<typename T>
    concept TableConstraintConcept = IsTableConstraint<T>::value;

    // Table option concept
    template<typename>
    struct IsTableOption : std::false_type {
    };

    template<>
    struct IsTableOption<WithoutRowId> : std::true_type {
    };

    template<>
    struct IsTableOption<Strict> : std::true_type {
    };

    template<typename T>
    concept TableOptionConcept = IsTableOption<T>::value;

    template<typename T>
    concept ColumnOrTableConstraintConcept = ColumnConcept<T> || TableConstraintConcept<T>;

    template<typename T>
    concept ColumnOrTableConstraintOrOptionConcept = ColumnConcept<T> || TableConstraintConcept<T> || TableOptionConcept<T>;
}

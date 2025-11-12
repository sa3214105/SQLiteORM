#pragma once
#include "Order.hpp"
#include "ConflictCause.hpp"
#include "../TemplateHelper/FixedType.hpp"
#include "../TemplateHelper/TypeGroup.hpp"

namespace SQLiteHelper {
    template<OrderType order = OrderType::ASC, ConflictCause conflictCause = ConflictCause::ABORT>
    struct ColumnPrimaryKey {
        constexpr static FixedString value = FixedString("PRIMARY KEY") + OrderTypeToString<order>() +
                                             ConflictCauseToString<conflictCause>();
    };

    template<ConflictCause conflictCause = ConflictCause::ABORT>
    struct ColumnNotNull {
        constexpr static FixedString value = "NOT NULL" + ConflictCauseToString<conflictCause>();
    };

    template<ConflictCause conflictCause = ConflictCause::ABORT>
    struct ColumnUnique {
        constexpr static FixedString value = "UNIQUE" + ConflictCauseToString<conflictCause>();
    };

    // Helper to convert FixedString to a quoted string for DEFAULT
    template<size_t N>
    constexpr auto toFixedStringLiteral(const FixedString<N> &fs) {
        return FixedString("'") + fs + FixedString("'");
    }

    template<FixedType DefaultValue>
    struct Default {
        constexpr static auto GetDefaultValueString() {
            using ValueType = decltype(DefaultValue.value);
            if constexpr (IsFixedString<ValueType>::value) {
                // For string types, wrap in quotes
                return FixedString("DEFAULT ") + toFixedStringLiteral(DefaultValue.value);
            } else if constexpr (std::is_integral_v<ValueType>) {
                // For integer types
                return FixedString("DEFAULT ") + toFixedString<DefaultValue.value>();
            } else if constexpr (std::is_floating_point_v<ValueType>) {
                // For floating point types
                return FixedString("DEFAULT ") + toFixedString<DefaultValue.value>();
            } else {
                return FixedString("DEFAULT ") + toFixedString<DefaultValue.value>();
            }
        }

        constexpr static auto value = GetDefaultValueString();
    };

    template<typename>
    struct IsColumnConstraint : std::false_type {
    };

    template<OrderType order, ConflictCause conflictCause>
    struct IsColumnConstraint<ColumnPrimaryKey<order, conflictCause> > : std::true_type {
    };

    template<ConflictCause conflictCause>
    struct IsColumnConstraint<ColumnNotNull<conflictCause> > : std::true_type {
    };

    template<ConflictCause conflictCause>
    struct IsColumnConstraint<ColumnUnique<conflictCause> > : std::true_type {
    };

    template<FixedType fixType>
    struct IsColumnConstraint<Default<fixType> > : std::true_type {
    };

    template<typename T>
    concept ColumnConstraintConcept = IsColumnConstraint<T>::value;

    template<typename>
    struct IsColumnConstraintGroup : std::false_type {
    };

    template<ColumnConstraintConcept ... Constraints>
    struct IsColumnConstraintGroup<TypeGroup<Constraints...> > : std::true_type {
    };

    template<typename TG>
    concept ColumnConstraintGroupConcept = IsColumnConstraintGroup<TG>::value;

    template<ColumnConstraintGroupConcept TG>
    constexpr auto GetColumnConstraintsSQL() {
        if constexpr (std::is_same_v<TG, TypeGroup<> >) {
            return FixedString(" ");
        } else if constexpr (!std::is_same_v<typename TG::next, TypeGroup<> >) {
            return " " + TG::type::value + GetColumnConstraintsSQL<typename TG::next>();
        } else {
            return " " + TG::type::value;
        }
    }
}

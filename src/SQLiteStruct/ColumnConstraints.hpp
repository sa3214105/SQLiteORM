#pragma once
#include "Order.hpp"
#include "ConflictCause.hpp"

namespace SQLiteHelper {
    template<OrderType order = OrderType::ASC, ConflictCause conflictCause = ConflictCause::ABORT>
    struct PrimaryKey {
        constexpr static FixedString value = FixedString("PRIMARY KEY") + OrderTypeToString<order>() +
                                             ConflictCauseToString<conflictCause>();
    };

    template<ConflictCause conflictCause = ConflictCause::ABORT>
    struct NotNull {
        constexpr static FixedString value = "NOT NULL" + ConflictCauseToString<conflictCause>();
    };

    template<ConflictCause conflictCause = ConflictCause::ABORT>
    struct Unique {
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

    template<typename TG>
    constexpr auto GetColumnConstraintsSQL() {
        if constexpr (std::is_same_v<TG, typeGroup<> >) {
            return FixedString(" ");
        } else if constexpr (!std::is_same_v<typename TG::next, typeGroup<> >) {
            return " " + TG::type::value + GetColumnConstraintsSQL<typename TG::next>();
        } else {
            return " " + TG::type::value;
        }
    }
}

#pragma once
#include "../TemplateHelper/FixedString.hpp"

namespace TypeSQLite {
    enum class OrderType {
        ASC,
        DESC
    };

    template<OrderType order>
    constexpr auto OrderTypeToString() {
        if constexpr (order == OrderType::ASC) {
            return FixedString(" ASC");
        } else {
            return FixedString(" DESC");
        }
    }
}
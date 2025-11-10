#pragma once
#include <type_traits>

template<typename T, size_t N = 0>
struct FixedType {
    using SaveType = std::conditional_t<std::is_same_v<char, T> && N != 0, FixedString<N>, T>;
    SaveType value;

    constexpr FixedType(T u) : value(u) {
    }

    constexpr FixedType(const T (&str)[N]) : value(str) {
    }
};

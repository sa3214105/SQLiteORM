#pragma once

template<typename... Ts>
    struct typeGroup;

template<typename T, typename... Ts>
struct typeGroup<T, Ts...> {
    using type = T;
    using next = typeGroup<Ts...>;
};

template<>
struct typeGroup<> {
    using type = void;
};

template<typename G1, typename G2>
    struct ConcatTypeGroup;

template<typename... Ts1, typename... Ts2>
struct ConcatTypeGroup<typeGroup<Ts1...>, typeGroup<Ts2...> > {
    using type = typeGroup<Ts1..., Ts2...>;
};

template<typename T, typename TG>
    constexpr bool findTypeInTypeGroup() {
    if constexpr (std::is_same_v<T, typename TG::type>) {
        return true;
    } else if constexpr (!std::is_same_v<typename TG::next, typeGroup<> >) {
        return findTypeInTypeGroup<T, typename TG::next>();
    } else {
        return false;
    }
}

template<typename TG1, typename TG2>
constexpr bool isTypeGroupSubset() {
    if constexpr (std::is_void_v<typename TG1::type>) {
        return true; // 空的 TG1 是任何 TG2 的子集
    } else {
        if constexpr (findTypeInTypeGroup<typename TG1::type, TG2>()) {
            if constexpr (!std::is_same_v<typename TG1::next, typeGroup<> >) {
                return isTypeGroupSubset<typename TG1::next, TG2>();
            } else {
                return true; // 已檢查完 TG1 的所有型別
            }
        } else {
            return false; // 找不到 TG1 的型別於 TG2 中
        }
    }
}
#pragma once
#include <string>
#include <tuple>
#include <stdexcept>
#include <type_traits>

namespace TypeSQLite {
    enum class JoinType {
        FULL,
        INNER,
        LEFT,
        RIGHT,
        CROSS
    };

    template<JoinType jt>
    constexpr auto GetJoinTypeString() {
        if constexpr (jt == JoinType::FULL) {
            return " FULL JOIN ";
        } else if constexpr (jt == JoinType::INNER) {
            return " INNER JOIN ";
        } else if constexpr (jt == JoinType::LEFT) {
            return " LEFT JOIN ";
        } else if constexpr (jt == JoinType::RIGHT) {
            return " RIGHT JOIN ";
        } else if constexpr (jt == JoinType::CROSS) {
            return " CROSS JOIN ";
        }
        throw std::runtime_error("Unsupported join type");
    }

    inline std::string GetJoinTypeString(const JoinType &jt) {
        switch (jt) {
            case JoinType::FULL:
                return " FULL JOIN ";
            case JoinType::INNER:
                return " INNER JOIN ";
            case JoinType::LEFT:
                return " LEFT JOIN ";
            case JoinType::RIGHT:
                return " RIGHT JOIN ";
            case JoinType::CROSS:
                return " CROSS JOIN ";
            default:
                throw std::runtime_error("Unsupported join type");
        }
    }

    template<typename Src, typename Cond>
    struct DataSource {
        using Source = Src;
        JoinType type;
        Cond condition;
    };

    template<typename MainSrc, typename... JoinSrcs>
    struct SourceInfo {
        using Source = MainSrc;
        std::tuple<JoinSrcs...> joins;
    };

    template<typename>
    struct IsSourceInfo : std::false_type {
    };

    template<typename MainSrc, typename... JoinSrcs>
    struct IsSourceInfo<SourceInfo<MainSrc, JoinSrcs...> > : std::true_type {
    };

    template<typename T>
    concept SourceInfoConcept = IsSourceInfo<T>::value;

    template<typename MainSrc, typename... JoinSrcs, typename NewJoin>
    auto JoinSource(SourceInfo<MainSrc, JoinSrcs...> src, NewJoin join) {
        using NewType = SourceInfo<MainSrc, JoinSrcs..., NewJoin>;
        return NewType{
            .joins = std::tuple_cat(src.joins, std::make_tuple(std::move(join)))
        };
    }

    template<typename MainSrc, typename... Joins>
    std::string MakeSourceSQL(const SourceInfo<MainSrc, Joins...> &src) {
        std::string sql = std::string(MainSrc::name);
        std::apply([&](const auto &... join) {
            // 展開每個 JoinInfo，使用 expr.sql
            ((sql += GetJoinTypeString(join.type)
              + std::string(std::remove_cvref_t<decltype(join)>::Source::name)
              + " ON " + std::string(join.condition.sql)), ...);
        }, src.joins);
        return sql;
    }

    // 提取 SourceInfo 中所有 JOIN 條件的參數
    template<typename MainSrc, typename... Joins>
    auto GetExtractSourceParams(const SourceInfo<MainSrc, Joins...> &src) {
        return std::apply([](const auto &... join) {
            return std::tuple_cat(join.condition.params...);
        }, src.joins);
    }

    template<typename MainSrc, typename... Joins>
    auto GetExtractSourceCols(const SourceInfo<MainSrc, Joins...> &src) {
        return std::apply([](const auto &... join) {
            return std::tuple_cat(GetCols(join.condition)...);
        }, src.joins);
    }

    // 特化：沒有 JOIN 時返回空 tuple
    template<typename MainSrc>
    auto GetExtractSourceParams(const SourceInfo<MainSrc> &src) {
        return std::tuple<>();
    }

    template<typename MainSrc>
    auto GetExtractSourceCols(const SourceInfo<MainSrc> &src) {
        return std::tuple<>();
    }
}

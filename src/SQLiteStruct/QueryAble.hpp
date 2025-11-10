#pragma once
#include "../SQLiteWrapper.hpp"
#include "DataSource.h"

namespace SQLiteHelper {
    template<typename Columns, typename Src>
    class QueryAble {
    public:
        using columns = Columns;
        using Source = Src;

    protected:
        SQLiteWrapper &_sqlite;
        const Source _source;

        template<typename... ResultColumns>
        class SelectQuery {
            const SQLiteWrapper &_sqlite;
            const Source &_source;
            std::string _basic_sql;
            std::function<std::vector<std::tuple<ResultColumns...> >()> _query_func;

            template<typename T, typename... Ts>
            std::string GetColumnNames() {
                if constexpr (IsTableColumn<T>) {
                    if constexpr (sizeof...(Ts) == 0) {
                        return std::string(T::TableType::name) + "." + std::string(T::name);
                    } else {
                        return std::string(T::TableType::name) + "." + std::string(T::name) + "," +
                               GetColumnNames<Ts...>();
                    }
                } else {
                    if constexpr (sizeof...(Ts) == 0) {
                        return std::string(T::name);
                    } else {
                        return std::string(T::name) + "," + GetColumnNames<Ts...>();
                    }
                }
            }

        public:
            explicit SelectQuery(const SQLiteWrapper &sqlite, const Source &source) : _sqlite(sqlite), _source(source) {
                _basic_sql = "SELECT " + GetColumnNames<ResultColumns...>();
                _query_func = [this]() {
                    auto sql = _basic_sql + " FROM " + MakeSourceSQL(_source) + ";";
                    auto params = ExtractSourceParams(_source);
                    return std::apply([this, &sql](auto &&... args) {
                        return _sqlite.Query<ResultColumns...>(sql, args...);
                    }, params);
                };
            }

            template<typename Cond>
            SelectQuery &Where(const Cond &condition) {
                _query_func = [this, condition]() {
                    auto sql = _basic_sql + " FROM " + MakeSourceSQL(_source) + " WHERE " + condition.condition + ";";
                    // 合併 JOIN 參數和 WHERE 參數
                    auto source_params = ExtractSourceParams(_source);
                    auto all_params = std::tuple_cat(source_params, condition.params);
                    return std::apply([this, &sql](auto &&... params) {
                        return _sqlite.Query<ResultColumns...>(sql, params...);
                    }, all_params);
                };
                return *this;
            }

            std::vector<std::tuple<ResultColumns...> > Results() {
                return _query_func();
            }
        };

    public:
        explicit QueryAble(SQLiteWrapper &sqlite, Source source) : _sqlite(sqlite), _source(source) {
        }

        virtual ~QueryAble() = default;

        template<typename... ResultCol>
        auto Select() const {
            static_assert(isTypeGroupSubset<typeGroup<ResultCol...>, columns>(),
                          "ResultCol must be subset of table columns");
            return SelectQuery<ResultCol...>(_sqlite, _source);
        }

        template<typename Table2, typename Cond>
        auto FullJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::FULL,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<typename Table2, typename Cond>
        auto InnerJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::INNER,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<typename Table2, typename Cond>
        auto LeftJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::LEFT,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<typename Table2, typename Cond>
        auto RightJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::RIGHT,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<typename Table2, typename Cond>
        auto CrossJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::CROSS,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }
    };
}

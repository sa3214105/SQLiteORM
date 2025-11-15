#pragma once
#include "../SQLiteWrapper.hpp"
#include "DataSource.h"
#include "Column.hpp"
#include "Condition.hpp"
#include "AggregateFunctions.hpp"

namespace SQLiteHelper {
    template<ColumnOrTableColumnGroupConcept Columns, SourceInfoConcept Src>
    class QueryAble;

    template<typename>
    struct IsQueryAble : std::false_type {
    };

    template<ColumnOrTableColumnGroupConcept Columns, SourceInfoConcept Src>
    struct IsQueryAble<QueryAble<Columns, Src> > : std::true_type {
    };

    template<typename T>
    concept ConvertToQueryAbleConcept = requires(T *p)
    {
        []<typename... Args>(const QueryAble<Args...> *) {
        }(p);
    } || IsQueryAble<T>::value;

    template<typename T>
    concept ResultColumnConcept = TableColumnConcept<T> || AggregateFunctionConcept<T>;

    template<ColumnOrTableColumnGroupConcept Columns, SourceInfoConcept Src>
    class QueryAble {
    public:
        using columns = Columns;
        using Source = Src;

    protected:
        SQLiteWrapper &_sqlite;
        const Source _source;

        template<typename _Where, typename _GroupBy, ResultColumnConcept... ResultColumns>
        class SelectStatement {
            const SQLiteWrapper &_sqlite;
            const Source &_source;
            _Where _where;
            //TODO 支援express limit offset
            std::optional<std::pair<int, int> > _limitOffset;
            bool _isDistinct;

        public:
            explicit SelectStatement(const SQLiteWrapper &sqlite, const Source &source,
                                     _Where where,
                                     const std::optional<std::pair<int, int> > &limitOffset = std::nullopt,
                                     bool isDistinct = false) : _sqlite(sqlite),
                                                                _source(source), _where(where),
                                                                _limitOffset(limitOffset), _isDistinct(isDistinct) {
            }

            template<ConditionConcept Cond>
            auto Where(const Cond &condition) {
                return SelectStatement<Cond, nullptr_t, ResultColumns...>(
                    _sqlite, _source, condition, _limitOffset, _isDistinct);
            }

            SelectStatement &LimitOffset(int limit, int offset = 0) {
                _limitOffset = std::make_pair(limit, offset);
                return *this;
            }

            SelectStatement &Distinct() {
                _isDistinct = true;
                return *this;
            }

            template<ColumnOrTableColumnConcept Column>
            auto GroupBy() {
                return SelectStatement<_Where, Column, ResultColumns...>(
                    _sqlite, _source, _where, _limitOffset, _isDistinct);
            }

            //TODO 支援迭代器模式
            std::vector<std::tuple<ResultColumns...> > Results() {
                auto sql = std::string("SELECT ") + (_isDistinct ? "DISTINCT " : "") + GetColumnNames<ResultColumns
                               ...>() + " FROM " + MakeSourceSQL(_source);
                if constexpr (!std::is_null_pointer_v<_Where>) {
                    sql += " WHERE " + _where.condition;
                }
                if constexpr (!std::is_null_pointer_v<_GroupBy>) {
                    sql += " GROUP BY " + GetColumnName<_GroupBy>();
                }
                if (_limitOffset.has_value()) {
                    sql += " LIMIT " + std::to_string(_limitOffset->first);
                    if (_limitOffset->second > 0) {
                        sql += " OFFSET " + std::to_string(_limitOffset->second);
                    }
                }
                sql += ";";
                auto all_params = [&]() {
                    if constexpr (std::is_null_pointer_v<_Where>) {
                        return ExtractSourceParams(_source);
                    } else {
                        return std::tuple_cat(ExtractSourceParams(_source), _where.params);
                    }
                }();
                return std::apply([this, &sql](auto &&... params) {
                    return _sqlite.Query<ResultColumns...>(sql, params...);
                }, all_params);
            }
        };

    public:
        explicit QueryAble(SQLiteWrapper &sqlite, Source source) : _sqlite(sqlite), _source(source) {
        }

        virtual ~QueryAble() = default;

        template<ResultColumnConcept... ResultCol>
        auto Select() const {
            //TODO 支援聚合暫時關閉檢查
            //static_assert(IsTypeGroupSubset<TypeGroup<ResultCol...>, columns>(),"ResultCol must be subset of table columns");
            return SelectStatement<nullptr_t, nullptr_t, ResultCol...>(_sqlite, _source, nullptr);
        }

        template<ConvertToQueryAbleConcept Table2, ConditionConcept Cond>
        auto FullJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::FULL,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<ConvertToQueryAbleConcept Table2, ConditionConcept Cond>
        auto InnerJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::INNER,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<ConvertToQueryAbleConcept Table2, ConditionConcept Cond>
        auto LeftJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::LEFT,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<ConvertToQueryAbleConcept Table2, ConditionConcept Cond>
        auto RightJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Cond>{
                                            .type = JoinType::RIGHT,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<ConvertToQueryAbleConcept Table2, ConditionConcept Cond>
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

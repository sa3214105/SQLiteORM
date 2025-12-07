#pragma once
#include <cmath>

#include "../SQLiteWrapper.hpp"
#include "DataSource.h"
#include "Column.hpp"
#include "Expr.hpp"
#include "AggregateFunctions.hpp"

namespace TypeSQLite {
    template<typename Cols, SourceInfoConcept Src>
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
    concept ResultColumnConcept = ExprConcept<T>/*TableColumnConcept<T> || AggregateFunctionConcept<T>*/;

    template<typename Cols, SourceInfoConcept Source>
    class QueryAble {
    public:
        const Cols columns;

    protected:
        SQLiteWrapper &_sqlite;
        const Source _source;

        template<typename _Where, typename _GroupBy, typename _OrderExpr, typename... ResultColumns>
        class SelectStatement {
            const SQLiteWrapper &_sqlite;
            const Source &_source;
            _Where _where;
            _GroupBy _groupBy;
            _OrderExpr _orderExpr;
            OrderType _orderType = OrderType::ASC;
            std::tuple<ResultColumns...> _resultColumns;
            //TODO 支援express limit offset
            std::optional<std::pair<int, int> > _limitOffset;
            bool _isDistinct;

            auto GetResultColumnParamTuple() const {
                return std::apply([&](auto &&... columns) {
                    return std::tuple_cat(([]<typename T>(const T &col) {
                        return col.params;
                    }(columns))...);
                }, _resultColumns);
            }

            auto GetWhereParamTuple() const {
                if constexpr (std::is_null_pointer_v<_Where>) {
                    return std::tuple();
                } else {
                    return _where.params;
                }
            }

            auto GetGroupByParamTuple() const {
                if constexpr (std::is_null_pointer_v<_GroupBy>) {
                    return std::tuple();
                } else {
                    return std::apply([](auto &&... expr) { return GetExprParamTuple(expr...); },
                                      _groupBy);
                }
            }

            auto GetOrderParamTuple() const {
                if constexpr (std::is_null_pointer_v<_OrderExpr>) {
                    return std::tuple();
                } else {
                    return _orderExpr.params;
                }
            }

        public:
            explicit SelectStatement(
                const SQLiteWrapper &sqlite,
                const Source &source,
                _Where where,
                _GroupBy groupBy,
                _OrderExpr orderExpr,
                const OrderType &orderType,
                const std::optional<std::pair<int, int> > &limitOffset,
                bool isDistinct, ResultColumns... columns) : _sqlite(sqlite),
                                                             _source(source),
                                                             _where(where),
                                                             _groupBy(groupBy),
                                                             _orderExpr(orderExpr),
                                                             _orderType(orderType),
                                                             _resultColumns(columns...),
                                                             _limitOffset(limitOffset),
                                                             _isDistinct(isDistinct) {
            }

            template<ExprConcept Expr>
            auto Where(const Expr &expr) {
                return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    return SelectStatement<Expr, _GroupBy, _OrderExpr, ResultColumns...>(
                        _sqlite, _source, expr, _groupBy, _orderExpr, _orderType, _limitOffset, _isDistinct,
                        std::get<Is>(_resultColumns)...);
                }(std::index_sequence_for<ResultColumns...>{});
            }

            SelectStatement &LimitOffset(int limit, int offset = 0) {
                _limitOffset = std::make_pair(limit, offset);
                return *this;
            }

            SelectStatement &Distinct() {
                _isDistinct = true;
                return *this;
            }

            template<ExprConcept... Exprs>
            auto GroupBy(Exprs... exprs) {
                return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    return SelectStatement<_Where, std::tuple<Exprs...>, _OrderExpr, ResultColumns...>(
                        _sqlite, _source, _where, std::make_tuple(exprs...), _orderExpr, _orderType, _limitOffset,
                        _isDistinct,
                        std::get<Is>(_resultColumns)...);
                }(std::index_sequence_for<ResultColumns...>{});
            }

            template<ExprConcept Expr>
            auto OrderBy(Expr expr, OrderType order = OrderType::ASC) {
                return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    return SelectStatement<_Where, _GroupBy, Expr, ResultColumns...>(
                        _sqlite, _source, _where, _groupBy, expr, order, _limitOffset, _isDistinct,
                        std::get<Is>(_resultColumns)...);
                }(std::index_sequence_for<ResultColumns...>{});
            }

            //TODO 支援迭代器模式
            auto Results() {
                auto sql = std::string("SELECT ") + (_isDistinct ? "DISTINCT " : "") + std::apply(
                               GetColumnNames<ResultColumns
                                   ...>, _resultColumns) + " FROM " + MakeSourceSQL(_source);
                if constexpr (!std::is_null_pointer_v<_Where>) {
                    sql += " WHERE " + _where.sql;
                }
                if constexpr (!std::is_null_pointer_v<_GroupBy>) {
                    sql += " GROUP BY " + std::apply([](auto &&... expr) { return MakeExprList(expr...); }, _groupBy);
                }
                if constexpr (!std::is_null_pointer_v<_OrderExpr>) {
                    sql += " ORDER BY " + _orderExpr.sql + " " + OrderTypeToString(_orderType);
                }
                if (_limitOffset.has_value()) {
                    sql += " LIMIT " + std::to_string(_limitOffset->first);
                    if (_limitOffset->second > 0) {
                        sql += " OFFSET " + std::to_string(_limitOffset->second);
                    }
                }
                sql += ";";
                auto all_params = std::tuple_cat(
                    GetResultColumnParamTuple(),
                    GetExtractSourceParams(_source),
                    GetWhereParamTuple(),
                    GetGroupByParamTuple(),
                    GetOrderParamTuple()
                );
                return std::apply([this, &sql](auto &&... params) {
                    return _sqlite.Query<ExprResultValueType<ResultColumns>...>(sql, params...);
                }, all_params);
            }
        };

    public:
        explicit QueryAble(SQLiteWrapper &sqlite, Cols columns, Source source) : columns(columns), _sqlite(sqlite),
            _source(source) {
        }

        virtual ~QueryAble() = default;

        template<typename... ResultCol>
        auto Select(ResultCol... resultCols) const {
            //TODO 支援聚合暫時關閉檢查
            //static_assert(IsTypeGroupSubset<TypeGroup<ResultCol...>, columns>(),"ResultCol must be subset of table columns");
            return SelectStatement<nullptr_t, nullptr_t, nullptr_t, ResultCol...>(
                _sqlite, _source, nullptr, nullptr, nullptr, OrderType::ASC, std::nullopt,
                false,
                resultCols...);
        }

        template<ConvertToQueryAbleConcept Table2, ExprConcept Expr>
        auto FullJoin(const Table2 &table2, const Expr &expr) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Expr>{
                                            .type = JoinType::FULL,
                                            .condition = expr
                                        });
            auto newColumns = std::tuple_cat(columns, table2.columns);
            using NewCols = decltype(newColumns);
            using NewSource = decltype(newSource);
            return QueryAble<NewCols, NewSource>(this->_sqlite, newColumns, newSource);
        }

        template<ConvertToQueryAbleConcept Table2, ExprConcept Expr>
        auto InnerJoin(const Table2 &table2, const Expr &expr) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Expr>{
                                            .type = JoinType::INNER,
                                            .condition = expr
                                        });
            auto newColumns = std::tuple_cat(columns, table2.columns);
            using NewCols = decltype(newColumns);
            using NewSource = decltype(newSource);
            return QueryAble<NewCols, NewSource>(this->_sqlite, newColumns, newSource);
        }

        template<ConvertToQueryAbleConcept Table2, ExprConcept Expr>
        auto LeftJoin(const Table2 &table2, const Expr &expr) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Expr>{
                                            .type = JoinType::LEFT,
                                            .condition = expr
                                        });
            auto newColumns = std::tuple_cat(columns, table2.columns);
            using NewCols = decltype(newColumns);
            using NewSource = decltype(newSource);
            return QueryAble<NewCols, NewSource>(this->_sqlite, newColumns, newSource);
        }

        template<ConvertToQueryAbleConcept Table2, ExprConcept Expr>
        auto RightJoin(const Table2 &table2, const Expr &expr) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Expr>{
                                            .type = JoinType::RIGHT,
                                            .condition = expr
                                        });
            auto newColumns = std::tuple_cat(columns, table2.columns);
            using NewCols = decltype(newColumns);
            using NewSource = decltype(newSource);
            return QueryAble<NewCols, NewSource>(this->_sqlite, newColumns, newSource);
        }

        template<ConvertToQueryAbleConcept Table2, ExprConcept Expr>
        auto CrossJoin(const Table2 &table2, const Expr &expr) const {
            auto newSource = JoinSource(this->_source, DataSource<Table2, Expr>{
                                            .type = JoinType::CROSS,
                                            .condition = expr
                                        });
            auto newColumns = std::tuple_cat(columns, table2.columns);
            using NewCols = decltype(newColumns);
            using NewSource = decltype(newSource);
            return QueryAble<NewCols, NewSource>(this->_sqlite, newColumns, newSource);
        }
    };
}

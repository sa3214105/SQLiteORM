#pragma once
#include "../SQLiteWrapper.hpp"
#include "DataSource.h"
#include "Column.hpp"
#include "Condition.hpp"

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
        []<typename... Args>(const QueryAble<Args...> *) {}(p);
    } || IsQueryAble<T>::value;


    template<ColumnOrTableColumnGroupConcept Columns, SourceInfoConcept Src>
    class QueryAble {
    public:
        using columns = Columns;
        using Source = Src;

    protected:
        SQLiteWrapper &_sqlite;
        const Source _source;

        template<TableColumnConcept... ResultColumns>
        class SelectQuery {
            const SQLiteWrapper &_sqlite;
            const Source &_source;
            std::string _basic_sql;
            std::function<std::vector<std::tuple<ResultColumns...> >()> _query_func;

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

            template<ConditionConcept Cond>
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

            //TODO 支援迭代器模式
            std::vector<std::tuple<ResultColumns...> > Results() {
                return _query_func();
            }
        };

    public:
        explicit QueryAble(SQLiteWrapper &sqlite, Source source) : _sqlite(sqlite), _source(source) {
        }

        virtual ~QueryAble() = default;

        template<TableColumnConcept... ResultCol>
        auto Select() const {
            static_assert(IsTypeGroupSubset<TypeGroup<ResultCol...>, columns>(),
                          "ResultCol must be subset of table columns");
            return SelectQuery<ResultCol...>(_sqlite, _source);
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

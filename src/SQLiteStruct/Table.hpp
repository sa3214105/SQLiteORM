#pragma once
#include "TableConstraint.hpp"

namespace TypeSQLite {
    // Helper to filter only columns from a parameter pack
    template<typename... Items>
    struct FilterColumns;

    template<>
    struct FilterColumns<> {
        using type = TypeGroup<>;
    };

    template<typename T, typename... Rest>
    struct FilterColumns<T, Rest...> {
        using type = std::conditional_t<
            ColumnConcept<T>,
            typename ConcatTypeGroup<TypeGroup<T>, typename FilterColumns<Rest...>::type>::type,
            typename FilterColumns<Rest...>::type
        >;
    };

    // Helper to filter only table constraints from a parameter pack
    template<typename... Items>
    struct FilterTableConstraints;

    template<>
    struct FilterTableConstraints<> {
        using type = TypeGroup<>;
    };

    template<typename T, typename... Rest>
    struct FilterTableConstraints<T, Rest...> {
        using type = std::conditional_t<
            TableConstraintConcept<T>,
            typename ConcatTypeGroup<TypeGroup<T>, typename FilterTableConstraints<Rest...>::type>::type,
            typename FilterTableConstraints<Rest...>::type
        >;
    };

    // Helper to filter only table options from a parameter pack
    template<typename... Items>
    struct FilterTableOptions;

    template<>
    struct FilterTableOptions<> {
        using type = TypeGroup<>;
    };

    template<typename T, typename... Rest>
    struct FilterTableOptions<T, Rest...> {
        using type = std::conditional_t<
            TableOptionConcept<T>,
            typename ConcatTypeGroup<TypeGroup<T>, typename FilterTableOptions<Rest...>::type>::type,
            typename FilterTableOptions<Rest...>::type
        >;
    };

    // Helper to extract column types from TypeGroup as parameter pack
    template<typename TG>
    struct TypeGroupToPackHelper;

    template<typename... Ts>
    struct TypeGroupToPackHelper<TypeGroup<Ts...> > {
        using type = TypeGroup<Ts...>;
    };

    template<ColumnOrTableColumnConcept T, ColumnOrTableColumnConcept... Ts>
    std::string GetUpdateField() {
        if constexpr (sizeof...(Ts) == 0) {
            return std::string(std::string(T::name) + " = ?");
        } else {
            return std::string(std::string(T::name) + " = ?, " + GetUpdateField<Ts...>());
        }
    }

    // Helper to generate table constraint SQL from TypeGroup
    template<typename ConstraintsTG>
    std::string GetTableConstraintSQL() {
        if constexpr (std::is_same_v<ConstraintsTG, TypeGroup<> >) {
            return "";
        } else {
            using CurrentConstraint = typename ConstraintsTG::type;
            std::string result = ", " + std::string(CurrentConstraint::value);
            if constexpr (!std::is_same_v<typename ConstraintsTG::next, TypeGroup<> >) {
                result += GetTableConstraintSQL<typename ConstraintsTG::next>();
            }
            return result;
        }
    }

    // Helper to generate table options SQL from TypeGroup (applied after closing parenthesis)
    template<typename OptionsTG>
    std::string GetTableOptionsSQL() {
        if constexpr (std::is_same_v<OptionsTG, TypeGroup<> >) {
            return "";
        } else {
            using CurrentOption = typename OptionsTG::type;
            std::string result = std::string(CurrentOption::value);
            if constexpr (!std::is_same_v<typename OptionsTG::next, TypeGroup<> >) {
                result += "," + GetTableOptionsSQL<typename OptionsTG::next>();
            }
            return result;
        }
    }

    // Helper to generate column definitions from TypeGroup of columns
    template<typename ColumnsTG>
    std::string GetColumnDefinitionsFromTypeGroup() {
        if constexpr (std::is_same_v<ColumnsTG, TypeGroup<> >) {
            return "";
        } else {
            using CurrentColumn = typename ColumnsTG::type;
            std::string result = GetColumnDefinition<CurrentColumn>();
            if constexpr (!std::is_same_v<typename ColumnsTG::next, TypeGroup<> >) {
                result += "," + GetColumnDefinitionsFromTypeGroup<typename ColumnsTG::next>();
            }
            return result;
        }
    }

    // Forward declaration
    template<FixedString TableName, ColumnOrTableConstraintOrOptionConcept... Items>
    class Table;

    // Helper to generate TableColumn TypeGroup from filtered columns
    template<typename TableType, typename ColumnsTG>
    struct GenerateTableColumns;

    template<typename TableType, typename... Columns>
    struct GenerateTableColumns<TableType, TypeGroup<Columns...> > {
        using type = TypeGroup<TableColumn_Base<TableType, Columns>...>;
    };

    template<FixedString TableName, ColumnOrTableConstraintOrOptionConcept... Items>
    class Table final : public QueryAble<
                typename GenerateTableColumns<
                    Table<TableName, Items...>,
                    typename FilterColumns<Items...>::type
                >::type,
                SourceInfo<Table<TableName, Items...> > > {
    private:
        using FilteredColumns = typename FilterColumns<Items...>::type;
        using FilteredConstraints = typename FilterTableConstraints<Items...>::type;
        using FilteredOptions = typename FilterTableOptions<Items...>::type;

    public:
        template<ColumnConcept Col>
        using TableColumn = TableColumn_Base<Table, Col>;
        constexpr static FixedString name = TableName;
        using columns = typename GenerateTableColumns<Table, FilteredColumns>::type;
        using Source = SourceInfo<Table>;

    private:
        SQLiteWrapper &_sqlite;

        template<typename _Where, TableColumnConcept... Ts>
        class UpdateStatement {
            const Table &_table;
            std::tuple<ExprResultValueType<Ts>...> datas;
            _Where _where;

        public:
            explicit UpdateStatement(_Where where, const Table &table, ExprResultValueType<Ts>... ts) : _table(table),
                datas(ts...),
                _where(where) {
            }

            template<ExprConcept Expr>
            auto Where(const Expr &expr) {
                return std::apply([&](auto &&... params) {
                    return UpdateStatement<Expr, Ts...>(expr, _table, params...);
                }, datas);
            }

            void Execute() {
                auto sql = std::string("UPDATE ") + std::string(name) + " SET " + GetUpdateField<Ts...>();
                if constexpr (!std::is_null_pointer_v<_Where>) {
                    sql += " WHERE " + _where.sql;
                }
                sql += ";";
                auto all_params = [&]() {
                    if constexpr (std::is_null_pointer_v<_Where>) {
                        return datas;
                    } else {
                        return std::tuple_cat(datas, _where.params);
                    }
                }();
                return std::apply([this, &sql](auto &&... params) {
                    return _table._sqlite.Execute(sql, params...);
                }, all_params);
            }
        };

        template<typename _Where>
        class DeleteStatement {
            const Table &_table;
            _Where _where;

        public:
            explicit DeleteStatement(_Where where, const Table &table) : _table(table), _where(where) {
            }

            template<ExprConcept Expr>
            auto Where(const Expr &expr) {
                return DeleteStatement<Expr>(expr, _table);
            }

            void Execute() {
                auto sql = std::string("DELETE FROM ") + std::string(name);
                if constexpr (!std::is_null_pointer_v<_Where>) {
                    sql += " WHERE " + _where.sql;
                }
                sql += ";";
                auto all_params = [&]() {
                    if constexpr (std::is_null_pointer_v<_Where>) {
                        return std::make_tuple();
                    } else {
                        return _where.params;
                    }
                }();
                return std::apply([this, &sql](auto &&... params) {
                    return _table._sqlite.Execute(sql, params...);
                }, all_params);
            }
        };

    public:
        explicit Table(SQLiteWrapper &sqlite) : QueryAble<columns, Source>(sqlite, Source()),
                                                _sqlite(sqlite) {
            std::string sql = std::string("CREATE TABLE IF NOT EXISTS ") + std::string(name) + " (";
            sql += GetColumnDefinitionsFromTypeGroup<FilteredColumns>();
            sql += GetTableConstraintSQL<FilteredConstraints>();
            sql += ")";
            sql += GetTableOptionsSQL<FilteredOptions>();
            sql += ";";
            sqlite.Execute(sql);
        }

        //TODO 支援批量插入
        template<TableColumnConcept... U>
        void Insert(ExprResultValueType<U>... values) {
            static_assert(IsTypeGroupSubset<TypeGroup<U...>, columns>(),
                          "Insert values must be subset of table columns");
            std::string sql = std::string("INSERT INTO ") + std::string(name) + " (";
            sql += GetColumnNamesWithOutTableName<U...>();
            sql += ") VALUES (";
            for (auto i = 0; i < sizeof...(U); ++i) {
                sql += "?, ";
            }
            if constexpr (sizeof...(U) > 0) {
                sql.pop_back(); // 去掉最後一個空格
                sql.pop_back(); // 去掉最後一個逗號
            }
            sql += ");";

            _sqlite.Execute(sql, values...);
        }

        // 批量插入支援
        template<TableColumnConcept... U>
        void InsertMany(const std::vector<std::tuple<ExprResultValueType<U>...> > &rows) {
            static_assert(IsTypeGroupSubset<TypeGroup<U...>, columns>(),
                          "Insert values must be subset of table columns");

            if (rows.empty()) {
                return;
            }

            // 使用 SQLiteWrapper::Transaction 來提高批量插入效能
            SQLiteWrapper::Transaction transaction(_sqlite);

            // 準備 SQL 語句
            std::string sql = std::string("INSERT INTO ") + std::string(name) + " (";
            sql += GetColumnNamesWithOutTableName<U...>();
            sql += ") VALUES (";
            for (size_t j = 0; j < sizeof...(U); ++j) {
                sql += "?, ";
            }
            sql.pop_back(); // 去掉最後一個空格
            sql.pop_back(); // 去掉最後一個逗號
            sql += ");";

            // 對每一行執行插入
            for (const auto &row: rows) {
                std::apply([this, &sql](auto &&... values) {
                    _sqlite.Execute(sql, values...);
                }, row);
            }

            // Transaction 解構子會自動 Commit（如果沒有異常）或 Rollback（如果有異常）
        }

    public:
        template<TableColumnConcept... U>
        auto Update(ExprResultValueType<U>... values) {
            static_assert(IsTypeGroupSubset<TypeGroup<U...>, columns>(),
                          "Update values must be subset of table columns");
            return UpdateStatement<nullptr_t,U...>(nullptr, *this, values...);
        }

        auto Delete() {
            return DeleteStatement<nullptr_t>(nullptr, *this);
        }
    };

    template<typename>
    struct IsTable : std::false_type {
    };

    template<FixedString TableName, ColumnOrTableConstraintOrOptionConcept... Items>
    struct IsTable<Table<TableName, Items...> > : std::true_type {
    };

    template<typename T>
    concept TableConcept = IsTable<T>::value;
}

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
    // template<typename... Items>
    // struct FilterTableConstraints;
    //
    // template<>
    // struct FilterTableConstraints<> {
    //     using type = TypeGroup<>;
    // };
    //
    // template<typename T, typename... Rest>
    // struct FilterTableConstraints<T, Rest...> {
    //     using type = std::conditional_t<
    //         TableConstraintConcept<T>,
    //         typename ConcatTypeGroup<TypeGroup<T>, typename FilterTableConstraints<Rest...>::type>::type,
    //         typename FilterTableConstraints<Rest...>::type
    //     >;
    // };
    //
    // // Helper to filter only table options from a parameter pack
    // template<typename... Items>
    // struct FilterTableOptions;
    //
    // template<>
    // struct FilterTableOptions<> {
    //     using type = TypeGroup<>;
    // };
    //
    // template<typename T, typename... Rest>
    // struct FilterTableOptions<T, Rest...> {
    //     using type = std::conditional_t<
    //         TableOptionConcept<T>,
    //         typename ConcatTypeGroup<TypeGroup<T>, typename FilterTableOptions<Rest...>::type>::type,
    //         typename FilterTableOptions<Rest...>::type
    //     >;
    // };

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

    template<typename T, typename... Ts>
    std::string GetTableConstraintSQLFromPack(T t, Ts... ts) {
        if constexpr (sizeof...(Ts) == 0) {
            return ", " + std::string(t.value);
        } else {
            return ", " + std::string(t.value) + GetTableConstraintSQLFromPack(ts...);
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

    template<typename T, typename... Ts>
    std::string GetTableOptionsSQLFromPack() {
        if constexpr (sizeof...(Ts) == 0) {
            return std::string(T::value);
        } else {
            return std::string(T::value) + "," + GetTableOptionsSQLFromPack<Ts...>();
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

    template<typename T, typename... Ts>
    std::string GetColumnDefinitionFromPack() {
        if constexpr (sizeof...(Ts) == 0) {
            return GetColumnDefinition<T>();
        } else {
            return GetColumnDefinition<T>() + "," + GetColumnDefinitionFromPack<Ts...>();
        }
    }

    template<
        FixedString Name,
        typename ColumTypes= std::tuple<>,
        typename TableConstraints= std::tuple<>,
        typename TableOptions=std::tuple<> >
    struct TableDefinition {
        constexpr static FixedString name = Name;
        ColumTypes columns;
        TableConstraints tableConstraints;
        TableOptions tableOptions;
    };

    template<FixedString Name, typename ColumTypes, typename TableConstraints = std::tuple<>, typename TableOptions =
        std::tuple<> >
    constexpr auto MakeTableDefinition(ColumTypes columns,
                                       TableConstraints tableConstraints = std::make_tuple(),
                                       TableOptions tableOptions = std::make_tuple()) {
        return TableDefinition<Name, ColumTypes, TableConstraints, TableOptions>{
            .columns = columns, .tableConstraints = tableConstraints, .tableOptions = tableOptions
        };
    }

    template<typename>
    struct IsTableDefinition : std::false_type {
    };

    template<FixedString Name, typename ColumTypes, typename TableConstraints, typename TableOptions>
    struct IsTableDefinition<TableDefinition<Name, ColumTypes, TableConstraints, TableOptions> > : std::true_type {
    };

    template<typename T>
    concept TableDefinitionConcept = IsTableDefinition<T>::value;

    // Helper to generate TableColumn TypeGroup from filtered columns
    template<typename TableType, typename ColumnsTG>
    struct GenerateTableColumns;

    template<typename TableType, typename... Columns>
    struct GenerateTableColumns<TableType, TypeGroup<Columns...> > {
        using type = TypeGroup<TableColumn_Base<TableType, Columns>...>;
    };

    template<typename TableDef>
    class Table final : public QueryAble<decltype(std::declval<TableDef>().columns), SourceInfo<Table<TableDef> > > {
    public:
        template<ColumnConcept Col>
        using TableColumn = TableColumn_Base<Table, Col>;
        constexpr static FixedString name = TableDef::name;
        TableDef _tableDef;
        const decltype(_tableDef.columns) columns;

    private:
        SQLiteWrapper &_sqlite;

        template<typename _Where, ColumnOrTableColumnConcept... Ts>
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
                auto sql = std::string("UPDATE ") + std::string(name) + " SET " + GetUpdateField<Ts
                               ...>();
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
        explicit Table(SQLiteWrapper &sqlite, TableDef table_def) : QueryAble<decltype(table_def.columns), SourceInfo<
                                                                        Table> >(sqlite, table_def.columns,
                                                                        SourceInfo<Table>()), _tableDef(table_def),
                                                                    columns(table_def.columns),
                                                                    _sqlite(sqlite) {
            std::string sql = std::string("CREATE TABLE IF NOT EXISTS ") + std::string(name) + " (";

            // 添加列定義
            sql += std::apply([](auto... cols) {
                                  if constexpr (sizeof ...(cols) == 0) {
                                      return "";
                                  } else {
                                      return GetColumnDefinitionFromPack<decltype(cols)...>();
                                  }
                              },
                              _tableDef.columns);

            // 添加表約束（GetTableConstraintSQLFromPack 已經包含前導逗號）
            sql += std::apply([](auto... tableConstraints) {
                                  if constexpr (sizeof...(tableConstraints) == 0) {
                                      return "";
                                  } else {
                                      return GetTableConstraintSQLFromPack(tableConstraints...);
                                  }
                              },
                              _tableDef.tableConstraints
            );

            sql += ")";

            // 添加表選項（在括號外面）
            sql += std::apply([](auto... tableOptions) {
                                  if constexpr (sizeof...(tableOptions) == 0) {
                                      return "";
                                  } else {
                                      return " " + GetTableOptionsSQLFromPack<decltype(tableOptions)...>();
                                  }
                              },
                              _tableDef.tableOptions);
            sql += ";";
            sqlite.Execute(sql);
        }

        //TODO 支援批量插入
        template<typename... U>
        void Insert(ExprResultValueType<U>... values) {
            // static_assert(IsTypeGroupSubset<TypeGroup<U...>, columns>(),
            //               "Insert values must be subset of table columns");
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
        template<typename... U>

        void InsertMany(const std::vector<std::tuple<ExprResultValueType<U>...> > &rows) {
            // static_assert(IsTypeGroupSubset<TypeGroup<U...>, columns>(),
            //               "Insert values must be subset of table columns");

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

        template<ColumnOrTableColumnConcept... U>
        auto Update(ExprResultValueType<U>... values) {
            // static_assert(IsTypeGroupSubset<TypeGroup<U...>, columns>(),
            //               "Update values must be subset of table columns");
            return UpdateStatement<nullptr_t, U...>(nullptr, *this, values...);
        }

        auto Delete() {
            return DeleteStatement<nullptr_t>(nullptr, *this);
        }

        template<typename Column>
        auto operator[](Column column) {
            return TableColumn<Column>();
        }
    };

    template<typename>
    struct IsTable : std::false_type {
    };

    template<TableDefinitionConcept TableDef>
    struct IsTable<Table<TableDef> > : std::true_type {
    };

    template<typename T>
    concept TableConcept = IsTable<T>::value;
}

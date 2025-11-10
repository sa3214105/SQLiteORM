#pragma once
namespace SQLiteHelper {
    template<typename T, typename... Ts>
    std::string GetUpdateField() {
        if constexpr (sizeof...(Ts) == 0) {
            return std::string(std::string(T::name) + " = ?");
        } else {
            return std::string(std::string(T::name) + " = ?, " + GetUpdateField<Ts...>());
        }
    }

    template<FixedString TableName, typename... Columns>
    class Table final : public QueryAble<typeGroup<TableColumn_Base<
                Table<TableName, Columns...>, Columns>...>, SourceInfo<Table<TableName, Columns...> > > {
    public:
        template<typename Col>
        using TableColumn = TableColumn_Base<Table, Col>;
        constexpr static FixedString name = TableName;
        using columns = typeGroup<TableColumn<Columns>...>;
        using Source = SourceInfo<Table>;

    private:
        SQLiteWrapper &_sqlite;

        template<typename... Ts>
        class UpdateQuery {
            const Table &_table;
            std::tuple<Ts...> datas;
            std::string _basic_sql;
            std::string _where_sql;
            std::function<void()> _executor;

        public:
            explicit UpdateQuery(const Table &table, Ts... ts) : _table(table), datas(std::forward<Ts>(ts)...) {
                _basic_sql = std::string("UPDATE ") + std::string(name) + " SET " + GetUpdateField<Ts...>();
                _executor = [this]() {
                    _table._sqlite.Execute(_basic_sql + ";", std::get<Ts>(datas)...);
                };
            }

            template<typename Condition>
            UpdateQuery &Where() {
                _where_sql = std::string(" WHERE ") + std::string(Condition::condition);
                _executor = [this]() {
                    _table._sqlite.Execute(_basic_sql + _where_sql + ";", std::get<Ts>(datas)...);
                };
                return *this;
            }

            template<typename Cond>
            UpdateQuery &Where(const Cond &condition) {
                _where_sql = " WHERE " + condition.condition;
                _executor = [this, condition]() {
                    auto sql = _basic_sql + _where_sql + ";";
                    // 合併 update 的參數和 where 的參數
                    auto combined_params = std::tuple_cat(datas, condition.params);
                    std::apply([this, &sql](auto &&... params) {
                        _table._sqlite.Execute(sql, params...);
                    }, combined_params);
                };
                return *this;
            }

            void Execute() {
                _executor();
            }
        };

        class DeleteQuery {
            const Table &_table;
            std::string _basic_sql;
            std::string _where_sql;
            std::function<void()> _executor;

        public:
            explicit DeleteQuery(const Table &table) : _table(table) {
                _basic_sql = std::string("DELETE FROM ") + std::string(name);
                _executor = [this]() {
                    _table._sqlite.Execute(_basic_sql + ";");
                };
            }

            template<typename Condition>
            DeleteQuery &Where() {
                _where_sql = std::string(" WHERE ") + std::string(Condition::condition);
                _executor = [this]() {
                    _table._sqlite.Execute(_basic_sql + _where_sql + ";");
                };
                return *this;
            }

            template<typename Cond>
            DeleteQuery &Where(const Cond &condition) {
                _where_sql = " WHERE " + condition.condition;
                _executor = [this, condition]() {
                    auto sql = _basic_sql + _where_sql + ";";
                    std::apply([this, &sql](auto &&... params) {
                        _table._sqlite.Execute(sql, params...);
                    }, condition.params);
                };
                return *this;
            }

            void Execute() {
                _executor();
            }
        };

    public:
        explicit Table(SQLiteWrapper &sqlite) : QueryAble<typeGroup<TableColumn<Columns>...>, Source>(sqlite, Source()),
                                                _sqlite(sqlite) {
            std::string sql = std::string("CREATE TABLE IF NOT EXISTS ") + std::string(name) + " (";
            sql += GetColumnDefinitions<Columns...>();
            sql += ");";
            sqlite.Execute(sql);
        }

        template<typename... U>
        void Insert(U... values) {
            static_assert(isTypeGroupSubset<typeGroup<U...>, columns>(),
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

        template<typename... U>
        auto Update(U... values) {
            static_assert(isTypeGroupSubset<typeGroup<U...>, columns>(),
                          "Update values must be subset of table columns");
            return UpdateQuery(*this, std::forward<U>(values)...);
        }

        auto Delete() {
            return DeleteQuery(*this);
        }

        template<typename U>
        static TableColumn<U> MakeTableColumn(const decltype(std::declval<U>().value) &v) {
            auto ret = TableColumn<U>();
            ret.value = v;
            return ret;
        }
    };
}

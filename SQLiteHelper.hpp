#pragma once
#include "sqlite3.h"
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <type_traits>
#include <string_view>
#include <stdexcept>
#include <utility>
#include <filesystem>

namespace SQLiteHelper {
    template<size_t N = 0>
    struct FixedString {
        char value[N];

        constexpr FixedString(const char (&str)[N]) {
            for (size_t i = 0; i < N; ++i)
                value[i] = str[i];
        }

        constexpr operator std::string_view() const {
            return {value, N - 1};
        }

        operator const char *() const {
            return value;
        }

        operator const std::string() const {
            return value;
        }
    };

    template<std::size_t N1, std::size_t N2>
    constexpr auto operator+(const FixedString<N1> &a, const FixedString<N2> &b) {
        char result[N1 + N2 - 1]{};
        std::copy_n(a.value, N1 - 1, result);
        std::copy_n(b.value, N2, result + N1 - 1);
        return FixedString<N1 + N2 - 1>(result);
    }

    template<std::size_t N1, std::size_t N2>
    constexpr auto operator+(const FixedString<N1> &a, const char (&b)[N2]) {
        char result[N1 + N2 - 1]{};
        std::copy_n(a.value, N1 - 1, result);
        std::copy_n(b, N2, result + N1 - 1);
        return FixedString<N1 + N2 - 1>(result);
    }

    template<std::size_t N1, std::size_t N2>
    constexpr auto operator+(const char (&a)[N1], const FixedString<N2> &b) {
        char result[N1 + N2 - 1]{};
        std::copy_n(a, N1 - 1, result);
        std::copy_n(b.value, N2, result + N1 - 1);
        return FixedString<N1 + N2 - 1>(result);
    }

    enum class column_type {
        TEXT,
        NUMERIC,
        INTEGER,
        REAL,
        BLOB
    };

    struct column_constraint {
        bool primary_key = false;
        bool not_null = false;
        bool unique = false;
        std::string default_value;
    };

    template<typename T>
    concept ColumnConcept = requires()
    {
        { T::type } -> std::convertible_to<column_type>;
        { T::name } -> std::convertible_to<std::string>;
        { T::constraint } -> std::convertible_to<column_constraint>;
    };

    struct EmptyCond {
        constexpr static FixedString condition = " 1 ";
    };

    template<typename T1, typename T2>
    struct EqualCond {
        constexpr static FixedString condition = FixedString(" ") + T1::name + " = " + T2::name;
    };

    struct Condition {
        std::string condition;

        Condition &operator&&(const Condition &other) {
            condition = "(" + condition + ") AND (" + other.condition + ")";
            return *this;
        }

        Condition &operator||(const Condition &other) {
            condition = "(" + condition + ") OR (" + other.condition + ")";
            return *this;
        }
    };

    template<typename T>
    Condition Equal(const std::string &value) {
        return {.condition = std::string(" ") + std::string(T::name) + " = '" + value + "'"};
    }

    template<typename T1, typename T2>
    Condition Equal() {
        return {.condition = std::string(" ") + std::string(T1::name) + " = " + T2::name};
    }

    template<typename T>
    Condition NotEqual(const std::string &value) {
        return {.condition = std::string(" ") + std::string(T::name) + " != '" + value + "'"};
    }

    template<typename T1, typename T2>
    Condition NotEqual() {
        return {.condition = std::string(" ") + std::string(T1::name) + " != " + T2::name};
    }

    template<typename T>
    Condition GreaterThan(const std::string &value) {
        return {.condition = std::string(" ") + std::string(T::name) + " > '" + value + "'"};
    }

    template<typename T1, typename T2>
    Condition GreaterThan() {
        return {.condition = std::string(" ") + std::string(T1::name) + " > " + T2::name};
    }

    template<typename T>
    Condition LessThan(const std::string &value) {
        return {.condition = std::string(" ") + std::string(T::name) + " < '" + value + "'"};
    }

    template<typename T1, typename T2>
    Condition LessThan() {
        return {.condition = std::string(" ") + std::string(T1::name) + " < " + T2::name};
    }

    template<typename T>
    Condition GreaterThanEqual(const std::string &value) {
        return {.condition = std::string(" ") + std::string(T::name) + " >= '" + value + "'"};
    }

    template<typename T1, typename T2>
    Condition GreaterThanEqual() {
        return {.condition = std::string(" ") + std::string(T1::name) + " >= " + T2::name};
    }

    template<typename T>
    Condition LessThanEqual(const std::string &value) {
        return {.condition = std::string(" ") + std::string(T::name) + " <= '" + value + "'"};
    }

    template<typename T1, typename T2>
    Condition LessThanEqual() {
        return {.condition = std::string(" ") + std::string(T1::name) + " <= " + T2::name};
    }

    template<typename T, typename... Ts>
    struct typeGroup {
        using type = T;
        using next = typeGroup<Ts...>;
    };

    // 終止版本：最後一個型別
    template<typename T>
    struct typeGroup<T> {
        using type = T;
        using next = void; // 結尾時沒有下一個
    };

    template<typename T, typename TG>
    constexpr bool findTypeInTypeGroup() {
        if constexpr (std::is_same_v<T, typename TG::type>) {
            return true;
        } else if constexpr (!std::is_void_v<typename TG::next>) {
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
                if constexpr (!std::is_void_v<typename TG1::next>) {
                    return isTypeGroupSubset<typename TG1::next, TG2>();
                } else {
                    return true; // 已檢查完 TG1 的所有型別
                }
            } else {
                return false; // 找不到 TG1 的型別於 TG2 中
            }
        }
    }

    template<ColumnConcept T>
    std::string DefineColumnSQL() {
        std::string sql = T::name + " ";
        switch (T::type) {
            case column_type::TEXT:
                sql += "TEXT";
                break;
            case column_type::NUMERIC:
                sql += "NUMERIC";
                break;
            case column_type::INTEGER:
                sql += "INTEGER";
                break;
            case column_type::REAL:
                sql += "REAL";
                break;
            case column_type::BLOB:
                sql += "BLOB";
                break;
        }
        if (T::constraint.primary_key) {
            sql += " PRIMARY KEY";
        }
        if (T::constraint.not_null) {
            sql += " NOT NULL";
        }
        if (T::constraint.unique) {
            sql += " UNIQUE";
        }
        if (!T::constraint.default_value.empty()) {
            sql += " DEFAULT " + T::constraint.default_value;
        }
        return sql;
    }

    template<ColumnConcept... Columns>
    std::string GetColumnDefinitions() {
        std::string result;
        ((result += DefineColumnSQL<Columns>() + ","), ...);
        if (!result.empty()) result.pop_back(); // 去掉最後一個逗號
        return result;
    }


    template<typename T, typename... Ts>
    std::string GetNames() {
        if constexpr (sizeof...(Ts) == 0) {
            return std::string(T::name);
        } else {
            return std::string(T::name) + "," + GetNames<Ts...>();
        }
    }

    template<typename T, typename... Ts>
    std::string GetUpdateField() {
        if constexpr (sizeof...(Ts) == 0) {
            return std::string(std::string(T::name) + " = ?");
        } else {
            return std::string(std::string(T::name) + " = ?, " + GetUpdateField<Ts...>());
        }
    }

    template<typename T, typename... Ts>
    std::string GetFullJoin() {
        if constexpr (sizeof...(Ts) == 0) {
            return std::string(" FULL JOIN ") + std::string(T::table_name) + " ON " + std::string(T::join_condition);
        } else {
            return std::string(" FULL JOIN ") + std::string(T::table_name) + " ON " + std::string(T::join_condition) +
                   " " + GetFullJoin<Ts...>();
        }
    }

    template<typename T>
    auto GetValue(sqlite3_stmt *stmt, int colIndex) {
        T t;
        auto datatype = sqlite3_column_type(stmt, colIndex);
        if constexpr (T::type == column_type::TEXT) {
            if (datatype != SQLITE_TEXT) {
                if constexpr (std::is_convertible_v<nullptr_t, T>) {
                    t.value = nullptr;
                }
            } else {
                t.value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, colIndex));
            }
        } else if constexpr (T::type == column_type::NUMERIC) {
            if (datatype != SQLITE_INTEGER) {
                if constexpr (std::is_convertible_v<nullptr_t, T>) {
                    t.value = nullptr;
                }
            }else{
                t.value = sqlite3_column_type(stmt, colIndex);
            }

        } else if constexpr (T::type == column_type::INTEGER) {
            if (datatype != SQLITE_INTEGER) {
                if constexpr (std::is_convertible_v<nullptr_t, T>) {
                    t.value = nullptr;
                }
            }else{
                t.value = sqlite3_column_int(stmt, colIndex);
            }

        } else if constexpr (T::type == column_type::REAL) {
            if (datatype != SQLITE_FLOAT) {
                if constexpr (std::is_convertible_v<nullptr_t, T>) {
                    t.value = nullptr;
                }
            }else{
                t.value = sqlite3_column_double(stmt, colIndex);
            }

        } else if constexpr (T::type == column_type::BLOB) {
            if (datatype != SQLITE_BLOB) {
                if constexpr (std::is_convertible_v<nullptr_t, T>) {
                    t.value = nullptr;
                }
            }else {
                auto pBytes = static_cast<const uint8_t *>(sqlite3_column_blob(stmt, colIndex));
                auto n = static_cast<size_t>(sqlite3_column_bytes(stmt, colIndex));
                if (pBytes && n > 0) {
                    t.value = std::vector<uint8_t>(pBytes, pBytes + n);
                } else {
                    t.value.clear();
                }
            }

        }
        return t;
    }

    template<typename T, typename... Ts>
    std::tuple<T, Ts...> GetRowData(sqlite3_stmt *stmt, int colIndex = 0) {
        auto tuple = std::make_tuple(GetValue<T>(stmt, colIndex));
        if constexpr (sizeof...(Ts) == 0) {
            return tuple;
        } else {
            return std::tuple_cat(
                tuple,
                GetRowData<Ts...>(stmt, colIndex + 1)
            );
        }
    }

    template<typename T>
    void bindValue(sqlite3_stmt *stmt, int index, const T &value) {
        ;
        if constexpr (T::type == column_type::TEXT) {
            sqlite3_bind_text(stmt, index, value.value.c_str(), -1, SQLITE_STATIC);
        } else if constexpr (T::type == column_type::NUMERIC) {
            sqlite3_bind_double(stmt, index, static_cast<double>(value.value));
        } else if constexpr (T::type == column_type::INTEGER) {
            sqlite3_bind_int(stmt, index, value.value);
        } else if constexpr (T::type == column_type::REAL) {
            sqlite3_bind_double(stmt, index, value.value);
        } else if constexpr (T::type == column_type::BLOB) {
            sqlite3_bind_blob(stmt, index, value.value.data(), static_cast<int>(value.value.size()), SQLITE_STATIC);
        }
    }

    class Database_Base {
    public:
        std::string _db_path;
        std::unique_ptr<sqlite3, decltype(&sqlite3_close)> _dbPtr = {nullptr, sqlite3_close};

        Database_Base(const std::string &dbPath) : _db_path(dbPath) {
            // Delete existing DB file if present to ensure a clean database on open
            std::error_code ec;
            if (std::filesystem::exists(_db_path, ec)) {
                std::filesystem::remove(_db_path, ec);
            }
            sqlite3 *pDb = nullptr;
            int rc = sqlite3_open_v2(_db_path.c_str(), &pDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
            if (rc != SQLITE_OK) {
                throw std::runtime_error("Can't open database: " + std::string(sqlite3_errmsg(pDb)));
            }
            _dbPtr = {pDb, sqlite3_close};
        }

        virtual ~Database_Base() = default;
    };

    class QueryAble {
    protected:
        std::string from_sql;
        Database_Base &db;

        template<typename... ResultColumns>
        class SelectQuery {
            QueryAble &_query_able;
            std::string _basic_sql;
            const std::string _from_sql;
            std::string _where_sql;

        public:
            explicit SelectQuery(QueryAble &query_able) : _query_able(query_able),
                                                          _from_sql(" FROM " + query_able.from_sql) {
                _basic_sql = "SELECT " + GetNames<ResultColumns...>();
            }

            SelectQuery &Where(const Condition &condition) {
                _where_sql = " WHERE " + condition.condition;
                return *this;
            }

            std::vector<std::tuple<ResultColumns...> > Results() {
                sqlite3_stmt *stmt = nullptr;
                auto sql = _basic_sql + _from_sql + _where_sql + ";";
                if (sqlite3_prepare_v2(_query_able.db._dbPtr.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                    throw std::runtime_error(
                        std::string("Failed to prepare statement") + sqlite3_errmsg(_query_able.db._dbPtr.get()));
                }
                std::vector<std::tuple<ResultColumns...> > ret;
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    ret.push_back(GetRowData<ResultColumns...>(stmt));
                }
                if (sqlite3_finalize(stmt) != SQLITE_OK) {
                    throw std::runtime_error("Failed to finalize statement");
                }
                return ret;
            }
        };

    public:
        explicit QueryAble(Database_Base &db, const std::string &from_sql) : db(db), from_sql(from_sql) {
        }

        virtual ~QueryAble() = default;

        template<typename... ResultCol>
        auto Select() {
            return SelectQuery<ResultCol...>(*this);
        }
    };

    template<typename Table1, typename Table2, typename Condition>
    class FullJoinTable : public QueryAble {
    public:
        FullJoinTable(Database_Base &db) : QueryAble(
            db, Table1::name + FixedString(" FULL JOIN ") + Table2::name + FixedString(" ON ") + Condition::condition) {
        }
    };

    template<typename Table1, typename Table2, typename Condition>
    class InnerJoinTable : public QueryAble {
    public:
        InnerJoinTable(Database_Base &db) : QueryAble(
            db, Table1::name + FixedString(" INNER JOIN ") + Table2::name + FixedString(" ON ") + Condition::condition) {
        }
    };

    template<typename Table1, typename Table2, typename Condition>
    class LeftJoinTable : public QueryAble {
    public:
        LeftJoinTable(Database_Base &db) : QueryAble(
            db, Table1::name + FixedString(" LEFT JOIN ") + Table2::name + FixedString(" ON ") + Condition::condition) {
        }
    };

    template<typename Table1, typename Table2, typename Condition>
    class RightJoinTable : public QueryAble {
    public:
        RightJoinTable(Database_Base &db) : QueryAble(
            db, Table1::name + FixedString(" RIGHT JOIN ") + Table2::name + FixedString(" ON ") + Condition::condition) {
        }
    };

    template<typename Table1, typename Table2, typename Condition>
    class CrossJoinTable : public QueryAble {
    public:
        CrossJoinTable(Database_Base &db) : QueryAble(
            db, Table1::name + FixedString(" CROSS JOIN ") + Table2::name + FixedString(" ON ") + Condition::condition) {
        }
    };

    template<FixedString TableName, typename... Columns>
    class Table : public QueryAble {
    public:
        constexpr static FixedString name = TableName;
        using columns = typeGroup<Columns...>;

    private:
        Database_Base &db;

        template<typename... Ts>
        class UpdateQuery {
            const Table &_table;
            std::tuple<Ts...> datas;
            std::string _basic_sql;
            std::string _set_sql;
            std::string _where_sql;

        public:
            UpdateQuery(const Table &table, Ts... ts) : _table(table), datas(std::forward<Ts>(ts)...) {
                _basic_sql = std::string("UPDATE ") + std::string(name) + " SET " + GetUpdateField<Ts...>();
            }

            UpdateQuery &Where(const Condition &condition) {
                _where_sql = std::string(" WHERE ") + condition.condition;
                return *this;
            }

            void Execute() {
                sqlite3_stmt *stmt = nullptr;
                auto sql = _basic_sql + _where_sql + ";";
                if (sqlite3_prepare_v2(_table.db._dbPtr.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                    throw std::runtime_error("Failed to prepare statement");
                }

                int index = 1;
                std::apply([&index, &stmt](auto &... columns) {
                    ((bindValue(stmt, index++, columns)), ...);
                }, datas);

                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    auto errMsg = sqlite3_errmsg(_table.db._dbPtr.get());
                    sqlite3_finalize(stmt);
                    throw std::runtime_error("Failed to execute update statement: " + std::string(errMsg));
                }
                if (sqlite3_finalize(stmt) != SQLITE_OK) {
                    throw std::runtime_error("Failed to finalize statement");
                }
            }
        };

        class DeleteQuery {
            const Table &_table;
            std::string _basic_sql;
            std::string _where_sql;

        public:
            DeleteQuery(const Table &table) : _table(table) {
                _basic_sql = std::string("DELETE FROM ") + std::string(name);
            }

            DeleteQuery &Where(const Condition &condition) {
                _where_sql = std::string(" WHERE ") + condition.condition;
                return *this;
            }

            void Execute() {
                sqlite3_stmt *stmt = nullptr;
                auto sql = _basic_sql + _where_sql + ";";
                if (sqlite3_prepare_v2(_table.db._dbPtr.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                    throw std::runtime_error("Failed to prepare statement");
                }
                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    auto errMsg = sqlite3_errmsg(_table.db._dbPtr.get());
                    sqlite3_finalize(stmt);
                    throw std::runtime_error("Failed to execute delete statement: " + std::string(errMsg));
                }
                if (sqlite3_finalize(stmt) != SQLITE_OK) {
                    throw std::runtime_error("Failed to finalize statement");
                }
            }
        };

    public:
        explicit Table(Database_Base &db) : QueryAble(db, name), db(db) {
            std::string sql = std::string("CREATE TABLE IF NOT EXISTS ") + std::string(name) + " (";
            sql += GetColumnDefinitions<Columns...>();
            sql += ");";
            char *errMsg = nullptr;
            int rc = sqlite3_exec(db._dbPtr.get(), sql.c_str(), nullptr, nullptr, &errMsg);
            if (rc != SQLITE_OK) {
                std::string msg = std::string("SQL error (") + std::string(name) + "): ";
                msg += errMsg ? errMsg : "";
                sqlite3_free(errMsg);
                throw std::runtime_error(msg);
            }
        }

        template<typename T>
        struct TableColumn {
            constexpr static FixedString name = FixedString(Table::name) + FixedString(".") + T::name;
            constexpr static column_type type = T::type;
            inline static column_constraint constraint = T::constraint;
            decltype(T::value) value;
        };

        template<typename Table2, typename Condition>
        auto FullJoin() {
            return FullJoinTable<Table, Table2, Condition>(this->db);
        }

        template<typename Table2, typename Condition>
        auto InnerJoin() {
            return InnerJoinTable<Table, Table2, Condition>(this->db);
        }

        template<typename Table2, typename Condition>
        auto LeftJoin() {
            return LeftJoinTable<Table, Table2, Condition>(this->db);
        }

        template<typename Table2, typename Condition>
        auto RightJoin() {
            return RightJoinTable<Table, Table2, Condition>(this->db);
        }

        template<typename Table2, typename Condition>
        auto CrossJoin() {
            return CrossJoinTable<Table, Table2, Condition>(this->db);
        }

        template<typename... U>
        void Insert(U... values) {
            static_assert(isTypeGroupSubset<typeGroup<U...>, columns>);
            std::string sql = std::string("INSERT INTO ") + std::string(name) + " (";
            sql += GetNames<U...>();
            sql += ") VALUES (";
            for (auto i = 0; i < sizeof...(U); ++i) {
                sql += "?, ";
            }
            if constexpr (sizeof...(U) > 0) {
                sql.pop_back(); // 去掉最後一個空格
                sql.pop_back(); // 去掉最後一個逗號
            }
            sql += ");";

            sqlite3_stmt *stmt = nullptr;
            if (sqlite3_prepare_v2(db._dbPtr.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                throw std::runtime_error("Failed to prepare insert statement");
            }

            int index = 1;
            ((bindValue(stmt, index++, values)), ...);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                sqlite3_finalize(stmt);
                throw std::runtime_error("Failed to execute insert statement");
            }
            sqlite3_finalize(stmt);
        }

        template<typename... U>
        auto Update(U... values) {
            return UpdateQuery(*this, std::forward<U>(values)...);
        }

        auto Delete() {
            return DeleteQuery(*this);
        }
    };

    template<typename... Table>
    class Database : public Database_Base {
    public:
        class Transaction {
            friend class Database;

        private:
            Database &_db;
            bool _isCommittedOrRolledBack = false;

            Transaction(Database &db) : _db(db) {
                char *errMsg = nullptr;
                int rc = sqlite3_exec(_db._dbPtr.get(), "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
                if (rc != SQLITE_OK) {
                    std::string msg = errMsg ? errMsg : "Unknown error";
                    throw std::runtime_error("Failed to begin transaction: " + msg);
                }
            }

        public:
            ~Transaction() {
                if (_isCommittedOrRolledBack) {
                    return;
                }
                if (sqlite3_errcode(_db._dbPtr.get()) != SQLITE_OK) {
                    Rollback();
                }
                Commit();
            }

            void Rollback() {
                _isCommittedOrRolledBack = true;
                char *errMsg = nullptr;
                if (sqlite3_exec(_db._dbPtr.get(), "ROLLBACK;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
                    std::string msg = errMsg ? errMsg : "Unknown error";
                    throw std::runtime_error("Failed to commit transaction: " + msg);
                }
            }

            void Commit() {
                _isCommittedOrRolledBack = true;
                char *errMsg = nullptr;
                if (sqlite3_exec(_db._dbPtr.get(), "COMMIT;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
                    std::string msg = errMsg ? errMsg : "Unknown error";
                    throw std::runtime_error("Failed to commit transaction: " + msg);
                }
            }

            template<typename T>
            T &GetTable() {
                return std::get<T>(_db._tables);
            }
        };

    private:
        std::tuple<Table...> _tables;

        sqlite3 *openSQlite(bool removeExisting = true) {
            if (removeExisting) {
                std::error_code ec; // non-throwing removal
                if (std::filesystem::exists(_db_path, ec)) {
                    std::filesystem::remove(_db_path, ec);
                }
            }
            sqlite3 *pDb = nullptr;
            int rc = sqlite3_open_v2(_db_path.c_str(), &pDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
            if (rc != SQLITE_OK) {
                throw std::runtime_error("Can't open database: " + std::string(sqlite3_errmsg(pDb)));
            }
            return pDb;
        }

    public:
        explicit Database(const std::string &db_path) : Database_Base(db_path),
                                                        _tables(Table(*this)...) {
        }

        template<typename T>
        T &GetTable() {
            return std::get<T>(_tables);
        }

        Transaction CreateTransaction() {
            return Transaction(*this);
        }
    };
}

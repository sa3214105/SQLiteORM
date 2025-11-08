#pragma once
#include "sqlite3.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <tuple>
#include <type_traits>
#include <string_view>
#include <stdexcept>
#include <utility>
#include <filesystem>

namespace SQLiteHelper {
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

    template<size_t N = 0>
    struct FixedString {
        char value[N];

        constexpr FixedString(const char (&str)[N]) : value{} {
            for (size_t i = 0; i < N; ++i)
                value[i] = str[i];
        }

        constexpr operator std::string_view() const {
            return {value, N - 1};
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

    template<int num>
    constexpr auto toFixedString() {
        if constexpr (num < 0) {
            constexpr char arr[2] = {'0' + (num * -1 % 10), '\0'};
            if constexpr (num <= -10) {
                return "-" + toFixedString<-(num / 10)>() + FixedString(arr);
            } else {
                return "-" + FixedString(arr);
            }
        } else {
            char arr[2] = {'0' + (num % 10), '\0'};
            if constexpr (num >= 10) {
                return toFixedString<num / 10>() + FixedString(arr);
            } else {
                return FixedString(arr);
            }
        }
    }

    template<double Fraction>
    constexpr auto FractionToFixedString() {
        // 如果剩餘的小數部分非常小，就停止遞迴
        if constexpr (Fraction < std::numeric_limits<double>::epsilon()) {
            return FixedString("");
        } else {
            constexpr double scaled = Fraction * 10; // 取出下一位小數
            return toFixedString<(int) scaled>() // 加上整數部分
                   + FractionToFixedString<scaled - (int) scaled>(); // 繼續處理剩餘小數
        }
    }

    template<double Number>
    constexpr auto toFixedString() {
        constexpr int IntegerPart = Number; // 整數部分
        constexpr double FractionPart = Number - IntegerPart; // 小數部分

        if constexpr (FractionPart < std::numeric_limits<double>::epsilon()) {
            // 沒有小數部分 → 僅輸出整數
            return toFixedString<IntegerPart>();
        } else {
            // 有小數 → 整數 + "." + 小數遞迴轉換
            return toFixedString<IntegerPart>()
                   + "."
                   + FractionToFixedString<FractionPart>();
        }
    }

    template<typename T>
    struct IsFixedString : std::false_type {
    };

    template<size_t N>
    struct IsFixedString<FixedString<N> > : std::true_type {
    };

    template<typename T, size_t N = 0>
    struct FixedType {
        using SaveType = std::conditional_t<std::is_same_v<char, T> && N != 0, FixedString<N>, T>;
        SaveType value;

        constexpr FixedType(T u) : value(u) {
        }

        constexpr FixedType(const T (&str)[N]) : value(str) {
        }
    };

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
        { T::name } -> std::convertible_to<std::string_view>;
        { T::constraint } -> std::convertible_to<column_constraint>;
    };

    template<typename T, ColumnConcept U>
    struct TableColumn_Base : U {
        using TableType = T;
    };

    template<typename T>
    concept IsTableColumn = requires()
    {
        typename T::TableType;
    };

    template<typename T>
    constexpr auto GetColumnName() {
        if constexpr (IsTableColumn<T>) {
            return T::TableType::name + FixedString(".") + T::name;
        } else {
            return T::name;
        }
    }

    struct EmptyCond {
        using Columns = void;
        constexpr static FixedString condition = " 1 ";
    };

    template<typename T1, typename T2, FixedString Opt>
    struct TwoValueCond {
        using Columns = typeGroup<T1, T2>;
        constexpr static FixedString condition = " " + GetColumnName<T1>() + Opt + GetColumnName<T2>();
    };

    template<typename T1, typename T2>
    using EqualCond = TwoValueCond<T1, T2, FixedString(" = ")>;
    template<typename T1, typename T2>
    using NotEqualCond = TwoValueCond<T1, T2, FixedString(" != ")>;
    template<typename T1, typename T2>
    using GreaterThanCond = TwoValueCond<T1, T2, FixedString(" > ")>;
    template<typename T1, typename T2>
    using GreaterThanEqualCond = TwoValueCond<T1, T2, FixedString(" >= ")>;
    template<typename T1, typename T2>
    using LessThanCond = TwoValueCond<T1, T2, FixedString(" < ")>;
    template<typename T1, typename T2>
    using LessThanEqualCond = TwoValueCond<T1, T2, FixedString(" <= ")>;

    template<typename T1, FixedType V2, FixedString Opt>
    struct OneValueCond;

    template<typename T1, FixedType V2, FixedString Opt> requires IsFixedString<typename decltype(V2)::SaveType>::value
    struct OneValueCond<T1, V2, Opt> {
        constexpr static FixedString condition = " " + GetColumnName<T1>() + Opt + "'" + V2.value + "'";
    };

    template<typename T1, FixedType V2, FixedString Opt> requires (!IsFixedString<typename decltype(V2
    )::SaveType>::value)
    struct OneValueCond<T1, V2, Opt> {
        constexpr static FixedString condition = " " + GetColumnName<T1>() + Opt + toFixedString<V2.value>();
    };

    template<typename T1, FixedType T2>
    using EqualValueCond = OneValueCond<T1, T2, FixedString(" = ")>;
    template<typename T1, FixedType T2>
    using NotEqualValueCond = OneValueCond<T1, T2, FixedString(" != ")>;
    template<typename T, FixedType V2> requires std::is_integral_v<typename decltype(V2)::SaveType>
    using GreaterThanValueCond = OneValueCond<T, V2, FixedString(" > ")>;
    template<typename T, FixedType V2> requires std::is_integral_v<typename decltype(V2)::SaveType>
    using GreaterThanEqualValueCond = OneValueCond<T, V2, FixedString(" >= ")>;
    template<typename T, FixedType V2> requires std::is_integral_v<typename decltype(V2)::SaveType>
    using LessThanValueCond = OneValueCond<T, V2, FixedString(" < ")>;
    template<typename T, FixedType V2> requires std::is_integral_v<typename decltype(V2)::SaveType>
    using LessThanEqualValueCond = OneValueCond<T, V2, FixedString(" <= ")>;

    template<typename Cond1, typename Cond2>
    struct AndCond {
        constexpr static FixedString condition = "(" + Cond1::condition + ") AND (" + Cond2::condition + ")";
    };

    template<typename Cond1, typename Cond2>
    struct OrCond {
        constexpr static FixedString condition = "(" + Cond1::condition + ") OR (" + Cond2::condition + ")";
    };

    template<ColumnConcept T>
    std::string DefineColumnSQL() {
        std::string sql = std::string(T::name) + " ";
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
            default:
                throw std::invalid_argument("Unsupported column type");
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

        // 使用 remove_cvref_t 統一處理型別判斷
        using ValueT = std::remove_cvref_t<decltype(std::declval<T>().value)>;

        constexpr bool is_nullable =
                (std::is_assignable_v<ValueT &, std::nullptr_t> ||
                 std::is_constructible_v<ValueT, std::nullptr_t>) &&
                !std::is_same_v<ValueT, std::string> &&
                !std::is_same_v<ValueT, std::vector<uint8_t> >;

        // 明確先處理 SQLITE_NULL
        if (datatype == SQLITE_NULL) {
            if constexpr (is_nullable) {
                t.value = nullptr;
            } else if constexpr (std::is_same_v<ValueT, std::string>) {
                t.value.clear(); // 對於 string，NULL 用空字串表示
            } else if constexpr (std::is_same_v<ValueT, std::vector<uint8_t> >) {
                t.value.clear(); // 對於 vector，NULL 用空容器表示
            }
            // 其他型別保留預設值
            return t;
        }

        // 處理非 NULL 的情況
        if constexpr (T::type == column_type::TEXT) {
            if (datatype == SQLITE_TEXT) {
                t.value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, colIndex));
            } else {
                // 型別不符，若支援 nullptr 則賦值，否則用預設值
                if constexpr (is_nullable) {
                    t.value = nullptr;
                } else if constexpr (std::is_same_v<ValueT, std::string>) {
                    t.value.clear();
                }
            }
        } else if constexpr (T::type == column_type::NUMERIC) {
            if (datatype == SQLITE_INTEGER || datatype == SQLITE_FLOAT) {
                t.value = sqlite3_column_double(stmt, colIndex);
            } else {
                if constexpr (is_nullable) {
                    t.value = nullptr;
                } else {
                    t.value = ValueT{}; // 預設值
                }
            }
        } else if constexpr (T::type == column_type::INTEGER) {
            if (datatype == SQLITE_INTEGER) {
                t.value = sqlite3_column_int(stmt, colIndex);
            } else {
                if constexpr (is_nullable) {
                    t.value = nullptr;
                } else {
                    t.value = ValueT{}; // 預設值
                }
            }
        } else if constexpr (T::type == column_type::REAL) {
            if (datatype == SQLITE_FLOAT || datatype == SQLITE_INTEGER) {
                t.value = sqlite3_column_double(stmt, colIndex);
            } else {
                if constexpr (is_nullable) {
                    t.value = nullptr;
                } else {
                    t.value = ValueT{}; // 預設值
                }
            }
        } else if constexpr (T::type == column_type::BLOB) {
            if (datatype == SQLITE_BLOB) {
                auto pBytes = static_cast<const uint8_t *>(sqlite3_column_blob(stmt, colIndex));
                auto n = static_cast<size_t>(sqlite3_column_bytes(stmt, colIndex));
                if (pBytes && n > 0) {
                    t.value = std::vector<uint8_t>(pBytes, pBytes + n);
                } else {
                    t.value.clear();
                }
            } else {
                if constexpr (is_nullable) {
                    t.value = nullptr;
                } else if constexpr (std::is_same_v<ValueT, std::vector<uint8_t> >) {
                    t.value.clear();
                }
            }
        }
        return t;
    }

    template<typename T>
    void bindValue(sqlite3_stmt *stmt, int index, const T &value) {
        ;
        if constexpr (T::type == column_type::TEXT) {
            sqlite3_bind_text(stmt, index, value.value.c_str(), -1, SQLITE_TRANSIENT);
        } else if constexpr (T::type == column_type::NUMERIC) {
            sqlite3_bind_double(stmt, index, static_cast<double>(value.value));
        } else if constexpr (T::type == column_type::INTEGER) {
            sqlite3_bind_int(stmt, index, value.value);
        } else if constexpr (T::type == column_type::REAL) {
            sqlite3_bind_double(stmt, index, value.value);
        } else if constexpr (T::type == column_type::BLOB) {
            sqlite3_bind_blob(stmt, index, value.value.data(), static_cast<int>(value.value.size()), SQLITE_TRANSIENT);
        }
    }

    class SQLiteWrapper {
    private:
        static void FinalizeStmt(sqlite3_stmt *stmt) {
            if (stmt) {
                if (sqlite3_finalize(stmt) != SQLITE_OK) {
                    throw std::runtime_error("Failed to finalize statement");
                }
            }
        }

        using AutoStmtPtr = std::unique_ptr<sqlite3_stmt, decltype(&FinalizeStmt)>;

        static AutoStmtPtr MakeAutoStmtPtr() {
            return {nullptr, FinalizeStmt};
        }

        static AutoStmtPtr MakeAutoStmtPtr(sqlite3_stmt *stmt) {
            return {stmt, FinalizeStmt};
        }

        template<typename T, typename... Ts>
        static std::tuple<T, Ts...> GetRowData(sqlite3_stmt *stmt, int colIndex = 0) {
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

    public:
        std::string _db_path;
        std::unique_ptr<sqlite3, decltype(&sqlite3_close)> _dbPtr = {nullptr, sqlite3_close};

        explicit SQLiteWrapper(const std::string &dbPath, const bool removeExisting = false,
                               const int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
        ) : _db_path(std::move(dbPath)) {
            // Optionally delete existing DB file if requested (useful for tests)
            if (removeExisting) {
                std::error_code ec;
                if (std::filesystem::exists(_db_path, ec)) {
                    std::filesystem::remove(_db_path, ec);
                }
            }
            sqlite3 *pDb = nullptr;
            if (sqlite3_open_v2(_db_path.c_str(), &pDb, flags, nullptr) != SQLITE_OK) {
                std::string errMsg = pDb ? sqlite3_errmsg(pDb) : "Unknown error";
                throw std::runtime_error("Can't open database: " + errMsg + "\nPath: " + _db_path);
            }
            _dbPtr = {pDb, sqlite3_close};
        }

        virtual ~SQLiteWrapper() = default;

        template<typename... ResultColumns>
        std::vector<std::tuple<ResultColumns...> > Query(const std::string &sql) const {
            auto pAutoStmt = MakeAutoStmtPtr();
            {
                sqlite3_stmt *pStmt = nullptr;
                if (sqlite3_prepare_v2(_dbPtr.get(), sql.c_str(), -1, &pStmt, nullptr) != SQLITE_OK) {
                    throw std::runtime_error(
                        "Failed to prepare statement: " + std::string(sqlite3_errmsg(_dbPtr.get())) +
                        "\nSQL: " + sql);
                }
                pAutoStmt = MakeAutoStmtPtr(pStmt);
            }

            std::vector<std::tuple<ResultColumns...> > ret;
            while (sqlite3_step(pAutoStmt.get()) == SQLITE_ROW) {
                ret.push_back(GetRowData<ResultColumns...>(pAutoStmt.get()));
            }
            return ret;
        }

        template<typename... ResultColumns>
        void Execute(const std::string &sql, const ResultColumns &... values) const {
            auto pAutoStmt = MakeAutoStmtPtr();
            {
                sqlite3_stmt *pStmt = nullptr;
                if (sqlite3_prepare_v2(_dbPtr.get(), sql.c_str(), -1, &pStmt, nullptr) != SQLITE_OK) {
                    throw std::runtime_error(
                        "Failed to prepare statement: " + std::string(sqlite3_errmsg(_dbPtr.get())) +
                        "\nSQL: " + sql);
                }
                pAutoStmt = MakeAutoStmtPtr(pStmt);
            }
            int index = 1;
            (bindValue<ResultColumns>(pAutoStmt.get(), index++, values), ...);
            if (sqlite3_step(pAutoStmt.get()) != SQLITE_DONE) {
                throw std::runtime_error(
                    "Failed to execute statement: " + std::string(sqlite3_errmsg(_dbPtr.get())) +
                    "\nSQL: " + sql);
            }
        }
    };

    enum class JoinType {
        FULL,
        INNER,
        LEFT,
        RIGHT,
        CROSS
    };

    template<JoinType JT, typename Table1, typename Table2, typename Condition>
    class JoinTable;
    template<typename Table1, typename Table2, typename Condition>
    using FullJoinTable = JoinTable<JoinType::FULL, Table1, Table2, Condition>;
    template<typename Table1, typename Table2, typename Condition>
    using InnerJoinTable = JoinTable<JoinType::INNER, Table1, Table2, Condition>;
    template<typename Table1, typename Table2, typename Condition>
    using LeftJoinTable = JoinTable<JoinType::LEFT, Table1, Table2, Condition>;
    template<typename Table1, typename Table2, typename Condition>
    using RightJoinTable = JoinTable<JoinType::RIGHT, Table1, Table2, Condition>;
    template<typename Table1, typename Table2, typename Condition>
    using CrossJoinTable = JoinTable<JoinType::CROSS, Table1, Table2, Condition>;

    template<FixedString From, typename Columns>
    class QueryAble {
    public:
        constexpr static FixedString name = From;
        using columns = Columns;

    protected:
        SQLiteWrapper &_sqlite;

        template<typename... ResultColumns>
        class SelectQuery {
            const SQLiteWrapper &_sqlite;
            std::string _basic_sql;
            std::string _where_sql;

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
            explicit SelectQuery(const SQLiteWrapper &sqlite) : _sqlite(sqlite) {
                _basic_sql = "SELECT " + GetColumnNames<ResultColumns...>();
            }

            template<typename Condition>
            SelectQuery &Where() {
                _where_sql = " WHERE " + std::string(Condition::condition);
                return *this;
            }

             std::vector<std::tuple<ResultColumns...> > Results() {
                auto sql = _basic_sql + " FROM " + std::string(From) + _where_sql + ";";
                return _sqlite.Query<ResultColumns...>(sql);
            }
        };

    public:
        explicit QueryAble(SQLiteWrapper &sqlite) : _sqlite(sqlite) {
        }

        virtual ~QueryAble() = default;

        template<typename... ResultCol>
        auto Select() const {
            static_assert(isTypeGroupSubset<typeGroup<ResultCol...>, columns>(),
                          "ResultCol must be subset of table columns");
            return SelectQuery<ResultCol...>(_sqlite);
        }

        template<typename Table2, typename Condition>
        auto FullJoin() const {
            return FullJoinTable<QueryAble, Table2, Condition>(this->_sqlite);
        }

        template<typename Table2, typename Condition>
        auto InnerJoin() const {
            return InnerJoinTable<QueryAble, Table2, Condition>(this->_sqlite);
        }

        template<typename Table2, typename Condition>
        auto LeftJoin() const {
            return LeftJoinTable<QueryAble, Table2, Condition>(this->_sqlite);
        }

        template<typename Table2, typename Condition>
        auto RightJoin() const {
            return RightJoinTable<QueryAble, Table2, Condition>(this->_sqlite);
        }

        template<typename Table2, typename Condition>
        auto CrossJoin() const {
            return CrossJoinTable<QueryAble, Table2, Condition>(this->_sqlite);
        }
    };

    template<JoinType jt>
    constexpr auto GetJoinTypeString() {
        if constexpr (jt == JoinType::FULL) {
            return FixedString(" FULL JOIN ");
        } else if constexpr (jt == JoinType::INNER) {
            return FixedString(" INNER JOIN ");
        } else if constexpr (jt == JoinType::LEFT) {
            return FixedString(" LEFT JOIN ");
        } else if constexpr (jt == JoinType::RIGHT) {
            return FixedString(" RIGHT JOIN ");
        } else if constexpr (jt == JoinType::CROSS) {
            return FixedString(" CROSS JOIN ");
        }
        throw std::runtime_error("Unsupported join type");
    }


    template<JoinType JT, typename Table1, typename Table2, typename Condition>
    class JoinTable : public QueryAble<Table1::name + GetJoinTypeString<JT>() + Table2::name +
                                       FixedString(" ON ") + Condition::condition, typename ConcatTypeGroup<typename
                Table1::columns, typename Table2::columns>::type> {
    public:
        constexpr static FixedString name = Table1::name + GetJoinTypeString<JT>() + Table2::name +
                                            FixedString(" ON ") + Condition::condition;
        using columns = ConcatTypeGroup<typename Table1::columns, typename Table2::columns>::type;

        explicit JoinTable(SQLiteWrapper &sqlite) : QueryAble<Table1::name + GetJoinTypeString<JT>() + Table2::name +
                                                          FixedString(" ON ") + Condition::condition, typename
            ConcatTypeGroup<typename
                Table1::columns, typename Table2::columns>::type>(sqlite) {
        }
    };

    template<FixedString TableName, typename... Columns>
    class Table : public QueryAble<TableName, typeGroup<TableColumn_Base<Table<TableName, Columns...>, Columns>...> > {
    public:
        template<typename Col>
        using TableColumn = TableColumn_Base<Table, Col>;
        constexpr static FixedString name = TableName;
        using columns = typeGroup<TableColumn<Columns>...>;

    private:
        SQLiteWrapper &_sqlite;

        template<typename... Ts>
        class UpdateQuery {
            const Table &_table;
            std::tuple<Ts...> datas;
            std::string _basic_sql;
            std::string _set_sql;
            std::string _where_sql;

        public:
            explicit UpdateQuery(const Table &table, Ts... ts) : _table(table), datas(std::forward<Ts>(ts)...) {
                _basic_sql = std::string("UPDATE ") + std::string(name) + " SET " + GetUpdateField<Ts...>();
            }

            template<typename Condition>
            UpdateQuery &Where() {
                _where_sql = std::string(" WHERE ") + std::string(Condition::condition);
                return *this;
            }

            void Execute() {
                _table._sqlite.Execute(_basic_sql + _where_sql + ";", std::get<Ts>(datas)...);
            }
        };

        class DeleteQuery {
            const Table &_table;
            std::string _basic_sql;
            std::string _where_sql;

        public:
            explicit DeleteQuery(const Table &table) : _table(table) {
                _basic_sql = std::string("DELETE FROM ") + std::string(name);
            }

            template<typename Condition>
            DeleteQuery &Where() {
                _where_sql = std::string(" WHERE ") + std::string(Condition::condition);
                return *this;
            }

            void Execute() {
                _table._sqlite.Execute(_basic_sql + _where_sql + ";");
            }
        };

    public:
        explicit Table(SQLiteWrapper &sqlite) : QueryAble<TableName, typeGroup<TableColumn<Columns>...> >(sqlite), _sqlite(sqlite) {
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

    template<typename... Table>
    class Database {
    public:
        class Transaction {
            friend class Database;

        private:
            SQLiteWrapper &_sqlite;
            bool _isCommittedOrRolledBack = false;
            int _exceptionCount;

            explicit Transaction(SQLiteWrapper &sqlite) : _sqlite(sqlite), _exceptionCount(std::uncaught_exceptions()) {
                char *errMsg = nullptr;
                _sqlite.Execute("BEGIN TRANSACTION;");
            }

        public:
            ~Transaction() noexcept {
                if (_isCommittedOrRolledBack) {
                    return;
                }

                if (std::uncaught_exceptions() > _exceptionCount) {
                    try {
                        Rollback();
                    } catch (const std::exception &exception) {
                        std::cerr << exception.what() << std::endl;
                    }
                } else {
                    try {
                        Commit();
                    } catch (const std::exception &exception) {
                        std::cerr << exception.what() << std::endl;
                    }
                }
            }

            void Rollback() {
                if (_isCommittedOrRolledBack) {
                    throw std::runtime_error("Multiple commit/rollback calls on the same transaction");
                }
                _isCommittedOrRolledBack = true;
                _sqlite.Execute("ROLLBACK;");
            }

            void Commit() {
                if (_isCommittedOrRolledBack) {
                    throw std::runtime_error("Multiple commit/rollback calls on the same transaction");
                }
                _isCommittedOrRolledBack = true;
                _sqlite.Execute("COMMIT;");
            }
        };

    private:
        SQLiteWrapper _sqlite;
        std::tuple<Table...> _tables;


    public:
        explicit Database(const std::string &db_path, bool removeExisting = false,
                          const int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)
            : _sqlite(db_path, removeExisting, flags),
              _tables(Table(_sqlite)...) {
        }

        template<typename T>
        T &GetTable() {
            return std::get<T>(_tables);
        }

        void CreateTransaction(const std::function<void(Transaction &)> &callback) {
            Transaction transaction(_sqlite);
            callback(transaction);
            // 解構子會自動根據錯誤狀態決定 Commit 或 Rollback
        }

        void CreateTransaction(const std::function<void()> &callback) {
            Transaction transaction(_sqlite);
            callback();
            // 解構子會自動根據錯誤狀態決定 Commit 或 Rollback
        }
    };
}

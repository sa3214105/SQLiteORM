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
#include <filesystem>
#include <functional>

namespace SQLiteHelper {
    template<typename... Ts>
    struct typeGroup;

    template<typename T, typename... Ts>
    struct typeGroup<T, Ts...> {
        using type = T;
        using next = typeGroup<Ts...>;
    };

    template<>
    struct typeGroup<> {
        using type = void;
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
        } else if constexpr (!std::is_same_v<typename TG::next, typeGroup<> >) {
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
                if constexpr (!std::is_same_v<typename TG1::next, typeGroup<> >) {
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

    template<std::size_t N>
    std::string operator+(const FixedString<N> &a, const std::string &b) {
        return std::string(a.value) + b;
    }

    template<std::size_t N>
    std::string operator+(const std::string &a, const FixedString<N> &b) {
        return a + std::string(b.value);
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

    // Helper to convert FixedString to a quoted string for DEFAULT
    template<size_t N>
    constexpr auto toFixedStringLiteral(const FixedString<N> &fs) {
        return FixedString("'") + fs + FixedString("'");
    }

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

    template<column_type type>
    constexpr auto ColumnTypeToString() {
        switch (type) {
            case column_type::TEXT:
                return "TEXT";
            case column_type::NUMERIC:
                return "NUMERIC";
            case column_type::INTEGER:
                return "INTEGER";
            case column_type::REAL:
                return "REAL";
            case column_type::BLOB:
                return "BLOB";
            default:
                return "UNKNOWN";
        }
    }

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

    template<typename ColumnGroup, typename... Parameters>
    struct Condition {
        using Columns = ColumnGroup;
        std::string condition;
        std::tuple<Parameters...> params;

        template<typename ColumnOther, typename... ParametersOther>
        auto operator&&(const Condition<ColumnOther, ParametersOther...> &other) const {
            return Condition<typename ConcatTypeGroup<ColumnGroup, ColumnOther>::type, Parameters..., ParametersOther
                ...>{
                .condition = "(" + condition + ") AND (" + other.condition + ")",
                .params = std::tuple_cat(params, other.params)
            };
        }

        template<typename ColumnOther, typename... ParametersOther>
        auto operator||(const Condition<ColumnOther, ParametersOther...> &other) const {
            return Condition<typename ConcatTypeGroup<ColumnGroup, ColumnOther>::type, Parameters..., ParametersOther
                ...>{
                .condition = "(" + condition + ") OR (" + other.condition + ")",
                .params = std::tuple_cat(params, other.params)
            };
        }
    };

    template<typename T>
    concept IsSQLNum = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template<typename T>
    concept IsSQLLiteral = IsSQLNum<T> || std::is_same_v<T, std::string> || std::is_same_v<T, const char *>;

    template<FixedString Opt, IsTableColumn Col1, IsTableColumn Col2>
    auto MakeCondition() {
        return Condition<typeGroup<Col1, Col2> >{
            .condition = std::string(GetColumnName<Col1>()) + Opt + std::string(GetColumnName<Col2>()),
            .params = {}
        };
    }

    template<FixedString Opt, IsTableColumn Col, IsSQLLiteral V>
    auto MakeCondition(V v) {
        return Condition<typeGroup<Col>, std::conditional_t<std::is_same_v<const char *, V>, std::string, V> >{
            .condition = std::string(GetColumnName<Col>()) + Opt + "?",
            .params = {v}
        };
    }

    template<FixedString Opt, IsSQLLiteral V1, IsSQLLiteral V2>
    auto MakeCondition(V1 v1, V2 v2) {
        return Condition<void, std::conditional_t<std::is_same_v<const char *, V1>, std::string, V1>,
            std::conditional_t<std::is_same_v<const char *, V2>, std::string, V2> >{
            .condition = std::string("?") + Opt + "?",
            .params = {v1, v2}
        };
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto Equal() {
        return MakeCondition<" = ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto NotEqual() {
        return MakeCondition<" != ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto GreaterThan() {
        return MakeCondition<" > ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto GreaterThanEqual() {
        return MakeCondition<" >= ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto LessThan() {
        return MakeCondition<" < ", Col1, Col2>();
    }

    template<IsTableColumn Col1, IsTableColumn Col2>
    auto LessThanEqual() {
        return MakeCondition<" <= ", Col1, Col2>();
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto Equal(V v) {
        return MakeCondition<" = ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto NotEqual(V v) {
        return MakeCondition<" != ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto GreaterThan(V v) {
        return MakeCondition<" > ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto GreaterThanEqual(V v) {
        return MakeCondition<" >= ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto LessThan(V v) {
        return MakeCondition<" < ", Col>(v);
    }

    template<IsTableColumn Col, IsSQLLiteral V>
    auto LessThanEqual(V v) {
        return MakeCondition<" <= ", Col>(v);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto Equal(V1 v1, V2 v2) {
        return MakeCondition<" = ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto NotEqual(V1 v1, V2 v2) {
        return MakeCondition<" != ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto GreaterThan(V1 v1, V2 v2) {
        return MakeCondition<" > ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto GreaterThanEqual(V1 v1, V2 v2) {
        return MakeCondition<" >= ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto LessThan(V1 v1, V2 v2) {
        return MakeCondition<" < ">(v1, v2);
    }

    template<IsSQLLiteral V1, IsSQLLiteral V2>
    auto LessThanEqual(V1 v1, V2 v2) {
        return MakeCondition<" <= ">(v1, v2);
    }

    enum class OrderType {
        ASC,
        DESC
    };

    template<OrderType order>
    constexpr auto OrderTypeToString() {
        if constexpr (order == OrderType::ASC) {
            return FixedString(" ASC");
        } else {
            return FixedString(" DESC");
        }
    }

    enum class ConflictCause {
        ROLLBACK,
        ABORT,
        FAIL,
        IGNORE,
        REPLACE
    };

    template<ConflictCause Cause>
    constexpr auto ConflictCauseToString() {
        if constexpr (Cause == ConflictCause::ROLLBACK) {
            return FixedString(" ON CONFLICT ROLLBACK");
        } else if constexpr (Cause == ConflictCause::ABORT) {
            return FixedString(" ON CONFLICT ABORT");
        } else if constexpr (Cause == ConflictCause::FAIL) {
            return FixedString(" ON CONFLICT FAIL");
        } else if constexpr (Cause == ConflictCause::IGNORE) {
            return FixedString(" ON CONFLICT IGNORE");
        } else if constexpr (Cause == ConflictCause::REPLACE) {
            return FixedString(" ON CONFLICT REPLACE");
        }
    }

    template<OrderType order = OrderType::ASC, ConflictCause conflictCause = ConflictCause::ABORT>
    struct PrimaryKey {
        constexpr static FixedString value = FixedString("PRIMARY KEY") + OrderTypeToString<order>() +
                                             ConflictCauseToString<conflictCause>();
    };

    template<ConflictCause conflictCause = ConflictCause::ABORT>
    struct NotNull {
        constexpr static FixedString value = "NOT NULL" + ConflictCauseToString<conflictCause>();
    };

    template<ConflictCause conflictCause = ConflictCause::ABORT>
    struct Unique {
        constexpr static FixedString value = "UNIQUE" + ConflictCauseToString<conflictCause>();
    };

    template<FixedType DefaultValue>
    struct Default {
        constexpr static auto GetDefaultValueString() {
            using ValueType = decltype(DefaultValue.value);
            if constexpr (IsFixedString<ValueType>::value) {
                // For string types, wrap in quotes
                return FixedString("DEFAULT ") + toFixedStringLiteral(DefaultValue.value);
            } else if constexpr (std::is_integral_v<ValueType>) {
                // For integer types
                return FixedString("DEFAULT ") + toFixedString<DefaultValue.value>();
            } else if constexpr (std::is_floating_point_v<ValueType>) {
                // For floating point types
                return FixedString("DEFAULT ") + toFixedString<DefaultValue.value>();
            } else {
                return FixedString("DEFAULT ") + toFixedString<DefaultValue.value>();
            }
        }

        constexpr static auto value = GetDefaultValueString();
    };

    template<FixedString Name, column_type Type, typename... Constraints>
    struct Column {
        constexpr static FixedString name = Name;
        constexpr static column_type type = Type;
        using constraints = typeGroup<Constraints...>;
        std::conditional_t<Type == column_type::TEXT, std::string,
            std::conditional_t<Type == column_type::NUMERIC, double,
                std::conditional_t<Type == column_type::INTEGER, int,
                    std::conditional_t<Type == column_type::REAL, double,
                        std::vector<uint8_t> > > > > value;
    };

    template<typename TG>
    constexpr auto CombineConstraintsSQL() {
        if constexpr (std::is_same_v<TG, typeGroup<> >) {
            return FixedString(" ");
        } else if constexpr (!std::is_same_v<typename TG::next, typeGroup<> >) {
            return " " + TG::type::value + CombineConstraintsSQL<typename TG::next>();
        } else {
            return " " + TG::type::value;
        }
    }

    template<typename Column>
    constexpr auto GetColumnConstraintsSQL() {
        return FixedString(" " + Column::name + " ") +
               ColumnTypeToString<Column::type>() +
               CombineConstraintsSQL<typename Column::constraints>();
    }

    template<ColumnConcept... Columns>
    std::string GetColumnDefinitions() {
        std::string result;
        ((result += GetColumnConstraintsSQL<Columns>() + ","), ...);
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
        if constexpr (ColumnConcept<T>) {
            // 處理 Column 類型
            if constexpr (T::type == column_type::TEXT) {
                sqlite3_bind_text(stmt, index, value.value.c_str(), -1, SQLITE_TRANSIENT);
            } else if constexpr (T::type == column_type::NUMERIC) {
                sqlite3_bind_double(stmt, index, static_cast<double>(value.value));
            } else if constexpr (T::type == column_type::INTEGER) {
                sqlite3_bind_int(stmt, index, value.value);
            } else if constexpr (T::type == column_type::REAL) {
                sqlite3_bind_double(stmt, index, value.value);
            } else if constexpr (T::type == column_type::BLOB) {
                sqlite3_bind_blob(stmt, index, value.value.data(), static_cast<int>(value.value.size()),
                                  SQLITE_TRANSIENT);
            }
        } else {
            // 處理原始值類型
            if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char *> ||
                          std::is_same_v<std::decay_t<T>, char *>) {
                if constexpr (std::is_same_v<T, std::string>) {
                    sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
                } else {
                    sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);
                }
            } else if constexpr (std::is_integral_v<T>) {
                sqlite3_bind_int(stmt, index, value);
            } else if constexpr (std::is_floating_point_v<T>) {
                sqlite3_bind_double(stmt, index, value);
            }
        }
    }

    class SQLiteWrapper {
    private:
        using AutoStmtPtr = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

        static AutoStmtPtr MakeAutoStmtPtr() {
            return {nullptr, sqlite3_finalize};
        }

        static AutoStmtPtr MakeAutoStmtPtr(sqlite3_stmt *stmt) {
            return {stmt, sqlite3_finalize};
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

        template<typename... ResultColumns, typename... Parameters>
        std::vector<std::tuple<ResultColumns...> > Query(const std::string &sql, Parameters... parameters) const {
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

            // 綁定參數
            if constexpr (sizeof...(Parameters) > 0) {
                int index = 1;
                ([&]() {
                    if constexpr (std::is_same_v<Parameters, std::string>) {
                        sqlite3_bind_text(pAutoStmt.get(), index++, parameters.c_str(), -1, SQLITE_TRANSIENT);
                    } else if constexpr (std::is_integral_v<Parameters>) {
                        sqlite3_bind_int(pAutoStmt.get(), index++, parameters);
                    } else if constexpr (std::is_floating_point_v<Parameters>) {
                        sqlite3_bind_double(pAutoStmt.get(), index++, parameters);
                    }
                }(), ...);
            }

            std::vector<std::tuple<ResultColumns...> > ret;
            while (sqlite3_step(pAutoStmt.get()) == SQLITE_ROW) {
                ret.push_back(GetRowData<ResultColumns...>(pAutoStmt.get()));
            }
            return ret;
        }

        template<typename... Parameter>
        void Execute(const std::string &sql, const Parameter &... values) const {
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
            (bindValue(pAutoStmt.get(), index++, values), ...);
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
    struct JoinInfo {
        using Source = Src;
        JoinType type;
        Cond condition;
    };

    template<typename MainSrc, typename... JoinSrcs>
    struct SourceInfo {
        using Source = MainSrc;
        std::tuple<JoinSrcs...> joins;
    };

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
            // 展開每個 JoinInfo
            ((sql += GetJoinTypeString(join.type)
              + std::string(std::remove_cvref_t<decltype(join)>::Source::name)
              + " ON " + join.condition.condition), ...);
        }, src.joins);
        return sql;
    }

    // 提取 SourceInfo 中所有 JOIN 條件的參數
    template<typename MainSrc, typename... Joins>
    auto ExtractSourceParams(const SourceInfo<MainSrc, Joins...> &src) {
        return std::apply([](const auto &... join) {
            return std::tuple_cat(join.condition.params...);
        }, src.joins);
    }

    // 特化：沒有 JOIN 時返回空 tuple
    template<typename MainSrc>
    auto ExtractSourceParams(const SourceInfo<MainSrc> &src) {
        return std::tuple<>();
    }

    template<JoinType JT, typename Table1, typename Table2>
    class JoinTable;
    template<typename Table1, typename Table2>
    using FullJoinTable = JoinTable<JoinType::FULL, Table1, Table2>;
    template<typename Table1, typename Table2>
    using InnerJoinTable = JoinTable<JoinType::INNER, Table1, Table2>;
    template<typename Table1, typename Table2>
    using LeftJoinTable = JoinTable<JoinType::LEFT, Table1, Table2>;
    template<typename Table1, typename Table2>
    using RightJoinTable = JoinTable<JoinType::RIGHT, Table1, Table2>;
    template<typename Table1, typename Table2>
    using CrossJoinTable = JoinTable<JoinType::CROSS, Table1, Table2>;

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
            auto newSource = JoinSource(this->_source, JoinInfo<Table2, Cond>{
                                            .type = JoinType::FULL,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<typename Table2, typename Cond>
        auto InnerJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, JoinInfo<Table2, Cond>{
                                            .type = JoinType::INNER,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<typename Table2, typename Cond>
        auto LeftJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, JoinInfo<Table2, Cond>{
                                            .type = JoinType::LEFT,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<typename Table2, typename Cond>
        auto RightJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, JoinInfo<Table2, Cond>{
                                            .type = JoinType::RIGHT,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }

        template<typename Table2, typename Cond>
        auto CrossJoin(const Cond &condition) const {
            auto newSource = JoinSource(this->_source, JoinInfo<Table2, Cond>{
                                            .type = JoinType::CROSS,
                                            .condition = condition
                                        });
            return QueryAble<typename ConcatTypeGroup<Columns, typename Table2::columns>::type, decltype(newSource)>(
                this->_sqlite, newSource);
        }
    };

    template<FixedString TableName, typename... Columns>
    class Table : public QueryAble<typeGroup<TableColumn_Base<
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

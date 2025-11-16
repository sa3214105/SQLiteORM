#pragma once
#include "SQLiteStruct/Column.hpp"

#include "sqlite3.h"
#include <memory>
#include <filesystem>

namespace TypeSQLite {
    //TODO Column類型不要在這邊做處理
    template<typename T>
    void bindValue(sqlite3_stmt *stmt, int index, const T &value) {
        if constexpr (ColumnOrTableColumnConcept<T>) {
            // 處理 Column 類型
            if constexpr (T::type == ExprResultType::TEXT) {
                sqlite3_bind_text(stmt, index, value.value.c_str(), -1, SQLITE_TRANSIENT);
            } else if constexpr (T::type == ExprResultType::NUMERIC) {
                sqlite3_bind_double(stmt, index, static_cast<double>(value.value));
            } else if constexpr (T::type == ExprResultType::INTEGER) {
                sqlite3_bind_int(stmt, index, value.value);
            } else if constexpr (T::type == ExprResultType::REAL) {
                sqlite3_bind_double(stmt, index, value.value);
            } else if constexpr (T::type == ExprResultType::BLOB) {
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
            } else {
                static_assert([]() { return false; }(), "Unsupported type for bindValue");
            }
        }
    }

    template<typename T>
    auto GetValue(sqlite3_stmt *stmt, int colIndex) {
        T t;
        auto datatype = sqlite3_column_type(stmt, colIndex);

        // 使用 remove_cvref_t 統一處理型別判斷
        using ValueT = std::remove_cvref_t<decltype(std::declval<T>())>;

        constexpr bool is_nullable =
                (std::is_assignable_v<ValueT &, std::nullptr_t> ||
                 std::is_constructible_v<ValueT, std::nullptr_t>) &&
                !std::is_same_v<ValueT, std::string> &&
                !std::is_same_v<ValueT, std::vector<uint8_t> >;

        // 明確先處理 SQLITE_NULL
        if (datatype == SQLITE_NULL) {
            if constexpr (is_nullable) {
                t = nullptr;
            } else if constexpr (std::is_same_v<ValueT, std::string>) {
                t.clear(); // 對於 string，NULL 用空字串表示
            } else if constexpr (std::is_same_v<ValueT, std::vector<uint8_t> >) {
                t.clear(); // 對於 vector，NULL 用空容器表示
            }
            // 其他型別保留預設值
            return t;
        }

        // 處理非 NULL 的情況
        if constexpr (std::is_same_v<T, std::string>) {
            if (datatype == SQLITE_TEXT) {
                t = reinterpret_cast<const char *>(sqlite3_column_text(stmt, colIndex));
            } else {
                // 型別不符，若支援 nullptr 則賦值，否則用預設值
                if constexpr (is_nullable) {
                    t = nullptr;
                } else if constexpr (std::is_same_v<ValueT, std::string>) {
                    t.clear();
                }
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (datatype == SQLITE_INTEGER || datatype == SQLITE_FLOAT) {
                t = sqlite3_column_double(stmt, colIndex);
            } else {
                if constexpr (is_nullable) {
                    t = nullptr;
                } else {
                    t = ValueT{}; // 預設值
                }
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (datatype == SQLITE_INTEGER) {
                t = sqlite3_column_int(stmt, colIndex);
            } else {
                if constexpr (is_nullable) {
                    t = nullptr;
                } else {
                    t = ValueT{}; // 預設值
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            if (datatype == SQLITE_BLOB) {
                auto pBytes = static_cast<const uint8_t *>(sqlite3_column_blob(stmt, colIndex));
                auto n = static_cast<size_t>(sqlite3_column_bytes(stmt, colIndex));
                if (pBytes && n > 0) {
                    t = std::vector<uint8_t>(pBytes, pBytes + n);
                } else {
                    t.clear();
                }
            } else {
                if constexpr (is_nullable) {
                    t = nullptr;
                } else if constexpr (std::is_same_v<ValueT, std::vector<uint8_t> >) {
                    t.clear();
                }
            }
        }
        return t;
    }

    class SQLiteWrapper final {
    public:
        // Transaction 類別，使用 RAII 模式管理交易
        class Transaction {
        private:
            SQLiteWrapper &_sqlite;
            bool _isCommittedOrRolledBack = false;
            int _exceptionCount;

        public:
            explicit Transaction(SQLiteWrapper &sqlite)
                : _sqlite(sqlite), _exceptionCount(std::uncaught_exceptions()) {
                _sqlite.Execute("BEGIN TRANSACTION;");
            }

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
}

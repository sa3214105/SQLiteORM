#pragma once
#include "Table.hpp"
#include "../TemplateHelper/FixedString.hpp"
#include "../SQLiteWrapper.hpp"

namespace TypeSQLite {
    template<TableDefinitionConcept... TableDefs>
    class Database {
    public:
        // 使用 SQLiteWrapper 的 Transaction
        using Transaction = SQLiteWrapper::Transaction;

    private:
        SQLiteWrapper _sqlite;
        std::tuple<Table<TableDefs>...> _tables;

    public:
        explicit Database(const std::string &db_path, TableDefs... table_defs)
            : _sqlite(db_path, false, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE),
              _tables(Table<TableDefs>(_sqlite, table_defs)...) {
        }

        template<typename T>
        auto &GetTable() {
            return std::get<Table<T>>(_tables);
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

#pragma once
#include "Table.hpp"
namespace SQLiteHelper {
    template<TableConcept... Table>
    class Database {
    public:
        // 使用 SQLiteWrapper 的 Transaction
        using Transaction = SQLiteWrapper::Transaction;

    private:
        SQLiteWrapper _sqlite;
        std::tuple<Table...> _tables;

    public:
        explicit Database(const std::string &db_path, bool removeExisting = false,
                          const int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)
            : _sqlite(db_path, removeExisting, flags),
              _tables(Table(_sqlite)...) {
        }

        template<TableConcept T>
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
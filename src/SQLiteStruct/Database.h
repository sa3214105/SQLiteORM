#pragma once
namespace SQLiteHelper {
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
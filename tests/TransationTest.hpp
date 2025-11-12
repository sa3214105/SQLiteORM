#pragma once
#include "Common.hpp"

// ============ Transaction 測試 ============
TEST(TransactionTest, TransactionCommit) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transation) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("TxUser"),
                                        UserTable::MakeTableColumn<AgeColumn>(99));
    });
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("TxUser")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 99);
}

TEST(TransactionTest, TransactionRollback) {
    Database<UserTable> db("test_database.db");
    EXPECT_ANY_THROW(db.CreateTransaction([&db](auto &transation ) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("TxRollback"), UserTable::MakeTableColumn
            <AgeColumn>(88));
        throw std::runtime_error("Force rollback");
        }));
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("TxRollback")).Results();
    EXPECT_EQ(results.size(), 0);
}

TEST(TransactionTest, TransactionExplicitCommit) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transaction) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("ExplicitCommit"),
                                        UserTable::MakeTableColumn<AgeColumn>(77));
        transaction.Commit(); // 明確呼叫 Commit
    });
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("ExplicitCommit")).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 77);
}

TEST(TransactionTest, TransactionExplicitRollback) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transaction) {
        db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("ExplicitRollback"),
                                        UserTable::MakeTableColumn<AgeColumn>(66));
        transaction.Rollback(); // 明確呼叫 Rollback
    });
    auto results = db.GetTable<UserTable>().Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<
                AgeColumn> >()
            .Where(Equal<UserTable::TableColumn<NameColumn> >("ExplicitRollback")).Results();
    EXPECT_EQ(results.size(), 0);
}
#pragma once
#include "Common.hpp"

// ============ Transaction 測試 ============
TEST(TransactionTest, TransactionCommit) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transation) {
        db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
            UserTable::TableColumn<AgeColumn> >("TxUser", 99);
    });
    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(),
                                                   UserTable::TableColumn<AgeColumn>())
            .Where(UserTable::TableColumn<NameColumn>() == "TxUser"_expr).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 99);
}

TEST(TransactionTest, TransactionRollback) {
    Database<UserTable> db("test_database.db");
    EXPECT_ANY_THROW(db.CreateTransaction([&db](auto &transation ) {
        db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,UserTable::TableColumn<AgeColumn>>(
            "TxRollback",88);
        throw std::runtime_error("Force rollback");
        }));
    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<
                                                       AgeColumn>())
            .Where(UserTable::TableColumn<NameColumn>() == "TxRollback"_expr).Results();
    EXPECT_EQ(results.size(), 0);
}

TEST(TransactionTest, TransactionExplicitCommit) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transaction) {
        db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn> >(
            "ExplicitCommit", 77);
        transaction.Commit(); // 明確呼叫 Commit
    });
    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(),
                                                   UserTable::TableColumn<AgeColumn>())
            .Where(UserTable::TableColumn<NameColumn>() == "ExplicitCommit"_expr).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 77);
}

TEST(TransactionTest, TransactionExplicitRollback) {
    Database<UserTable> db("test_database.db");
    db.CreateTransaction([&db](auto &transaction) {
        db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn> >(
            "ExplicitRollback", 66);
        transaction.Rollback(); // 明確呼叫 Rollback
    });
    auto results = db.GetTable<UserTable>().Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<
                AgeColumn>())
            .Where(UserTable::TableColumn<NameColumn>() == "ExplicitRollback"_expr).Results();
    EXPECT_EQ(results.size(), 0);
}

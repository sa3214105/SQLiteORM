#pragma once
#include "Common.hpp"

class TransactionTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 每個測試前會重新建立資料庫
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// ============ Transaction 測試 ============
TEST_F(TransactionTest, TransactionCommit) {
    db.CreateTransaction([this](auto &transaction) {
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("TxUser", 99);
    });

    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .Where(userTable[NameColumn] == "TxUser"_expr).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 99);
}

TEST_F(TransactionTest, TransactionRollback) {
    EXPECT_ANY_THROW(db.CreateTransaction([this](auto &transaction) {
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("TxRollback", 88);
        throw std::runtime_error("Force rollback");
    }));

    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .Where(userTable[NameColumn] == "TxRollback"_expr).Results();
    EXPECT_EQ(results.size(), 0);
}

TEST_F(TransactionTest, TransactionExplicitCommit) {
    db.CreateTransaction([this](auto &transaction) {
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("ExplicitCommit", 77);
        transaction.Commit(); // 明確呼叫 Commit
    });

    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .Where(userTable[NameColumn] == "ExplicitCommit"_expr).Results();
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(std::get<1>(results[0]), 77);
}

TEST_F(TransactionTest, TransactionExplicitRollback) {
    db.CreateTransaction([this](auto &transaction) {
        userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("ExplicitRollback", 66);
        transaction.Rollback(); // 明確呼叫 Rollback
    });

    auto results = userTable.Select(userTable[NameColumn], userTable[AgeColumn])
            .Where(userTable[NameColumn] == "ExplicitRollback"_expr).Results();
    EXPECT_EQ(results.size(), 0);
}

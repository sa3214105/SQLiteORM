#pragma once
#include "Common.hpp"

// ============ LimitOffset Tests ============

// 基本 LimitOffset 測試類別（使用 10 筆資料）
class LimitOffsetTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入 10 筆測試資料
        for (int i = 0; i < 10; i++) {
            userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>(
                "User" + std::to_string(i),
                20 + i
            );
        }
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 小資料集測試類別（使用 5 筆資料）
class LimitOffsetSmallTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition)> db = Database{"test_database.db", UserTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        // 插入 5 筆測試資料
        for (int i = 0; i < 5; i++) {
            userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>(
                "User" + std::to_string(i),
                20 + i
            );
        }
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// 測試只使用 LIMIT
TEST_F(LimitOffsetTest, LimitOnly) {
    // Select with LIMIT 5
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .LimitOffset(5)
            .Results();

    EXPECT_EQ(results.size(), 5);
}

// 測試 LIMIT 與 OFFSET 組合
TEST_F(LimitOffsetTest, LimitWithOffset) {
    // Select with LIMIT 3 OFFSET 2
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .LimitOffset(3, 2)
            .Results();

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<0>(results[0]), "User2");
    EXPECT_EQ(std::get<1>(results[0]), 22);
}

// 測試 LIMIT OFFSET 與 WHERE 組合
TEST_F(LimitOffsetTest, LimitOffsetWithWhere) {
    // Select with WHERE, LIMIT and OFFSET
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .Where(userTable[AgeColumn] >= 23_expr)
            .LimitOffset(3, 1)
            .Results();

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<1>(results[0]), 24);
}

// 測試 OFFSET 超過總行數
TEST_F(LimitOffsetSmallTest, OffsetExceedsTotalRows) {
    // Select with OFFSET greater than total rows
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .LimitOffset(5, 10)
            .Results();

    EXPECT_EQ(results.size(), 0);
}

// 測試 LIMIT 超過總行數
TEST_F(LimitOffsetSmallTest, LimitExceedsTotalRows) {
    // Select with LIMIT greater than total rows
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .LimitOffset(100)
            .Results();

    EXPECT_EQ(results.size(), 5);
}

// 測試分頁功能
TEST_F(LimitOffsetTest, Pagination) {
    // Simulate pagination: page size = 3
    int pageSize = 3;

    // Page 1 (offset 0)
    auto page1 = userTable
            .Select(userTable[NameColumn])
            .LimitOffset(pageSize, 0)
            .Results();
    EXPECT_EQ(page1.size(), 3);
    EXPECT_EQ(std::get<0>(page1[0]), "User0");

    // Page 2 (offset 3)
    auto page2 = userTable
            .Select(userTable[NameColumn])
            .LimitOffset(pageSize, 3)
            .Results();
    EXPECT_EQ(page2.size(), 3);
    EXPECT_EQ(std::get<0>(page2[0]), "User3");

    // Page 3 (offset 6)
    auto page3 = userTable
            .Select(userTable[NameColumn])
            .LimitOffset(pageSize, 6)
            .Results();
    EXPECT_EQ(page3.size(), 3);
    EXPECT_EQ(std::get<0>(page3[0]), "User6");

    // Page 4 (offset 9, only 1 record left)
    auto page4 = userTable
            .Select(userTable[NameColumn])
            .LimitOffset(pageSize, 9)
            .Results();
    EXPECT_EQ(page4.size(), 1);
    EXPECT_EQ(std::get<0>(page4[0]), "User9");
}

// 測試 LIMIT 為 0
TEST_F(LimitOffsetSmallTest, ZeroLimit) {
    // Select with LIMIT 0
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .LimitOffset(0)
            .Results();

    EXPECT_EQ(results.size(), 0);
}

// 測試 WHERE 之後鏈接 LIMIT OFFSET
TEST_F(LimitOffsetTest, LimitOffsetChainedAfterWhere) {
    // Chain Where then LimitOffset
    auto results = userTable
            .Select(userTable[NameColumn], userTable[AgeColumn])
            .Where(userTable[AgeColumn] < 28_expr)
            .LimitOffset(2, 1)
            .Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), 21);
    EXPECT_EQ(std::get<1>(results[1]), 22);
}


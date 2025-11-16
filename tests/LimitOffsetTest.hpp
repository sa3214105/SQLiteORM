#pragma once
#include "Common.hpp"

// ============ LimitOffset Tests ============
TEST(LimitOffsetTest, LimitOnly) {
    Database<UserTable> db("test_database.db", true);

    // Insert 10 records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert<
            UserTable::TableColumn<NameColumn>,
            UserTable::TableColumn<AgeColumn>
        >("User" + std::to_string(i), 20 + i);
    }

    // Select with LIMIT 5
    auto results = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<AgeColumn>())
            .LimitOffset(5)
            .Results();

    EXPECT_EQ(results.size(), 5);
}

TEST(LimitOffsetTest, LimitWithOffset) {
    Database<UserTable> db("test_database.db", true);

    // Insert 10 records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert<
            UserTable::TableColumn<NameColumn>,
            UserTable::TableColumn<AgeColumn>
        >("User" + std::to_string(i), 20 + i);
    }

    // Select with LIMIT 3 OFFSET 2
    auto results = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<AgeColumn>())
            .LimitOffset(3, 2)
            .Results();

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<0>(results[0]), "User2");
    EXPECT_EQ(std::get<1>(results[0]), 22);
}

TEST(LimitOffsetTest, LimitOffsetWithWhere) {
    Database<UserTable> db("test_database.db", true);

    // Insert 10 records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert<
            UserTable::TableColumn<NameColumn>,
            UserTable::TableColumn<AgeColumn>
        >("User" + std::to_string(i), 20 + i);
    }

    // Select with WHERE, LIMIT and OFFSET
    auto results = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<AgeColumn>())
            .Where(UserTable::TableColumn<AgeColumn>() >= 23_expr)
            .LimitOffset(3, 1)
            .Results();

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<1>(results[0]), 24);
}

TEST(LimitOffsetTest, OffsetExceedsTotalRows) {
    Database<UserTable> db("test_database.db", true);

    // Insert 5 records
    for (int i = 0; i < 5; i++) {
        db.GetTable<UserTable>().Insert<
            UserTable::TableColumn<NameColumn>,
            UserTable::TableColumn<AgeColumn>
        >("User" + std::to_string(i), 20 + i);
    }

    // Select with OFFSET greater than total rows
    auto results = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<AgeColumn>())
            .LimitOffset(5, 10)
            .Results();

    EXPECT_EQ(results.size(), 0);
}

TEST(LimitOffsetTest, LimitExceedsTotalRows) {
    Database<UserTable> db("test_database.db", true);

    // Insert 5 records
    for (int i = 0; i < 5; i++) {
        db.GetTable<UserTable>().Insert<
            UserTable::TableColumn<NameColumn>,
            UserTable::TableColumn<AgeColumn>
        >("User" + std::to_string(i), 20 + i);
    }

    // Select with LIMIT greater than total rows
    auto results = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<AgeColumn>())
            .LimitOffset(100)
            .Results();

    EXPECT_EQ(results.size(), 5);
}

TEST(LimitOffsetTest, Pagination) {
    Database<UserTable> db("test_database.db", true);

    // Insert 10 records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert<
            UserTable::TableColumn<NameColumn>,
            UserTable::TableColumn<AgeColumn>
        >("User" + std::to_string(i), 20 + i);
    }

    // Simulate pagination: page size = 3
    int pageSize = 3;

    // Page 1 (offset 0)
    auto page1 = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>())
            .LimitOffset(pageSize, 0)
            .Results();
    EXPECT_EQ(page1.size(), 3);
    EXPECT_EQ(std::get<0>(page1[0]), "User0");

    // Page 2 (offset 3)
    auto page2 = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>())
            .LimitOffset(pageSize, 3)
            .Results();
    EXPECT_EQ(page2.size(), 3);
    EXPECT_EQ(std::get<0>(page2[0]), "User3");

    // Page 3 (offset 6)
    auto page3 = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>())
            .LimitOffset(pageSize, 6)
            .Results();
    EXPECT_EQ(page3.size(), 3);
    EXPECT_EQ(std::get<0>(page3[0]), "User6");

    // Page 4 (offset 9, only 1 record left)
    auto page4 = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>())
            .LimitOffset(pageSize, 9)
            .Results();
    EXPECT_EQ(page4.size(), 1);
    EXPECT_EQ(std::get<0>(page4[0]), "User9");
}

TEST(LimitOffsetTest, ZeroLimit) {
    Database<UserTable> db("test_database.db", true);

    // Insert 5 records
    for (int i = 0; i < 5; i++) {
        db.GetTable<UserTable>().Insert<
            UserTable::TableColumn<NameColumn>,
            UserTable::TableColumn<AgeColumn>
        >("User" + std::to_string(i), 20 + i);
    }

    // Select with LIMIT 0
    auto results = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<AgeColumn>())
            .LimitOffset(0)
            .Results();

    EXPECT_EQ(results.size(), 0);
}

TEST(LimitOffsetTest, LimitOffsetChainedAfterWhere) {
    Database<UserTable> db("test_database.db", true);

    // Insert records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn> >(
            "User" + std::to_string(i), 20 + i);
    }

    // Chain Where then LimitOffset
    auto results = db.GetTable<UserTable>()
            .Select(UserTable::TableColumn<NameColumn>(), UserTable::TableColumn<AgeColumn>())
            .Where(UserTable::TableColumn<AgeColumn>() < 28_expr)
            .LimitOffset(2, 1)
            .Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<1>(results[0]), 21);
    EXPECT_EQ(std::get<1>(results[1]), 22);
}

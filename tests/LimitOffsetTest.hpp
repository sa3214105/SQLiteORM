#pragma once
#include "Common.hpp"

// ============ LimitOffset Tests ============
TEST(LimitOffsetTest, LimitOnly) {
    Database<UserTable> db("test_database.db", true);

    // Insert 10 records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
            UserTable::MakeTableColumn<AgeColumn>(20 + i)
        );
    }

    // Select with LIMIT 5
    auto results = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
        .LimitOffset(5)
        .Results();

    EXPECT_EQ(results.size(), 5);
}

TEST(LimitOffsetTest, LimitWithOffset) {
    Database<UserTable> db("test_database.db", true);

    // Insert 10 records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
            UserTable::MakeTableColumn<AgeColumn>(20 + i)
        );
    }

    // Select with LIMIT 3 OFFSET 2
    auto results = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
        .LimitOffset(3, 2)
        .Results();

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(results[0]).value, "User2");
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 22);
}

TEST(LimitOffsetTest, LimitOffsetWithWhere) {
    Database<UserTable> db("test_database.db", true);

    // Insert 10 records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
            UserTable::MakeTableColumn<AgeColumn>(20 + i)
        );
    }

    // Select with WHERE, LIMIT and OFFSET
    auto results = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
        .Where(GreaterThanEqual<UserTable::TableColumn<AgeColumn>>(23))
        .LimitOffset(3, 1)
        .Results();

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 24);
}

TEST(LimitOffsetTest, OffsetExceedsTotalRows) {
    Database<UserTable> db("test_database.db", true);

    // Insert 5 records
    for (int i = 0; i < 5; i++) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
            UserTable::MakeTableColumn<AgeColumn>(20 + i)
        );
    }

    // Select with OFFSET greater than total rows
    auto results = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
        .LimitOffset(5, 10)
        .Results();

    EXPECT_EQ(results.size(), 0);
}

TEST(LimitOffsetTest, LimitExceedsTotalRows) {
    Database<UserTable> db("test_database.db", true);

    // Insert 5 records
    for (int i = 0; i < 5; i++) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
            UserTable::MakeTableColumn<AgeColumn>(20 + i)
        );
    }

    // Select with LIMIT greater than total rows
    auto results = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
        .LimitOffset(100)
        .Results();

    EXPECT_EQ(results.size(), 5);
}

TEST(LimitOffsetTest, Pagination) {
    Database<UserTable> db("test_database.db", true);

    // Insert 10 records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
            UserTable::MakeTableColumn<AgeColumn>(20 + i)
        );
    }

    // Simulate pagination: page size = 3
    int pageSize = 3;

    // Page 1 (offset 0)
    auto page1 = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>>()
        .LimitOffset(pageSize, 0)
        .Results();
    EXPECT_EQ(page1.size(), 3);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(page1[0]).value, "User0");

    // Page 2 (offset 3)
    auto page2 = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>>()
        .LimitOffset(pageSize, 3)
        .Results();
    EXPECT_EQ(page2.size(), 3);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(page2[0]).value, "User3");

    // Page 3 (offset 6)
    auto page3 = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>>()
        .LimitOffset(pageSize, 6)
        .Results();
    EXPECT_EQ(page3.size(), 3);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(page3[0]).value, "User6");

    // Page 4 (offset 9, only 1 record left)
    auto page4 = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>>()
        .LimitOffset(pageSize, 9)
        .Results();
    EXPECT_EQ(page4.size(), 1);
    EXPECT_EQ(std::get<UserTable::TableColumn<NameColumn>>(page4[0]).value, "User9");
}

TEST(LimitOffsetTest, ZeroLimit) {
    Database<UserTable> db("test_database.db", true);

    // Insert 5 records
    for (int i = 0; i < 5; i++) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
            UserTable::MakeTableColumn<AgeColumn>(20 + i)
        );
    }

    // Select with LIMIT 0
    auto results = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
        .LimitOffset(0)
        .Results();

    EXPECT_EQ(results.size(), 0);
}

TEST(LimitOffsetTest, LimitOffsetChainedAfterWhere) {
    Database<UserTable> db("test_database.db", true);

    // Insert records
    for (int i = 0; i < 10; i++) {
        db.GetTable<UserTable>().Insert(
            UserTable::MakeTableColumn<NameColumn>("User" + std::to_string(i)),
            UserTable::MakeTableColumn<AgeColumn>(20 + i)
        );
    }

    // Chain Where then LimitOffset
    auto results = db.GetTable<UserTable>()
        .Select<UserTable::TableColumn<NameColumn>, UserTable::TableColumn<AgeColumn>>()
        .Where(LessThan<UserTable::TableColumn<AgeColumn>>(28))
        .LimitOffset(2, 1)
        .Results();

    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[0]).value, 21);
    EXPECT_EQ(std::get<UserTable::TableColumn<AgeColumn>>(results[1]).value, 22);
}


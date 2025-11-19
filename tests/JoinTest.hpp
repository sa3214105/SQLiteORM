#pragma once
#include "Common.hpp"

// ============ 基本 Join 測試類別 (UserTable + DeptTable) ============
class BasicJoinTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition), decltype(DeptTableDefinition)> db =
        Database{"test_database.db", UserTableDefinition, DeptTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();
    Table<decltype(DeptTableDefinition)> &deptTable = db.GetTable<decltype(DeptTableDefinition)>();

    void SetUp() override {
        // 每個測試前會重新建立資料庫，各測試自行插入所需資料
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// ============ Join 測試 ============
TEST_F(BasicJoinTest, InnerJoinBasic) {
    // UserTable: Alice, Bob, Charlie, David
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 20);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 30);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Charlie", 40);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("David", 50);

    // DeptTable: Alice, David (只有交集部分)
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("HR", "Alice");
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("IT", "David");

    // InnerJoin on NameColumn - 只返回交集 (Alice, David)
    auto results = userTable.InnerJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
            .Select(userTable[NameColumn], userTable[AgeColumn], deptTable[DeptColumn])
            .Results();

    // INNER JOIN 只有交集: Alice, David
    EXPECT_EQ(results.size(), 2);
    std::vector<std::string> names;
    for (const auto &[name, age, dept]: results) {
        names.push_back(name);
    }
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Alice") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "David") != names.end());
}

TEST_F(BasicJoinTest, LeftJoinBasic) {
    // UserTable: Alice, Bob, Charlie, David
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 20);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 30);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Charlie", 40);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("David", 50);

    // DeptTable: Alice, David, Eve
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("HR", "Alice");
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("IT", "David");
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("Sales", "Eve");

    // LeftJoin - 返回左表的所有行 (Alice, Bob, Charlie, David)
    auto results = userTable.LeftJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
            .Select(userTable[NameColumn], userTable[AgeColumn], deptTable[DeptColumn])
            .Results();

    // LEFT JOIN 應返回左表的所有行
    EXPECT_EQ(results.size(), 4);
    std::vector<std::string> names;
    for (const auto &[name, age, dept]: results) {
        names.push_back(name);
    }
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Alice") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Bob") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Charlie") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "David") != names.end());
}

TEST_F(BasicJoinTest, RightJoinBasic) {
    // UserTable: Alice, Bob
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 20);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 30);

    // DeptTable: Alice, David, Eve (比左表多)
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("HR", "Alice");
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("IT", "David");
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("Sales", "Eve");

    // RightJoin - 返回右表的所有行 (Alice, David, Eve)
    auto results = userTable.RightJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
            .Select(userTable[NameColumn], userTable[AgeColumn], deptTable[DeptColumn])
            .Results();

    // RIGHT JOIN 應返回右表的所有行 - 至少應該包含右表中的所有行
    EXPECT_GE(results.size(), 3);
}

TEST_F(BasicJoinTest, CrossJoinBasic) {
    // UserTable: Alice, Bob
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 20);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 30);

    // DeptTable: HR, IT
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("HR", "X");
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("IT", "Y");

    // CrossJoin - 笛卡爾乘積 (2 x 2 = 4)
    auto results = userTable.CrossJoin(deptTable, 1_expr == 1_expr)
            .Select(userTable[NameColumn], userTable[AgeColumn], deptTable[DeptColumn])
            .Results();

    // CROSS JOIN 應返回笛卡爾乘積的結果 (2 x 2 = 4)
    EXPECT_EQ(results.size(), 4);
}

TEST_F(BasicJoinTest, FullJoinBasic) {
    // UserTable: Alice, Bob, Charlie, David
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Alice", 20);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Bob", 30);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("Charlie", 40);
    userTable.Insert<decltype(NameColumn), decltype(AgeColumn)>("David", 50);

    // DeptTable: Alice, David
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("HR", "Alice");
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("IT", "David");

    // FullJoin on NameColumn
    auto results = userTable.FullJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
            .Select(userTable[NameColumn], userTable[AgeColumn], deptTable[DeptColumn])
            .Results();

    // 應該有 4 筆: Alice(交集), Bob(左), Charlie(左), David(交集)
    std::vector<std::string> names;
    std::vector<std::string> depts;
    for (const auto &[name, age, dept]: results) {
        names.push_back(name);
        depts.push_back(dept);
    }
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Alice") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Bob") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Charlie") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "David") != names.end());
    EXPECT_TRUE(std::find(depts.begin(), depts.end(), "HR") != depts.end());
    EXPECT_TRUE(std::find(depts.begin(), depts.end(), "IT") != depts.end());
    EXPECT_EQ(results.size(), 4);
}

// ============ 多重 Join 測試類別 (UserTable + CityTable + CountryTable) ============
class MultiJoinTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition), decltype(CityTableDefinition), decltype(CountryTableDefinition)> db =
        Database{"test_database.db", UserTableDefinition, CityTableDefinition, CountryTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();
    Table<decltype(CityTableDefinition)> &cityTable = db.GetTable<decltype(CityTableDefinition)>();
    Table<decltype(CountryTableDefinition)> &countryTable = db.GetTable<decltype(CountryTableDefinition)>();

    void SetUp() override {
        // 每個測試前會重新建立資料庫
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

// ============ 多重 Join 測試 ============
TEST_F(MultiJoinTest, MultiJoin_Inner_Inner_ThreeTables) {
    // users
    userTable.Insert<decltype(NameColumn)>("Alice");
    userTable.Insert<decltype(NameColumn)>("Bob");
    userTable.Insert<decltype(NameColumn)>("Eve");

    // cities
    cityTable.Insert<decltype(NameColumn), decltype(CityColumn)>("Alice", "Paris");
    cityTable.Insert<decltype(NameColumn), decltype(CityColumn)>("Bob", "Tokyo");
    cityTable.Insert<decltype(NameColumn), decltype(CityColumn)>("Mallory", "Berlin");

    // countries
    countryTable.Insert<decltype(CityColumn), decltype(CountryColumn)>("Paris", "France");
    countryTable.Insert<decltype(CityColumn), decltype(CountryColumn)>("Tokyo", "Japan");
    countryTable.Insert<decltype(CityColumn), decltype(CountryColumn)>("Berlin", "Germany");

    auto results = userTable
            .InnerJoin(cityTable, userTable[NameColumn] == cityTable[NameColumn])
            .InnerJoin(countryTable, cityTable[CityColumn] == countryTable[CityColumn])
            .Select(userTable[NameColumn], cityTable[CityColumn], countryTable[CountryColumn])
            .Results();

    ASSERT_EQ(results.size(), 2);
    std::vector<std::string> countries;
    for (auto &[name, city, country]: results) {
        countries.push_back(country);
    }
    EXPECT_NE(std::find(countries.begin(), countries.end(), "France"), countries.end());
    EXPECT_NE(std::find(countries.begin(), countries.end(), "Japan"), countries.end());
}

TEST_F(MultiJoinTest, MultiJoin_Left_Left_ThreeTables) {
    // users
    userTable.Insert<decltype(NameColumn)>("Alice");
    userTable.Insert<decltype(NameColumn)>("Bob");
    userTable.Insert<decltype(NameColumn)>("Charlie");

    // cities (Charlie has no city)
    cityTable.Insert<decltype(NameColumn), decltype(CityColumn)>("Alice", "Paris");
    cityTable.Insert<decltype(NameColumn), decltype(CityColumn)>("Bob", "Tokyo");

    // countries (Tokyo has no country)
    countryTable.Insert<decltype(CityColumn), decltype(CountryColumn)>("Paris", "France");

    auto results = userTable
            .LeftJoin(cityTable, userTable[NameColumn] == cityTable[NameColumn])
            .LeftJoin(countryTable, cityTable[CityColumn] == countryTable[CityColumn])
            .Select(userTable[NameColumn], cityTable[CityColumn], countryTable[CountryColumn])
            .Results();

    ASSERT_EQ(results.size(), 3);
    // Ensure Charlie exists with empty city/country
    bool hasCharlie = false;
    bool hasEmptyForCharlie = false;
    for (auto &[name, city, country]: results) {
        if (name == "Charlie") {
            hasCharlie = true;
            hasEmptyForCharlie = city.empty() && country.empty();
        }
    }
    EXPECT_TRUE(hasCharlie);
    EXPECT_TRUE(hasEmptyForCharlie);
}

// ============ 混合 Join 測試類別 (UserTable + DeptTable + CityTable) ============
class MixedJoinTest : public ::testing::Test {
protected:
    Database<decltype(UserTableDefinition), decltype(DeptTableDefinition), decltype(CityTableDefinition)> db =
        Database{"test_database.db", UserTableDefinition, DeptTableDefinition, CityTableDefinition};
    Table<decltype(UserTableDefinition)> &userTable = db.GetTable<decltype(UserTableDefinition)>();
    Table<decltype(DeptTableDefinition)> &deptTable = db.GetTable<decltype(DeptTableDefinition)>();
    Table<decltype(CityTableDefinition)> &cityTable = db.GetTable<decltype(CityTableDefinition)>();

    void SetUp() override {
        // 每個測試前會重新建立資料庫
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

TEST_F(MixedJoinTest, MultiJoin_Inner_Then_Left) {
    // users
    userTable.Insert<decltype(NameColumn)>("Alice");
    userTable.Insert<decltype(NameColumn)>("Bob");
    userTable.Insert<decltype(NameColumn)>("Eve");

    // departments (only Alice and Bob)
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("HR", "Alice");
    deptTable.Insert<decltype(DeptColumn), decltype(NameColumn)>("IT", "Bob");

    // cities (only Alice)
    cityTable.Insert<decltype(NameColumn), decltype(CityColumn)>("Alice", "Paris");

    auto results = userTable
            .InnerJoin(deptTable, userTable[NameColumn] == deptTable[NameColumn])
            .LeftJoin(cityTable, userTable[NameColumn] == cityTable[NameColumn])
            .Select(userTable[NameColumn], deptTable[DeptColumn], cityTable[CityColumn])
            .Results();

    // Only Alice and Bob (inner with Dept), with city only for Alice
    ASSERT_EQ(results.size(), 2);
    for (auto &[name, dept, city]: results) {
        if (name == "Alice") {
            EXPECT_EQ(dept, "HR");
            EXPECT_EQ(city, "Paris");
        }
        if (name == "Bob") {
            EXPECT_EQ(dept, "IT");
            EXPECT_TRUE(city.empty());
        }
    }
}
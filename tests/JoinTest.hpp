#pragma once
#include "Common.hpp"

// ============ Join 測試 ============
TEST(JoinTest, InnerJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Alice",20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Bob",30);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Charlie",40);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("David",50);
    // DeptTable: Alice, David (只有交集部分)
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("HR","Alice");
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("IT","David");

    // InnerJoin on NameColumn - 只返回交集 (Alice, David)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().InnerJoin<DeptTable>(_NameColumn() == __NameColumn()).
            Select(_NameColumn(), _AgeColumn(), _DeptColumn()).Results();

    // INNER JOIN 只有交集: Alice, David
    EXPECT_EQ(results.size(), 2);
    std::vector<std::string> names;
    for (const auto &[name,age,dept]: results) {
        names.push_back(name);
    }
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Alice") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "David") != names.end());
}

TEST(JoinTest, LeftJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Alice", 20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Bob", 30);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Charlie", 40);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("David", 50);
    // DeptTable: Alice, David, Eve
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("HR", "Alice");
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("IT", "David");
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("Sales", "Eve");

    // LeftJoin - 返回左表的所有行 (Alice, Bob, Charlie, David)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().LeftJoin<DeptTable>(_NameColumn() == __NameColumn()).
            Select(_NameColumn(), _AgeColumn(), _DeptColumn()).Results();

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

TEST(JoinTest, RightJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Alice", 20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Bob", 30);
    // DeptTable: Alice, David, Eve (比左表多)
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("HR", "Alice");
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("IT", "David");
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("Sales", "Eve");

    // RightJoin - 返回右表的所有行 (Alice, David, Eve)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().RightJoin<DeptTable>(_NameColumn() == __NameColumn()).
            Select(_NameColumn(), _AgeColumn(), _DeptColumn()).Results();

    // RIGHT JOIN 應返回右表的所有行 - 至少應該包含右表中的所有行
    EXPECT_GE(results.size(), 3);
}

TEST(JoinTest, CrossJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Alice", 20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Bob", 30);
    // DeptTable: HR, IT
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("HR", "X");
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("IT", "Y");

    // CrossJoin - 笛卡爾乘積 (2 x 2 = 4)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;

    auto results = db.GetTable<UserTable>().CrossJoin<DeptTable>(1_expr == 1_expr).
            Select(_NameColumn(), _AgeColumn(), _DeptColumn()).Results();

    // CROSS JOIN 應返回笛卡爾乘積的結果 (2 x 2 = 4)
    EXPECT_EQ(results.size(), 4);
}

TEST(JoinTest, FullJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Alice", 20);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Bob", 30);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("Charlie", 40);
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>,
                                    UserTable::TableColumn<AgeColumn>>("David", 50);
    // DeptTable: Alice, David
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("HR", "Alice");
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("IT", "David");
    // FullJoin on NameColumn
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;
    auto results = db.GetTable<UserTable>().FullJoin<DeptTable>(_NameColumn() == __NameColumn()).
            Select(_NameColumn(), _AgeColumn(), _DeptColumn()).Results();
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

// ============ 多重 Join 測試 ============
TEST(JoinTest, MultiJoin_Inner_Inner_ThreeTables) {
    Database<UserTable, CityTable, CountryTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<CityTable>().Delete().Execute();
    db.GetTable<CountryTable>().Delete().Execute();

    // users
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Alice");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Bob");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Eve");

    // cities
    db.GetTable<CityTable>().Insert<CityTable::TableColumn<NameColumn>,
                                    CityTable::TableColumn<CityColumn>>("Alice", "Paris");
    db.GetTable<CityTable>().Insert<CityTable::TableColumn<NameColumn>,
                                    CityTable::TableColumn<CityColumn>>("Bob", "Tokyo");
    db.GetTable<CityTable>().Insert<CityTable::TableColumn<NameColumn>,
                                    CityTable::TableColumn<CityColumn>>("Mallory", "Berlin");

    // countries
    db.GetTable<CountryTable>().Insert<CountryTable::TableColumn<CityColumn>,
                                       CountryTable::TableColumn<CountryColumn>>("Paris", "France");
    db.GetTable<CountryTable>().Insert<CountryTable::TableColumn<CityColumn>,
                                       CountryTable::TableColumn<CountryColumn>>("Tokyo", "Japan");
    db.GetTable<CountryTable>().Insert<CountryTable::TableColumn<CityColumn>,
                                       CountryTable::TableColumn<CountryColumn>>("Berlin", "Germany");

    using UName = UserTable::TableColumn<NameColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using UCity = CityTable::TableColumn<CityColumn>;
    using CCty = CountryTable::TableColumn<CityColumn>;
    using UCountry = CountryTable::TableColumn<CountryColumn>;

    auto results = db.GetTable<UserTable>()
            .InnerJoin<CityTable>(UName() == CName())
            .InnerJoin<CountryTable>(UCity() == CCty())
            .Select(UName(), UCity(), UCountry())
            .Results();

    ASSERT_EQ(results.size(), 2);
    std::vector<std::string> countries;
    for (auto &[name, city, country]: results) {
        countries.push_back(country);
    }
    EXPECT_NE(std::find(countries.begin(), countries.end(), "France"), countries.end());
    EXPECT_NE(std::find(countries.begin(), countries.end(), "Japan"), countries.end());
}

TEST(JoinTest, MultiJoin_Left_Left_ThreeTables) {
    Database<UserTable, CityTable, CountryTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<CityTable>().Delete().Execute();
    db.GetTable<CountryTable>().Delete().Execute();

    // users
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Alice");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Bob");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Charlie");

    // cities (Charlie has no city)
    db.GetTable<CityTable>().Insert<CityTable::TableColumn<NameColumn>,
                                    CityTable::TableColumn<CityColumn>>("Alice", "Paris");
    db.GetTable<CityTable>().Insert<CityTable::TableColumn<NameColumn>,
                                    CityTable::TableColumn<CityColumn>>("Bob", "Tokyo");

    // countries (Tokyo has no country)
    db.GetTable<CountryTable>().Insert<CountryTable::TableColumn<CityColumn>,
                                       CountryTable::TableColumn<CountryColumn>>("Paris", "France");

    using UName = UserTable::TableColumn<NameColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using UCity = CityTable::TableColumn<CityColumn>;
    using CCty = CountryTable::TableColumn<CityColumn>;
    using UCountry = CountryTable::TableColumn<CountryColumn>;

    auto results = db.GetTable<UserTable>()
            .LeftJoin<CityTable>(UName() == CName())
            .LeftJoin<CountryTable>(UCity() == CCty())
            .Select(UName(), UCity(), UCountry())
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

TEST(JoinTest, MultiJoin_Inner_Then_Left) {
    Database<UserTable, DeptTable, CityTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    db.GetTable<CityTable>().Delete().Execute();

    // users
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Alice");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Bob");
    db.GetTable<UserTable>().Insert<UserTable::TableColumn<NameColumn>>("Eve");

    // departments (only Alice and Bob)
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("HR", "Alice");
    db.GetTable<DeptTable>().Insert<DeptTable::TableColumn<DeptColumn>,
                                    DeptTable::TableColumn<NameColumn>>("IT", "Bob");

    // cities (only Alice)
    db.GetTable<CityTable>().Insert<CityTable::TableColumn<NameColumn>,
                                    CityTable::TableColumn<CityColumn>>("Alice", "Paris");

    using UName = UserTable::TableColumn<NameColumn>;
    using DName = DeptTable::TableColumn<NameColumn>;
    using Dept = DeptTable::TableColumn<DeptColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using City = CityTable::TableColumn<CityColumn>;

    auto results = db.GetTable<UserTable>()
            .InnerJoin<DeptTable>(UName() == DName())
            .LeftJoin<CityTable>(UName() == CName())
            .Select(UName(), Dept(), City())
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
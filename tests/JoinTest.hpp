#pragma once
#include "Common.hpp"

// ============ Join 測試 ============
TEST(JoinTest, InnerJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(40));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"),
                                    UserTable::MakeTableColumn<AgeColumn>(50));
    // DeptTable: Alice, David (只有交集部分)
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("David"));

    // InnerJoin on NameColumn - 只返回交集 (Alice, David)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().InnerJoin<DeptTable>(Equal<_NameColumn, __NameColumn>()).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // INNER JOIN 只有交集: Alice, David
    EXPECT_EQ(results.size(), 2);
    std::vector<std::string> names;
    for (const auto &row: results) {
        names.push_back(std::get<_NameColumn>(row).value);
    }
    EXPECT_TRUE(std::find(names.begin(), names.end(), "Alice") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "David") != names.end());
}

TEST(JoinTest, LeftJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie, David
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(40));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"),
                                    UserTable::MakeTableColumn<AgeColumn>(50));
    // DeptTable: Alice, David, Eve
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("David"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("Sales"),
                                    DeptTable::MakeTableColumn<NameColumn>("Eve"));

    // LeftJoin - 返回左表的所有行 (Alice, Bob, Charlie, David)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().LeftJoin<DeptTable>(Equal<_NameColumn, __NameColumn>()).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // LEFT JOIN 應返回左表的所有行
    EXPECT_EQ(results.size(), 4);
    std::vector<std::string> names;
    for (const auto &row: results) {
        names.push_back(std::get<_NameColumn>(row).value);
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
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    // DeptTable: Alice, David, Eve (比左表多)
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("David"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("Sales"),
                                    DeptTable::MakeTableColumn<NameColumn>("Eve"));

    // RightJoin - 返回右表的所有行 (Alice, David, Eve)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;

    auto results = db.GetTable<UserTable>().RightJoin<DeptTable>(Equal<_NameColumn, __NameColumn>()).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // RIGHT JOIN 應返回右表的所有行 - 至少應該包含右表中的所有行
    EXPECT_GE(results.size(), 3);
}

TEST(JoinTest, CrossJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db");
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    // DeptTable: HR, IT
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("X"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("Y"));

    // CrossJoin - 笛卡爾乘積 (2 x 2 = 4)
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;

    auto results = db.GetTable<UserTable>().CrossJoin<DeptTable>(Equal(1, 1)).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();

    // CROSS JOIN 應返回笛卡爾乘積的結果 (2 x 2 = 4)
    EXPECT_EQ(results.size(), 4);
}

TEST(JoinTest, FullJoinBasic) {
    Database<UserTable, DeptTable> db("test_database.db",true);
    db.GetTable<UserTable>().Delete().Execute();
    db.GetTable<DeptTable>().Delete().Execute();
    // UserTable: Alice, Bob, Charlie
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"),
                                    UserTable::MakeTableColumn<AgeColumn>(20));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"),
                                    UserTable::MakeTableColumn<AgeColumn>(30));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"),
                                    UserTable::MakeTableColumn<AgeColumn>(40));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("David"),
                                    UserTable::MakeTableColumn<AgeColumn>(50));
    // DeptTable: Alice, David
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("David"));
    // FullJoin on NameColumn
    using _NameColumn = UserTable::TableColumn<NameColumn>;
    using _AgeColumn = UserTable::TableColumn<AgeColumn>;
    using _DeptColumn = DeptTable::TableColumn<DeptColumn>;
    using __NameColumn = DeptTable::TableColumn<NameColumn>;
    auto results = db.GetTable<UserTable>().FullJoin<DeptTable>(Equal<_NameColumn, __NameColumn>()).
            Select<_NameColumn, _AgeColumn, _DeptColumn>().Results();
    // 應該有 4 筆: Alice(交集), Bob(左), Charlie(左), David(右)
    std::vector<std::string> names;
    std::vector<std::string> depts;
    for (const auto &row: results) {
        names.push_back(std::get<_NameColumn>(row).value);
        depts.push_back(std::get<_DeptColumn>(row).value);
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
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Eve"));

    // cities
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Alice"),
                                    CityTable::MakeTableColumn<CityColumn>("Paris"));
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Bob"),
                                    CityTable::MakeTableColumn<CityColumn>("Tokyo"));
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Mallory"),
                                    CityTable::MakeTableColumn<CityColumn>("Berlin"));

    // countries
    db.GetTable<CountryTable>().Insert(CountryTable::MakeTableColumn<CityColumn>("Paris"),
                                       CountryTable::MakeTableColumn<CountryColumn>("France"));
    db.GetTable<CountryTable>().Insert(CountryTable::MakeTableColumn<CityColumn>("Tokyo"),
                                       CountryTable::MakeTableColumn<CountryColumn>("Japan"));
    db.GetTable<CountryTable>().Insert(CountryTable::MakeTableColumn<CityColumn>("Berlin"),
                                       CountryTable::MakeTableColumn<CountryColumn>("Germany"));

    using UName = UserTable::TableColumn<NameColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using UCity = CityTable::TableColumn<CityColumn>;
    using CCty = CountryTable::TableColumn<CityColumn>;
    using UCountry = CountryTable::TableColumn<CountryColumn>;

    auto results = db.GetTable<UserTable>()
            .InnerJoin<CityTable>(Equal<UName, CName>())
            .InnerJoin<CountryTable>(Equal<UCity, CCty>())
            .Select<UName, UCity, UCountry>()
            .Results();

    ASSERT_EQ(results.size(), 2);
    std::vector<std::string> countries;
    for (auto &row: results) {
        countries.push_back(std::get<UCountry>(row).value);
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
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Charlie"));

    // cities (Charlie has no city)
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Alice"),
                                    CityTable::MakeTableColumn<CityColumn>("Paris"));
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Bob"),
                                    CityTable::MakeTableColumn<CityColumn>("Tokyo"));

    // countries (Tokyo has no country)
    db.GetTable<CountryTable>().Insert(CountryTable::MakeTableColumn<CityColumn>("Paris"),
                                       CountryTable::MakeTableColumn<CountryColumn>("France"));

    using UName = UserTable::TableColumn<NameColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using UCity = CityTable::TableColumn<CityColumn>;
    using CCty = CountryTable::TableColumn<CityColumn>;
    using UCountry = CountryTable::TableColumn<CountryColumn>;

    auto results = db.GetTable<UserTable>()
            .LeftJoin<CityTable>(Equal<UName, CName>())
            .LeftJoin<CountryTable>(Equal<UCity, CCty>())
            .Select<UName, UCity, UCountry>()
            .Results();

    ASSERT_EQ(results.size(), 3);
    // Ensure Charlie exists with empty city/country
    bool hasCharlie = false;
    bool hasEmptyForCharlie = false;
    for (auto &row: results) {
        if (std::get<UName>(row).value == "Charlie") {
            hasCharlie = true;
            hasEmptyForCharlie = std::get<UCity>(row).value.empty() && std::get<UCountry>(row).value.empty();
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
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Bob"));
    db.GetTable<UserTable>().Insert(UserTable::MakeTableColumn<NameColumn>("Eve"));

    // departments (only Alice and Bob)
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("HR"),
                                    DeptTable::MakeTableColumn<NameColumn>("Alice"));
    db.GetTable<DeptTable>().Insert(DeptTable::MakeTableColumn<DeptColumn>("IT"),
                                    DeptTable::MakeTableColumn<NameColumn>("Bob"));

    // cities (only Alice)
    db.GetTable<CityTable>().Insert(CityTable::MakeTableColumn<NameColumn>("Alice"),
                                    CityTable::MakeTableColumn<CityColumn>("Paris"));

    using UName = UserTable::TableColumn<NameColumn>;
    using DName = DeptTable::TableColumn<NameColumn>;
    using Dept = DeptTable::TableColumn<DeptColumn>;
    using CName = CityTable::TableColumn<NameColumn>;
    using City = CityTable::TableColumn<CityColumn>;

    auto results = db.GetTable<UserTable>()
            .InnerJoin<DeptTable>(Equal<UName, DName>())
            .LeftJoin<CityTable>(Equal<UName, CName>())
            .Select<UName, Dept, City>()
            .Results();

    // Only Alice and Bob (inner with Dept), with city only for Alice
    ASSERT_EQ(results.size(), 2);
    for (auto &row: results) {
        if (std::get<UName>(row).value == "Alice") {
            EXPECT_EQ(std::get<Dept>(row).value, std::string("HR"));
            EXPECT_EQ(std::get<City>(row).value, std::string("Paris"));
        }
        if (std::get<UName>(row).value == "Bob") {
            EXPECT_EQ(std::get<Dept>(row).value, std::string("IT"));
            EXPECT_TRUE(std::get<City>(row).value.empty());
        }
    }
}
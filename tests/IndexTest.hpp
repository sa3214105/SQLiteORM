#pragma once
#include "Common.hpp"

// 測試單一欄位索引
class IndexTest_SingleColumn : public ::testing::Test {
protected:
    inline static auto indexDef = MakeIndexDefinition<"idx_name", decltype(UserTableDefinition), std::tuple<decltype(NameColumn)>, true>(std::make_tuple(NameColumn));
    Database<decltype(UserTableDefinition), decltype(indexDef)> db = Database{"test_index.db", UserTableDefinition, indexDef};
    Table<decltype(UserTableDefinition)> &table = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 85.5);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 90.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Charlie", 28, 88.5);
    }

    void TearDown() override {
        std::remove("test_index.db");
    }
};

TEST_F(IndexTest_SingleColumn, QueryWithIndex) {
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn])
                       .Where(table[NameColumn] == "Bob"_expr)
                       .Results();

    ASSERT_EQ(results.size(), 1);
    auto &[name, age, score] = results[0];
    EXPECT_EQ(name, "Bob");
    EXPECT_EQ(age, 30);
    EXPECT_DOUBLE_EQ(score, 90.0);
}

// 測試複合欄位索引
class IndexTest_Composite : public ::testing::Test {
protected:
    inline static auto indexDef = MakeIndexDefinition<"idx_name_age", decltype(UserTableDefinition), std::tuple<decltype(NameColumn), decltype(AgeColumn)>, true>(
        std::make_tuple(NameColumn, AgeColumn)
    );
    Database<decltype(UserTableDefinition), decltype(indexDef)> db = Database{"test_index_composite.db", UserTableDefinition, indexDef};
    Table<decltype(UserTableDefinition)> &table = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 85.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 30, 88.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 25, 90.0);
    }

    void TearDown() override {
        std::remove("test_index_composite.db");
    }
};

TEST_F(IndexTest_Composite, QueryCompositeIndex) {
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn])
                       .Where(table[NameColumn] == "Alice"_expr && table[AgeColumn] == 30_expr)
                       .Results();

    ASSERT_EQ(results.size(), 1);
    auto &[name, age, score] = results[0];
    EXPECT_EQ(name, "Alice");
    EXPECT_EQ(age, 30);
    EXPECT_DOUBLE_EQ(score, 88.0);
}

// 測試非唯一索引
class IndexTest_NonUnique : public ::testing::Test {
protected:
    inline static auto indexDef = MakeIndexDefinition<"idx_age", decltype(UserTableDefinition), std::tuple<decltype(AgeColumn)>, false>(
        std::make_tuple(AgeColumn)
    );
    Database<decltype(UserTableDefinition), decltype(indexDef)> db = Database{"test_index_nonunique.db", UserTableDefinition, indexDef};
    Table<decltype(UserTableDefinition)> &table = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Product1", 25, 100.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Product2", 25, 200.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Product3", 25, 150.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Product4", 30, 20.0);
    }

    void TearDown() override {
        std::remove("test_index_nonunique.db");
    }
};

TEST_F(IndexTest_NonUnique, QueryNonUniqueIndex) {
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn])
                       .Where(table[AgeColumn] == 25_expr)
                       .Results();

    ASSERT_EQ(results.size(), 3);
    for (const auto &[name, age, score] : results) {
        EXPECT_EQ(age, 25);
    }
}

// 測試多個索引
class IndexTest_Multiple : public ::testing::Test {
protected:
    inline static auto nameIndexDef = MakeIndexDefinition<"idx_name", decltype(UserTableDefinition), std::tuple<decltype(NameColumn)>, true>(
        std::make_tuple(NameColumn)
    );
    inline static auto ageIndexDef = MakeIndexDefinition<"idx_age", decltype(UserTableDefinition), std::tuple<decltype(AgeColumn)>, false>(
        std::make_tuple(AgeColumn)
    );
    Database<decltype(UserTableDefinition), decltype(nameIndexDef), decltype(ageIndexDef)> db = Database{"test_index_multiple.db", UserTableDefinition, nameIndexDef, ageIndexDef};
    Table<decltype(UserTableDefinition)> &table = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 85.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 90.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Charlie", 25, 88.0);
    }

    void TearDown() override {
        std::remove("test_index_multiple.db");
    }
};

TEST_F(IndexTest_Multiple, QueryWithNameIndex) {
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn])
                       .Where(table[NameColumn] == "Bob"_expr)
                       .Results();
    ASSERT_EQ(results.size(), 1);
    auto &[name, age, score] = results[0];
    EXPECT_EQ(name, "Bob");
}

TEST_F(IndexTest_Multiple, QueryWithAgeIndex) {
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn])
                       .Where(table[AgeColumn] == 25_expr)
                       .Results();
    ASSERT_EQ(results.size(), 2);
}

// 測試唯一索引約束
class IndexTest_UniqueConstraint : public ::testing::Test {
protected:
    inline static auto indexDef = MakeIndexDefinition<"idx_unique_name", decltype(UserTableDefinition), std::tuple<decltype(NameColumn)>, true>(
        std::make_tuple(NameColumn)
    );
    Database<decltype(UserTableDefinition), decltype(indexDef)> db = Database{"test_index_unique.db", UserTableDefinition, indexDef};
    Table<decltype(UserTableDefinition)> &table = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Test1", 25, 85.0);
    }

    void TearDown() override {
        std::remove("test_index_unique.db");
    }
};

TEST_F(IndexTest_UniqueConstraint, ViolateUniqueConstraint) {
    // 嘗試插入重複的 name (應該失敗)
    EXPECT_THROW((table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Test1", 30, 90.0)), std::runtime_error);

    // 確認只有一筆資料
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn]).Results();
    ASSERT_EQ(results.size(), 1);
}

// 測試複合唯一索引
class IndexTest_CompositeUnique : public ::testing::Test {
protected:
    inline static auto indexDef = MakeIndexDefinition<"idx_unique_name_age", decltype(UserTableDefinition), std::tuple<decltype(NameColumn), decltype(AgeColumn)>, true>(
        std::make_tuple(NameColumn, AgeColumn)
    );
    Database<decltype(UserTableDefinition), decltype(indexDef)> db = Database{"test_index_comp_unique.db", UserTableDefinition, indexDef};
    Table<decltype(UserTableDefinition)> &table = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("John", 25, 85.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("John", 30, 88.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Jane", 25, 90.0);
    }

    void TearDown() override {
        std::remove("test_index_comp_unique.db");
    }
};

TEST_F(IndexTest_CompositeUnique, ViolateCompositeUnique) {
    // 嘗試插入重複的名字組合
    EXPECT_THROW(({
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("John", 25, 95.0);
    }), std::runtime_error);

    // 確認資料數量
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn]).Results();
    ASSERT_EQ(results.size(), 3);
}

// 測試索引對排序查詢的影響
class IndexTest_OrderBy : public ::testing::Test {
protected:
    inline static auto indexDef = MakeIndexDefinition<"idx_score", decltype(UserTableDefinition), std::tuple<decltype(ScoreColumn)>, false>(
        std::make_tuple(ScoreColumn)
    );
    Database<decltype(UserTableDefinition), decltype(indexDef)> db = Database{"test_index_orderby.db", UserTableDefinition, indexDef};
    Table<decltype(UserTableDefinition)> &table = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Alice", 25, 85.5);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Bob", 30, 92.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Charlie", 28, 78.5);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("David", 35, 95.0);
    }

    void TearDown() override {
        std::remove("test_index_orderby.db");
    }
};

TEST_F(IndexTest_OrderBy, SortByIndexedColumn) {
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn])
                       .OrderBy(table[ScoreColumn], OrderType::DESC)
                       .Results();

    ASSERT_EQ(results.size(), 4);
    auto &[name1, age1, score1] = results[0];
    EXPECT_EQ(name1, "David");
    EXPECT_DOUBLE_EQ(score1, 95.0);

    auto &[name2, age2, score2] = results[1];
    EXPECT_EQ(name2, "Bob");
    EXPECT_DOUBLE_EQ(score2, 92.0);
}

// 測試範圍查詢與索引
class IndexTest_RangeQuery : public ::testing::Test {
protected:
    inline static auto indexDef = MakeIndexDefinition<"idx_score_range", decltype(UserTableDefinition), std::tuple<decltype(ScoreColumn)>, false>(
        std::make_tuple(ScoreColumn)
    );
    Database<decltype(UserTableDefinition), decltype(indexDef)> db = Database{"test_index_range.db", UserTableDefinition, indexDef};
    Table<decltype(UserTableDefinition)> &table = db.GetTable<decltype(UserTableDefinition)>();

    void SetUp() override {
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Item1", 20, 10.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Item2", 25, 25.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Item3", 30, 50.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Item4", 35, 75.0);
        table.Insert<decltype(NameColumn), decltype(AgeColumn), decltype(ScoreColumn)>("Item5", 40, 100.0);
    }

    void TearDown() override {
        std::remove("test_index_range.db");
    }
};

TEST_F(IndexTest_RangeQuery, QueryRange) {
    auto results = table.Select(table[NameColumn], table[AgeColumn], table[ScoreColumn])
                       .Where(table[ScoreColumn] >= 25.0_expr && table[ScoreColumn] <= 75.0_expr)
                       .Results();

    ASSERT_EQ(results.size(), 3);
    auto &[name1, age1, score1] = results[0];
    EXPECT_EQ(name1, "Item2");
    auto &[name2, age2, score2] = results[1];
    EXPECT_EQ(name2, "Item3");
    auto &[name3, age3, score3] = results[2];
    EXPECT_EQ(name3, "Item4");
}


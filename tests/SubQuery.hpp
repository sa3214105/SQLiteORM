#pragma once
#include "Common.hpp"

class SubQueryTest : public ::testing::Test {
protected:
    Column<"id", DataType::INTEGER, ColumnPrimaryKey<OrderType::ASC, ConflictCause::ABORT, true> > IdColumn;
    Column<"name", DataType::TEXT> NameColumn;
    Column<"user_id", DataType::INTEGER> UserColumn;
    Column<"price", DataType::REAL> PriceColumn;
    decltype(MakeTableDefinition<"users">(
        std::make_tuple(IdColumn, NameColumn)
    )) UsersDefinition = MakeTableDefinition<"users">(
        std::make_tuple(IdColumn, NameColumn)
    );
    decltype( MakeTableDefinition<"orders">(
        std::make_tuple(IdColumn, UserColumn, PriceColumn)
    )) OrderDefinition = MakeTableDefinition<"orders">(
        std::make_tuple(IdColumn, UserColumn, PriceColumn)
    );

    Database<decltype(UsersDefinition), decltype(OrderDefinition)> db = Database{
        "test_database.db",
        UsersDefinition,
        OrderDefinition
    };
    Table<decltype(UsersDefinition)> &userTable = db.GetTable<decltype(UsersDefinition)>();
    Table<decltype(OrderDefinition)> &orderTable = db.GetTable<decltype(OrderDefinition)>();

    void SetUp() override {
        // 插入用戶
        userTable.Insert<decltype(NameColumn)>("Alice");
        userTable.Insert<decltype(NameColumn)>("Bob");

        // 插入訂單
        orderTable.Insert<decltype(UserColumn), decltype(PriceColumn)>(1, 100.0);
        orderTable.Insert<decltype(UserColumn), decltype(PriceColumn)>(1, 150.0);
        orderTable.Insert<decltype(UserColumn), decltype(PriceColumn)>(2, 200.0);
    }

    void TearDown() override {
        std::remove("test_database.db");
    }
};

//TODO 支援更多子查詢測試
TEST_F(SubQueryTest, SimpleSubQuery) {
    // 子查詢：選擇有訂單的用戶
    auto subQuery = orderTable.Select(orderTable[UserColumn]).Distinct();

    auto x = orderTable.Select(
        Count(orderTable[UserColumn])
    ).Where(orderTable[UserColumn] == userTable[IdColumn]).resultType;
    // 主查詢：選擇用戶名稱，條件是用戶ID在子查詢結果中
    auto results = userTable.Select(
                userTable[IdColumn],
                userTable[NameColumn],
                orderTable.Select(
                    Count(orderTable[UserColumn])
                ).Where(orderTable[UserColumn] == userTable[IdColumn])
            )
            .Results()
            .ToVector();

    ASSERT_EQ(results.size(), 2);
}

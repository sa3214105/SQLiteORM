#pragma once
#include <gtest/gtest.h>
#include "../src/TypeSQlite.hpp"

using namespace TypeSQLite;
inline Column<"name", ExprResultType::TEXT> NameColumn;
inline Column<"age", ExprResultType::INTEGER> AgeColumn;
inline Column<"score", ExprResultType::REAL> ScoreColumn;
inline Column<"dept", ExprResultType::TEXT> DeptColumn;
inline Column<"city", ExprResultType::TEXT> CityColumn;
inline Column<"country", ExprResultType::TEXT> CountryColumn;

// 定義表類型
inline auto UserTableDefinition = MakeTableDefinition<"name">(std::make_tuple(NameColumn, AgeColumn, ScoreColumn));
inline auto DeptTableDefinition = MakeTableDefinition<"departments">(std::make_tuple(DeptColumn, NameColumn));
inline auto CityTableDefinition = MakeTableDefinition<"cities">(std::make_tuple(NameColumn, CityColumn));
inline auto CountryTableDefinition = MakeTableDefinition<"countries">(std::make_tuple(CityColumn, CountryColumn));

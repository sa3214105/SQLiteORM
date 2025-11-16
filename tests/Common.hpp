#pragma once
#include <gtest/gtest.h>
#include "../src/TypeSQlite.hpp"

using namespace TypeSQLite;
using NameColumn = Column<"name", ExprResultType::TEXT>;
using AgeColumn = Column<"age", ExprResultType::INTEGER>;
using ScoreColumn = Column<"score", ExprResultType::REAL>;
using DeptColumn = Column<"dept", ExprResultType::TEXT>;
using CityColumn = Column<"city", ExprResultType::TEXT>;
using CountryColumn = Column<"country", ExprResultType::TEXT>;

// 定義表類型
using UserTable = Table<"users", NameColumn, AgeColumn, ScoreColumn>;
using DeptTable = Table<"departments", DeptColumn, NameColumn>;
using CityTable = Table<"cities", NameColumn, CityColumn>;
using CountryTable = Table<"countries", CityColumn, CountryColumn>;
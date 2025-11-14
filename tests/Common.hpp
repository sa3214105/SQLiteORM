#pragma once
#include <gtest/gtest.h>
#include "../src/TypeSQlite.hpp"

using namespace SQLiteHelper;
using NameColumn = Column<"name", column_type::TEXT>;
using AgeColumn = Column<"age", column_type::INTEGER>;
using ScoreColumn = Column<"score", column_type::REAL>;
using DeptColumn = Column<"dept", column_type::TEXT>;
using CityColumn = Column<"city", column_type::TEXT>;
using CountryColumn = Column<"country", column_type::TEXT>;

// 定義表類型
using UserTable = Table<"users", NameColumn, AgeColumn, ScoreColumn>;
using DeptTable = Table<"departments", DeptColumn, NameColumn>;
using CityTable = Table<"cities", NameColumn, CityColumn>;
using CountryTable = Table<"countries", CityColumn, CountryColumn>;
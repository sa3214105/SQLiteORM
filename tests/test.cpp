#include "SelectTest.hpp"
#include "InsertTest.hpp"
#include "UpdateTest.hpp"
#include "DeleteTest.hpp"
#include "UpsertTest.hpp"
#include "TransationTest.hpp"
#include "JoinTest.hpp"
#include "ColumnConstraintsTest.hpp"
#include "TableConstraintTest.hpp"
#include "TableOptionTest.hpp"
#include "BatchInsertTest.hpp"
#include "LimitOffsetTest.hpp"
#include "DistinctTest.hpp"
#include "AggregateFunctionsTest.hpp"
#include "ExprTest.hpp"
#include "IndexTest.hpp"
#include "WindowFunctionTest.hpp"
#include "MathFunctionsTest.hpp"
#include "ScalarFunctionsTest.hpp"
#include "DateTimeFunctionsTest.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

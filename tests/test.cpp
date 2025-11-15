#include "SelectTest.hpp"
#include "InsertTest.hpp"
#include "UpdateTest.hpp"
#include "DeleteTest.hpp"
#include "TransationTest.hpp"
#include "JoinTest.hpp"
#include "ColumnConstraintsTest.hpp"
#include "TableConstraintTest.hpp"
#include "TableOptionTest.hpp"
#include "BatchInsertTest.hpp"
#include "LimitOffsetTest.hpp"
#include "DistinctTest.hpp"
#include "AggregateFunctionsTest.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

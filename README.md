# SQLiteHelper

SQLiteHelper is a simple C++ SQLite database utility that supports table definition, data insertion, query, update, and deletion, with integrated GoogleTest unit testing.

## Features
- Type-safe Table/Column definitions
- Full CRUD operations (Insert/Select/Update/Delete)
- Condition queries (Equal, NotEqual, GreaterThan, LessThan, ...)
- Support for AND/OR condition combinations
- Transaction support with automatic commit/rollback
- Join operations (INNER, LEFT, RIGHT, FULL, CROSS)
- Unit testing (GoogleTest)
- Coverage report (lcov/gcov)

## Transaction Usage

SQLiteHelper provides exception-safe transaction support:

```cpp
// Automatic commit (if no exception occurs)
db.CreateTransaction([&db](auto &transaction) {
    db.GetTable<UserTable>().Insert(...);
    // Transaction commits automatically when lambda exits normally
});

// Automatic rollback (if exception occurs)
db.CreateTransaction([&db](auto &transaction) {
    db.GetTable<UserTable>().Insert(...);
    throw std::runtime_error("error"); // Transaction rolls back automatically
});

// Explicit commit
db.CreateTransaction([&db](auto &transaction) {
    db.GetTable<UserTable>().Insert(...);
    transaction.Commit(); // Explicitly commit
});

// Explicit rollback
db.CreateTransaction([&db](auto &transaction) {
    db.GetTable<UserTable>().Insert(...);
    transaction.Rollback(); // Explicitly rollback
});
```

**Note:** 
- If you need to ensure commit succeeds, call `transaction.Commit()` explicitly.
- The destructor will not throw exceptions to maintain exception safety.
- If an exception occurs during the transaction, automatic rollback is performed.

## Build Instructions
```bash
# Enter project directory
cd SQLiteHelper

# Create build directory
mkdir -p cmake-build-debug
cd cmake-build-debug

# Configure CMake and build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j 10
```

## Generate Coverage Report
```bash
# Enable coverage and build
cmake .. -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j 10
make coverage
# Open coverage_report/index.html in your browser
```

## License
This project is licensed under the MIT License. See the LICENSE file for details.

# SQLiteHelper

SQLiteHelper is a simple C++ SQLite database utility that supports table definition, data insertion, query, update, and deletion, with integrated GoogleTest unit testing.

## Features
- Type-safe Table/Column definitions
- Full CRUD operations (Insert/Select/Update/Delete)
- Condition queries (Equal, NotEqual, GreaterThan, LessThan, ...)
- Support for AND/OR condition combinations
- Unit testing (GoogleTest)
- Coverage report (lcov/gcov)

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

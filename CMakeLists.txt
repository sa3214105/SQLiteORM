cmake_minimum_required(VERSION 3.22)
project(SQLiteHelper)

set(CMAKE_CXX_STANDARD 20)

add_library(SQLiteHelper INTERFACE
        tests/test.cpp)
target_sources(SQLiteHelper INTERFACE SQLiteHelper.hpp)
target_include_directories(SQLiteHelper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

option(ENABLE_TESTS "Enable building tests" ON)
if(ENABLE_TESTS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_package(SQLite3 REQUIRED)
    add_subdirectory(external/googletest)
    file(GLOB_RECURSE TEST_FILES tests/*.cpp)
    list(FILTER SRC_FILES EXCLUDE REGEX "(^|[/\\\\])main\\.cpp$")
    add_executable(SQLiteHelper_Test ${TEST_FILES} ${SRC_FILES})
    target_include_directories(SQLiteHelper_Test PRIVATE external/gltFX-DataTransferStruct)
    target_include_directories(SQLiteHelper_Test PRIVATE external/EnvHelper)
    target_link_libraries(SQLiteHelper_Test PRIVATE SQLiteHelper gtest gtest_main SQLite::SQLite3)

    enable_testing()
    add_test(NAME SQLiteHelper_Test COMMAND SQLiteHelper_Test)

    # for CTest
    include(GoogleTest)
    gtest_discover_tests(SQLiteHelper_Test)
endif()
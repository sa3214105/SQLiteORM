cmake_minimum_required(VERSION 3.22)
project(SQLiteHelper)

set(CMAKE_CXX_STANDARD 20)

# Coverage option and flags
option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Code coverage enabled")
        set(COVERAGE_COMPILE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILE_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_COMPILE_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    else()
        message(WARNING "Code coverage is only supported with GCC or Clang")
    endif()
endif()

add_library(SQLiteHelper INTERFACE)
target_sources(SQLiteHelper INTERFACE SQLiteHelper.hpp)
target_include_directories(SQLiteHelper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

option(ENABLE_TESTS "Enable building tests" ON)
if(ENABLE_TESTS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_package(SQLite3 REQUIRED)
    add_subdirectory(external/googletest)
    file(GLOB_RECURSE TEST_FILES tests/*.cpp)
    list(FILTER SRC_FILES EXCLUDE REGEX "(^|[/\\])main\.cpp$")
    add_executable(SQLiteHelper_Test ${TEST_FILES} ${SRC_FILES})
    target_include_directories(SQLiteHelper_Test PRIVATE external/gltFX-DataTransferStruct)
    target_include_directories(SQLiteHelper_Test PRIVATE external/EnvHelper)
    target_link_libraries(SQLiteHelper_Test PRIVATE SQLiteHelper gtest gtest_main SQLite::SQLite3)

    # Apply coverage flags to test target
    if(ENABLE_COVERAGE)
        target_compile_options(SQLiteHelper_Test PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(SQLiteHelper_Test PRIVATE --coverage)
    endif()

    enable_testing()
    add_test(NAME SQLiteHelper_Test COMMAND SQLiteHelper_Test)

    # for CTest
    include(GoogleTest)
    gtest_discover_tests(SQLiteHelper_Test)

    # Coverage report targets
    if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_custom_target(coverage
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND lcov --directory ${CMAKE_BINARY_DIR} --capture --output-file coverage.info
            COMMAND lcov --remove coverage.info '/usr/*' '*/external/*' '*/tests/*' --output-file coverage.info.cleaned
            COMMAND genhtml coverage.info.cleaned --output-directory coverage_report
            COMMAND echo "Coverage report generated in: ${CMAKE_BINARY_DIR}/coverage_report/index.html"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS SQLiteHelper_Test
        )
        add_custom_target(coverage_clean
            COMMAND find ${CMAKE_BINARY_DIR} -name '*.gcda' -delete
            COMMAND find ${CMAKE_BINARY_DIR} -name '*.gcno' -delete
            COMMAND rm -f coverage.info coverage.info.cleaned
            COMMAND rm -rf coverage_report
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()
endif()
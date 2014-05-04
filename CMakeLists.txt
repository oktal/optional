cmake_minimum_required(VERSION 2.6)
project(optional_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ADD_SUBDIRECTORY(gtest-1.7.0)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(run_optional_tests test_optional.cc)
target_link_libraries(run_optional_tests gtest gtest_main)

add_test(run_optional_tests run_optional_tests)

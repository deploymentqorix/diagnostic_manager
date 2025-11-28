cmake_minimum_required(VERSION 3.10)
project(TestApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Conan setup
if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../inc")

add_library(app STATIC ../src/app.cpp)
add_executable(run_tests ../src/test.cpp)
target_link_libraries(run_tests PRIVATE app GTest::gtest_main)

target_include_directories(app PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../inc")
target_include_directories(run_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../inc")

enable_testing()
add_test(NAME AppTests COMMAND run_tests)
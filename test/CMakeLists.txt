cmake_minimum_required(VERSION 3.5)

project(unit_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -fPIC")

enable_testing()
add_subdirectory(../external/googletest build)

include_directories("../inc")

file(GLOB SOURCES "../src/**/*.cpp")

add_executable(unit_tests ${SOURCES})

target_link_libraries(unit_tests gtest gmock gtest_main)
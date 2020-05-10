cmake_minimum_required(VERSION 3.10.0)
project(MY_PROJECT CXX)

add_executable(proj_test test.cpp lib.cpp)
add_executable(proj main.cpp lib.cpp)
target_link_libraries(proj_test PUBLIC breep::breep default Catch2::Catch2)
target_link_libraries(proj PUBLIC breep::breep default)
catch_discover_tests(proj_test)

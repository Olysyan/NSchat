cmake_minimum_required(VERSION 3.10.0)
project(MY_PROJECT CXX)


add_executable(proj main.cpp lib.hpp)
add_executable(proj_test test.cpp lib.hpp)


target_link_libraries(proj PUBLIC breep::breep default)
target_link_libraries(proj PUBLIC nana::nana default)
target_link_libraries(proj_test PUBLIC breep::breep default)
target_link_libraries(proj_test PUBLIC nana::nana default)
target_link_libraries(proj_test PUBLIC Catch2::Catch2 default)
catch_discover_tests(proj_test)
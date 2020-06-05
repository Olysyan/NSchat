cmake_minimum_required(VERSION 3.10.0)
project(MY_PROJECT CXX)


add_executable(proj main.cpp lib.cpp)

target_link_libraries(proj PUBLIC breep::breep default)
target_link_libraries(proj PUBLIC nana::nana default)


cmake_minimum_required(VERSION 3.5)
project(Breep LANGUAGES CXX)
find_package( Boost 1.55 REQUIRED system )
find_package( Threads )

add_library(breep INTERFACE)
target_include_directories(breep SYSTEM
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>
)
target_link_libraries(breep INTERFACE Boost::system Threads::Threads)

add_library(breep::breep ALIAS breep)

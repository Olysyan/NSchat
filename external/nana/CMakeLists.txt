cmake_minimum_required(VERSION 3.13)
project(seminar_gui CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckIncludeFileCXX)
if(MSVC)
 add_compile_options(/permissive-)
 set(MSVC_USE_STATIC_RUNTIME OFF CACHE BOOL "")
endif(MSVC)

include(FetchContent)
FetchContent_Declare(
 nana
 GIT_REPOSITORY
 https://github.com/cnjinhao/nana.git
 GIT_TAG v1.7.4)

FetchContent_GetProperties(nana)
if(NOT nana_POPULATED)
 FetchContent_Populate(nana)
 add_subdirectory(${nana_SOURCE_DIR} ${nana_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


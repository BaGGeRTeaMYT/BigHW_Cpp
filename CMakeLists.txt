cmake_minimum_required(VERSION 3.20)
project(not_sql)

set(CMAKE_CXX_STANDARD 20)

file (GLOB_RECURSE SOURCES "src/*.cpp")
add_library(not_sql SHARED ${SOURCES})

target_include_directories(not_sql PUBLIC include)

add_executable(Main main.cpp)
target_link_libraries(Main PRIVATE not_sql)
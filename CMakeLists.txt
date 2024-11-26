cmake_minimum_required(VERSION 3.20)
project(not_sql)

set(CMAKE_CXX_STANDARD 20)

file (GLOB_RECURSE SOURCES "src/*.cpp")
add_library(not_sql SHARED ${SOURCES})

include_directories(include/ include/cells)

target_include_directories(not_sql PUBLIC include)

add_executable(Main
            main.cpp
            src/Query.cpp
            src/cells/Cell.cpp
            src/cells/IntCell.cpp
            src/cells/BoolCell.cpp
            src/cells/BytesCell.cpp
            src/cells/StringCell.cpp
            src/Column.cpp
            src/Table.cpp
            src/Database.cpp)
# target_link_libraries(Main PRIVATE not_sql)
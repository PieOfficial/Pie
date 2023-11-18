cmake_minimum_required(VERSION 3.20)

project(pie VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -g")

set(SOURCES
    src/main.cpp
)

set(INCLUDE_DIRS
    "include"
)

set(LIBRARY_DIRS
    lib
)

add_executable(pie ${SOURCES})
target_link_libraries(pie libcurl)

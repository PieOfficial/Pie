cmake_minimum_required(VERSION 3.20)

project(pie VERSION 1.0.0)

set(CMAKE_GENERATOR "MSYS Makefiles" CACHE INTERNAL "" FORCE)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -g")


set(SOURCE
    src/main.cpp
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/liblua54.a DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lua54.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add the include directory for the Lua header files
include_directories(include)

add_executable(pie ${SOURCE})

# Link against the Lua library
target_link_libraries(pie "${CMAKE_CURRENT_SOURCE_DIR}/liblua54.a" "${CMAKE_CURRENT_SOURCE_DIR}/lua54.dll")
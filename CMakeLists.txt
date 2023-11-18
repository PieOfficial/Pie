cmake_minimum_required(VERSION 3.20)

project(pie VERSION 1.0.0)

set(CMAKE_GENERATOR "MSYS Makefiles" CACHE INTERNAL "" FORCE)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -g")

set(SOURCE
    src/main.cpp
)

# set(INCLUDE_DIRS
#     include
# )
# set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)
# set(LIBRARY_DIRS
#     lib
# )

add_executable(pie ${SOURCE})
#target_link_libraries(pie libcurl)

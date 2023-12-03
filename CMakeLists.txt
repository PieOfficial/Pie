cmake_minimum_required(VERSION 3.20)

project(pie VERSION 1.0.0)

set(CMAKE_GENERATOR "MSYS Makefiles" CACHE INTERNAL "" FORCE)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -g")


set(SOURCE
    src/main.cpp
    src/core/gui/gui.cpp
)

# set(INCLUDE_DIRS
#     include
# )
# set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)
# set(LIBRARY_DIRS
#     lib
# )
#set(CURL_LIBRARY_DIRS "C:/Users/Dragon boy/Desktop/UnSorted/PieNew/curl/lib")
#set(CURL_INCLUDE_DIRS "C:/Users/Dragon boy/Desktop/UnSorted/PieNew/curl/include")



# find_package(CURL QUIET)
# if(CURL_FOUND)
#     set(INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
#     target_link_libraries(pie ${CURL_LIBRARIES})
# else()
#     message(FATAL_ERROR "Failed to find libcurl")
# endif()

add_executable(pie ${SOURCE})


# find_package (CURL)
# if (CURL_FOUND)
# 	include_directories(${CURL_INCLUDE_DIRS})
# 	target_link_libraries(pie ${CURL_LIBRARIES})
# else()
# 	MESSAGE(FATAL_ERROR "LibCurl is not found in your system.")
# endif (CURL_FOUND)
#target_link_libraries(pie libcurl)
# Add the curl library to the target's link libraries
#target_link_libraries(pie lib/libcurl.a)

cmake_minimum_required(VERSION 3.20)

project(pie VERSION 1.0.0)

#set(CMAKE_GENERATOR "MSYS Makefiles" CACHE INTERNAL "" FORCE)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -g")

# Set Lua include and library paths
IF (WIN32)
  set(LUA_INCLUDE_DIR "C:/msys64/mingw64/include/lua5.3")
  set(LUA_LIBRARIES "C:/msys64/mingw64/lib/liblua5.3.a")
  set(LUA_LIBRARIESDLL "C:/msys64/mingw64/bin/lua53.dll")
  
ELSE()
  # set stuff for other systems
ENDIF()

set(SOURCE
    src/main.cpp
    src/script/LuaManager.cpp
    src/core/Network/network.cpp
)

# Links CURL; "l" for "library", then follows the library name.
set(CURL_LIBRARY "-lcurl")
# Find CURL
find_package(CURL REQUIRED)
# Find Lua libraries
find_package(Lua REQUIRED)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/liblua54.a DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lua54.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/curl.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add the include directory for the Lua header files
include_directories(include)

add_executable(pie ${SOURCE})

# Link the CURL "include" directory
include_directories(${CURL_INCLUDE_DIR})
# Link the CURL libraries and Lua libraries
target_link_libraries(pie ${CURL_LIBRARIES} ${LUA_LIBRARIES} ${LUA_LIBRARIESDLL} -lmingw32)

include_directories(${LUA_INCLUDE_DIR})


#target_link_directories(pie PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Link against the curl library
#target_link_libraries(pie "${CMAKE_CURRENT_SOURCE_DIR}/curl.dll")

# Link against the Lua library
#target_link_libraries(pie "${CMAKE_CURRENT_SOURCE_DIR}/liblua54.a" "${CMAKE_CURRENT_SOURCE_DIR}/lua54.dll")
cmake_minimum_required(VERSION 2.6)
project(wadmake)

add_subdirectory("lib")

include_directories("lib/bstring-1.0.0/bstring")
include_directories("lib/lua-5.3.0/src")

set(WADMAKE_SOURCES src/wadmake.c)
set(WADMAKE_HEADERS "")

add_executable(wadmake ${WADMAKE_SOURCES} ${WADMAKE_HEADERS})
target_link_libraries(wadmake bstring lua53)

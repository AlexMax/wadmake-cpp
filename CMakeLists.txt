cmake_minimum_required(VERSION 2.6)
project(wadmake)

add_subdirectory("lib")
add_subdirectory("tool")

include_directories("lib/lua-5.3.0/src")

# Sources
set(WADMAKE_SOURCES src/lwad.cc src/wad.cc)
set(WADMAKE_HEADERS src/lua.hh src/lwad.hh src/wad.hh)
set(WADMAKE_LUA_SOURCES src/lua/init.lua)

# Compile Lua sources into the binary
set(LUA_HEADER_DIR "${CMAKE_BINARY_DIR}/src/lua")
file(MAKE_DIRECTORY ${LUA_HEADER_DIR})
include_directories(${LUA_HEADER_DIR})
foreach(LUA_SOURCE ${WADMAKE_LUA_SOURCES})
	set(LUA_SOURCE_OUTFILE "${LUA_SOURCE}.hh")
	set(LUA_SOURCE_FULL_OUTFILE "${CMAKE_BINARY_DIR}/${LUA_SOURCE_OUTFILE}")
	add_custom_command(
		OUTPUT ${LUA_SOURCE_FULL_OUTFILE}
		COMMAND xxd -i ${LUA_SOURCE} ${LUA_SOURCE_FULL_OUTFILE}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Dumping to CXX header ${LUA_SOURCE_OUTFILE}")
	list(APPEND WADMAKE_HEADERS ${LUA_SOURCE_FULL_OUTFILE})
endforeach()

# Set compiler-specific flags
if(MSVC)
	set(CXXFLAGS "/W4")
else()
	set(CXXFLAGS "-Wall -Wextra -std=c++11")
endif()

# Binary
add_executable(wadmake src/wadmake.cc
	${WADMAKE_SOURCES} ${WADMAKE_HEADERS})
target_link_libraries(wadmake lua53)
set_target_properties(wadmake PROPERTIES COMPILE_FLAGS ${CXXFLAGS})

# Unit testing
enable_testing()

# Files needed for unit testing
file(DOWNLOAD "http://static.best-ever.org/wads/moo2d.wad" "${CMAKE_BINARY_DIR}/moo2d.wad"
	EXPECTED_MD5 "2e4635df68da25f78fde58ab179b8c2c")

include_directories(src)
add_executable(testwadmake test/testwadmake.cc
	${WADMAKE_SOURCES} ${WADMAKE_HEADERS})
target_link_libraries(testwadmake lua53)
add_test(NAME wadmake COMMAND testwadmake)
set_target_properties(testwadmake PROPERTIES COMPILE_FLAGS ${CXXFLAGS})

# Target for our .lua files so they'll show up in an IDE
add_custom_target(wadmake_lua SOURCES ${WADMAKE_LUA_SOURCES})

cmake_minimum_required(VERSION 2.6)
project(wadmake)

add_subdirectory("lib")
add_subdirectory("tool")

# Remove unneeded targets from default build
foreach(TAR example example64 minigzip minigzip64 zlib)
	if(TARGET ${TAR})
		set_target_properties(${TAR} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
	endif()
endforeach()

# Include library directories
include_directories("lib/lua-5.3.0/src")
include_directories("lib/zlib-1.2.8")
include_directories("${CMAKE_BINARY_DIR}/lib/zlib-1.2.8") # zconf.h

# Sources
set(WADMAKE_SOURCES src/lua.cc src/lwad.cc src/wad.cc src/zip.cc)
set(WADMAKE_HEADERS src/lua.hh src/lwad.hh src/wad.hh src/zip.hh)
set(WADMAKE_LUA_SOURCES src/lua/init.lua src/lua/lwad.lua)

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
		DEPENDS ${LUA_SOURCE}
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
set(WADMAKE_LIBRARIES lua53 zlibstatic)

# Binary
add_executable(wadmake src/wadmake.cc
	${WADMAKE_SOURCES} ${WADMAKE_HEADERS})
target_link_libraries(wadmake ${WADMAKE_LIBRARIES})
set_target_properties(wadmake PROPERTIES COMPILE_FLAGS ${CXXFLAGS})

# Unit testing
enable_testing()

# Files needed for unit testing
file(DOWNLOAD "http://static.best-ever.org/wads/moo2d.wad" "${CMAKE_BINARY_DIR}/moo2d.wad"
	EXPECTED_MD5 "2e4635df68da25f78fde58ab179b8c2c")
file(DOWNLOAD "http://static.best-ever.org/wads/duel32f.pk3" "${CMAKE_BINARY_DIR}/duel32f.pk3"
	EXPECTED_MD5 "15dadb7e1976d2c7825e3221c4b649eb")

# Unit testing executable
include_directories(src)
add_executable(testwadmake test/testwadmake.cc
	${WADMAKE_SOURCES} ${WADMAKE_HEADERS})
target_link_libraries(testwadmake ${WADMAKE_LIBRARIES})

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wno-unknown-pragmas" NO_WARN_UNKNOWN_PRAGMAS)

if(NO_WARN_UNKNOWN_PRAGMAS)
	set_target_properties(testwadmake PROPERTIES COMPILE_FLAGS "${CXXFLAGS} -Wno-unknown-pragmas")
else()
	set_target_properties(testwadmake PROPERTIES COMPILE_FLAGS "${CXXFLAGS}")
endif()
	
# Running the unit test
add_test(NAME Test COMMAND testwadmake WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Running the unit test under valgrind, if available
find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
	add_test(NAME Valgrind
		COMMAND ${VALGRIND_EXECUTABLE} "--error-exitcode=1" "$<TARGET_FILE:testwadmake>")
endif()

# Target for our .lua files so they'll show up in an IDE
add_custom_target(wadmake_lua SOURCES ${WADMAKE_LUA_SOURCES})

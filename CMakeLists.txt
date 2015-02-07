cmake_minimum_required(VERSION 2.6)
project(wadmake)

# CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Binary files we compile
add_subdirectory("bin")

# Documentation
add_subdirectory("doc")

# Libraries we link against
add_subdirectory("lib")

# Wadmake source files
add_subdirectory("src")

# Unit testing
add_subdirectory("test")

# Tools needed to build
add_subdirectory("tool")

# Remove unneeded targets from default build
foreach(TAR example example64 minigzip minigzip64 zlib)
	if(TARGET ${TAR})
		set_target_properties(${TAR} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
	endif()
endforeach()

# Unit testing
enable_testing()

# Run unit tests
add_test(NAME TestWadmake COMMAND testwadmake WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test")

# Run unit tests under valgrind, if available
find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
	add_test(NAME Valgrind
	         COMMAND ${VALGRIND_EXECUTABLE} "--error-exitcode=1" "$<TARGET_FILE:testwadmake_exe>"
	         WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test")
endif()

if(MSVC)
	set(WADMAKE_EXE_CXXFLAGS "/W4")
	set(WADSH_EXE_CXXFLAGS "/W4")
else()
	set(WADMAKE_EXE_CXXFLAGS "-Wall -Wextra -std=c++11")
	set(WADSH_EXE_CXXFLAGS "-Wall -Wextra -std=c++11")
endif()

include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/lib/lua-5.3.0/src"
	"${CMAKE_CURRENT_BINARY_DIR}"
)

add_executable(wadmake_exe wadmake.cc)
set_target_properties(wadmake_exe PROPERTIES COMPILE_FLAGS ${WADMAKE_EXE_CXXFLAGS})
set_target_properties(wadmake_exe PROPERTIES OUTPUT_NAME wadmake)
target_link_libraries(wadmake_exe wadmake)

# wadsh uses Readline if available
find_package(Readline)

configure_file(config_wadsh.h.in config_wadsh.h)

add_executable(wadsh_exe wadsh.cc)
set_target_properties(wadsh_exe PROPERTIES COMPILE_FLAGS ${WADSH_EXE_CXXFLAGS})
set_target_properties(wadsh_exe PROPERTIES OUTPUT_NAME wadsh)
target_link_libraries(wadsh_exe wadmake)

if(READLINE_FOUND)
	set_target_properties(wadsh_exe PROPERTIES COMPILE_DEFINITIONS "HAS_READLINE")
	target_include_directories(wadsh_exe PRIVATE "${READLINE_INCLUDE_DIR}")
	target_link_libraries(wadsh_exe "${READLINE_LIBRARY}")
endif()

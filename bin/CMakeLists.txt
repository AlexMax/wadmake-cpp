if(MSVC)
	set(WADMAKE_EXE_CXXFLAGS "/W4")
else()
	set(WADMAKE_EXE_CXXFLAGS "-Wall -Wextra -std=c++11")
endif()

include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/lib/lua-5.3.0/src"
)

add_executable(wadmake_exe wadmake.cc)
set_target_properties(wadmake_exe PROPERTIES COMPILE_FLAGS ${WADMAKE_EXE_CXXFLAGS})
set_target_properties(wadmake_exe PROPERTIES OUTPUT_NAME wadmake)
target_link_libraries(wadmake_exe wadmake)

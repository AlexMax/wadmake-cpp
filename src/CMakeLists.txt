function(dump_lua LUA_SOURCES LUA_HEADER_EXTENSION LUA_HEADERS_VAR)
	foreach(LUA_SOURCE ${LUA_SOURCES})
		set(LUA_SOURCE_OUTFILE "${LUA_SOURCE}${LUA_HEADER_EXTENSION}")
		set(LUA_SOURCE_FULL_OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/${LUA_SOURCE_OUTFILE}")
		add_custom_command(
			OUTPUT ${LUA_SOURCE_FULL_OUTFILE}
			COMMAND xxd -i ${LUA_SOURCE} ${LUA_SOURCE_FULL_OUTFILE}
			DEPENDS ${LUA_SOURCE}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMENT "Dumping to CXX header ${LUA_SOURCE_OUTFILE}")
		list(APPEND LUA_HEADERS ${LUA_SOURCE_FULL_OUTFILE})
	endforeach()
	set(${LUA_HEADERS_VAR} ${LUA_HEADERS} PARENT_SCOPE)
endfunction()

# Compiler flags
if(MSVC)
	set(WADMAKE_CXXFLAGS "/W4")
else()
	set(WADMAKE_CXXFLAGS "-Wall -Wextra -std=c++11")
endif()

# Sources
set(WADMAKE_SOURCES buffer.cc lua.cc luawad.cc wad.cc zip.cc)
set(WADMAKE_HEADERS buffer.hh lua.hh luawad.hh wad.hh zip.hh)
set(WADMAKE_LUA_SOURCES init.lua luawad.lua)

dump_lua("${WADMAKE_LUA_SOURCES}" ".hh" WADMAKE_LUA_HEADERS)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	"${CMAKE_SOURCE_DIR}/lib/lua-5.3.0/src"
	"${CMAKE_SOURCE_DIR}/lib/zlib-1.2.8"
	"${CMAKE_BINARY_DIR}/lib/zlib-1.2.8" # zconf.h
)

add_library(wadmake STATIC ${WADMAKE_SOURCES} ${WADMAKE_HEADERS} ${WADMAKE_LUA_SOURCES} ${WADMAKE_LUA_HEADERS})
set_target_properties(wadmake PROPERTIES COMPILE_FLAGS ${WADMAKE_CXXFLAGS})
target_link_libraries(wadmake lua53 zlibstatic)

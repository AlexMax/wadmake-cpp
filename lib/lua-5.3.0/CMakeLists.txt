set(LUA_CORE src/lapi.c src/lcode.c src/lctype.c src/ldebug.c src/ldo.c
             src/ldump.c src/lfunc.c src/lgc.c src/llex.c src/lmem.c
             src/lobject.c src/lopcodes.c src/lparser.c src/lstate.c
             src/lstring.c src/ltable.c src/ltm.c src/lundump.c src/lvm.c
             src/lzio.c)
set(LUA_LIB src/lauxlib.c src/lbaselib.c src/lbitlib.c src/lcorolib.c
            src/ldblib.c src/liolib.c src/lmathlib.c src/loslib.c
            src/lstrlib.c src/ltablib.c src/lutf8lib.c src/loadlib.c
            src/linit.c)

set(LUA_SOURCES ${LUA_CORE} ${LUA_LIB})
set_source_files_properties(${LUA_SOURCES} PROPERTIES LANGUAGE CXX)

add_library(lua53 STATIC ${LUA_SOURCES})
if(NOT MSVC)
	target_link_libraries(lua53 m)
	set_target_properties(lua53 PROPERTIES COMPILE_FLAGS "-xc++")
endif()

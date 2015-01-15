#include <stdio.h>
#include <stdlib.h>

#include <bstrlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "error.h"
#include "wad.h"
#include "init.lua.h"

lua_State* Lua;

static void Cleanup() {
	lua_close(Lua);
}

void FatalError(const bstring error) {
	fprintf(stderr, "wadmake: %s\n", error->data);
	Cleanup();
	exit(EXIT_FAILURE);
}

void CFatalError(const char* error) {
	fprintf(stderr, "wadmake: %s\n", error);
	Cleanup();
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	const char* filename = "wadmakefile";

	Lua = luaL_newstate();
	luaL_requiref(Lua, "_G", luaopen_base, 1);
	luaL_requiref(Lua, LUA_LOADLIBNAME, luaopen_package, 1);
	lua_pop(Lua, 1);

	if (luaL_loadbuffer(Lua, (char*)src_lua_init_lua, src_lua_init_lua_len, "init") != LUA_OK) {
		FatalError(bformat("internal lua error\n%s", lua_tostring(Lua, -1)));
	}

	if (lua_pcall(Lua, 0, LUA_MULTRET, 0) != LUA_OK) {
		FatalError(bformat("internal lua runtime error\n%s", lua_tostring(Lua, -1)));
	}

	if (luaL_dofile(Lua, filename) == 1) {
		FatalError(bformat("error while parsing %s", filename));
	}

	return EXIT_SUCCESS;
}

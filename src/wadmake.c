/*
 *  wadmake: a WAD manipulation utility.
 *  Copyright (C) 2015  Alex Mayfield
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

int main() {
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

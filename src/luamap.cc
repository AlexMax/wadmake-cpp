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

#include <lua.h>
#include <lauxlib.h>

#include "map.hh"

namespace WADmake {

static const char META_DOOMMAP[] = "DoomMap";

static int wad_createDoomMap(lua_State* L) {
	auto ptr = static_cast<std::shared_ptr<DoomMap>*>(lua_newuserdata(L, sizeof(std::shared_ptr<DoomMap>)));
	new(ptr) std::shared_ptr<DoomMap>(new DoomMap());
	luaL_setmetatable(L, WADmake::META_DOOMMAP);
	return 1;
}

static int udoommap_getthing(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	DoomThing thing;
	try {
		thing = ptr->getThings().at(index - 1);
	} catch (const std::out_of_range& e) {
		lua_pushnil(L);
		return 1;
	}

	lua_newtable(L);
	lua_pushinteger(L, thing.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, thing.y);
	lua_setfield(L, -2, "y");
	lua_pushinteger(L, thing.angle);
	lua_setfield(L, -2, "angle");
	lua_pushinteger(L, thing.type);
	lua_setfield(L, -2, "type");
	lua_pushinteger(L, thing.flags.to_ulong());
	lua_setfield(L, -2, "flags");

	return 1;
}
static int udoommap_setthing(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	DoomThing& thing = ptr->getThings()[index - 1];

	lua_getfield(L, 3, "x");
	if (!lua_isnil(L, -1)) {
		thing.x = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "y");
	if (!lua_isnil(L, -1)) {
		thing.y = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "angle");
	if (!lua_isnil(L, -1)) {
		thing.angle = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "type");
	if (!lua_isnil(L, -1)) {
		thing.angle = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	return 0;
}

// Garbage-collect Lumps
static int udoommap_gc(lua_State* L) {
	auto ptr = static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));
	ptr->~shared_ptr();
	return 0;
}

// Functions attached to DoomMap userdata
static const luaL_Reg udoommap_functions[] = {
	{"getthing", udoommap_getthing},
	{"setthing", udoommap_setthing},
	{"__gc", udoommap_gc},
	{NULL, NULL}
};

// Functions that go in the top-level wad package
static const luaL_Reg wad_functions[] = {
	{"createDoomMap", wad_createDoomMap},
	{NULL, NULL}
};

// Initialize the part of the wad package that deals with maps.  Assumes that
// the 'wad' library table is at the top of the stack.
void luaopen_map(lua_State* L) {
	// Create "DoomMap" userdata
	luaL_newmetatable(L, WADmake::META_DOOMMAP);
	// [wadlib][DoomMapmeta]
	lua_pushvalue(L, -1);
	// [wadlib][DoomMapmeta][DoomMapmeta]
	lua_setfield(L, -2, "__index");
	// [wadlib][DoomMapmeta]
	luaL_setfuncs(L, udoommap_functions, 0);
	lua_pop(L, 1);
	// [wadlib]
	luaL_setfuncs(L, wad_functions, 0);
}

}

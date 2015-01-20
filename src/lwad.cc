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

#include <cstring>
#include <exception>

#include <lua.h>
#include <lauxlib.h>

#include "wad.hh"

#define META_LUMPS "Lumps"

// Create an empty Lumps userdata
static int wad_createLumps(lua_State* L) {
	Directory** ptr = (Directory**)lua_newuserdata(L, sizeof(Directory*));
	*ptr = new Directory();
	luaL_setmetatable(L, META_LUMPS);
	return 1;
}

// Read WAD file data and return the WAD type and lumps
static int wad_readwad(lua_State* L) {
	// Read WAD file data into stringstream.
	size_t len;
	const char* buffer = luaL_checklstring(L, 1, &len);
	std::string buffer_string(buffer, len);
	std::stringstream buffer_stream;
	buffer_stream << buffer_string;

	// Stream the data into Wad class to get our WAD type and lumps.
	Wad wad = Wad(buffer_stream);

	// Create a table
	lua_createtable(L, 0, 2); /* [table] */

	// Store 'type' in table
	lua_pushstring(L, "type");
	Wad::Type wad_type = wad.getType();
	if (wad_type == Wad::Type::IWAD) {
		lua_pushstring(L, "iwad");
	} else if (wad_type == Wad::Type::PWAD) {
		lua_pushstring(L, "pwad");
	}
	lua_settable(L, -3);

	// store 'lumps' in table
	lua_pushstring(L, "lumps");
	Directory** ptr = (Directory**)lua_newuserdata(L, sizeof(Directory*));
	*ptr = new Directory(wad.getLumps());
	luaL_setmetatable(L, META_LUMPS);
	lua_settable(L, -3);

	// return our table
	return 1;
}

// Garbage-collect Lumps
static int ulumps_gc(lua_State* L) {
	Directory* ptr = *(Directory**)luaL_checkudata(L, 1, META_LUMPS);
	if (ptr) {
		delete ptr;
	}
	return 0;
}

// Return a specific index out of Lumps (1-indexed)
static int ulumps_index(lua_State* L) {
	Directory* ptr = *(Directory**)luaL_checkudata(L, 1, META_LUMPS);
	Lump lump = ptr->at(luaL_checkinteger(L, 2) - 1);

	// Set name (always null-terminated)
	lua_createtable(L, 0, 2);
	lua_pushstring(L, "name");
	lua_pushstring(L, lump.getName().c_str());
	lua_settable(L, -3);

	// Set data
	const std::string data = lump.getData();
	lua_pushstring(L, "data");
	lua_pushlstring(L, data.data(), data.size());
	lua_settable(L, -3);
	return 1;
}

// Return the length of the Lumps
static int ulumps_len(lua_State* L) {
	Directory* ptr = *(Directory**)luaL_checkudata(L, 1, META_LUMPS);
	lua_pushinteger(L, ptr->size());
	return 1;
}

// Print Lumps as a string
static int ulumps_tostring(lua_State* L) {
	Directory* ptr = *(Directory**)luaL_checkudata(L, 1, META_LUMPS);
	size_t size = ptr->size();
	if (size == 1) {
		lua_pushfstring(L, META_LUMPS ": %p, %d lump", ptr, ptr->size());
	} else {
		lua_pushfstring(L, META_LUMPS ": %p, %d lumps", ptr, ptr->size());
	}
	return 1;
}

// Functions attached to Lumps userdata
static const luaL_Reg ulumps_functions[] = {
	{"__gc", ulumps_gc},
	{"__index", ulumps_index},
	{"__len", ulumps_len},
	{"__tostring", ulumps_tostring},
	{NULL, NULL}
};

// Functions in the wad package
static const luaL_Reg wad_functions[] = {
	{ "createLumps", wad_createLumps },
	{ "readwad", wad_readwad },
	{ NULL, NULL }
};

// Initialize the wad package
int luaopen_wad(lua_State* L) {
	luaL_newlib(L, wad_functions);

	// Create "Lumps" userdata
	luaL_newmetatable(L, META_LUMPS);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, ulumps_functions, 0);
	lua_pop(L, 1);

	return 1;
}

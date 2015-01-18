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

#include <exception>

#include <iostream>

#include <lua.h>
#include <lauxlib.h>

#include "wad.hh"

static int wad_create(lua_State* L) {
	Wad** ptr = (Wad**)lua_newuserdata(L, sizeof(Wad*));

	if (lua_isstring(L, 1)) {
		// Passing in a string creates the wad object from a bytestring
		size_t len = 0;
		const char* buf = lua_tolstring(L, 1, &len);
		try {
			std::stringstream buffer;
			buffer << std::string(buf, len);
			*ptr = new Wad(buffer);
		} catch (std::exception& e) {
			luaL_error(L, e.what());
			return 0;
		}
	} else if (lua_istable(L, 1)) {
		// Passing in a table of options creates a fresh wad object
		*ptr = new Wad(Wad::Type::IWAD);
	} else {
		luaL_error(L, "missing parameter");
		return 0;
	}

	luaL_setmetatable(L, "wad");
	return 1;
}

static int uwad_gc(lua_State* L) {
	Wad* ptr = *(Wad**)luaL_checkudata(L, 1, "wad");
	if (ptr) {
		delete ptr;
	}
	return 0;
}

static int uwad_tostring(lua_State* L) {
	Wad* ptr = *(Wad**)luaL_checkudata(L, 1, "wad");
	if (ptr->getType() == Wad::Type::PWAD) {
		lua_pushfstring(L, "PWAD %p", ptr);
	} else {
		lua_pushfstring(L, "IWAD %p", ptr);
	}
	return 1;
}

static const luaL_Reg uwad_functions[] = {
	{"__gc", uwad_gc},
	{"__tostring", uwad_tostring},
	{NULL, NULL}
};

static const luaL_Reg wad_functions[] = {
	{ "createwad", wad_create },
	{ NULL, NULL }
};

int luaopen_wad(lua_State* L) {
	luaL_newlib(L, wad_functions);

	// Create "wad" userdata
	luaL_newmetatable(L, "wad");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, uwad_functions, 0);
	lua_pop(L, 1);

	return 1;
}

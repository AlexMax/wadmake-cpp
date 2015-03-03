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

#include <sstream>

#include <lua.h>
#include <lauxlib.h>

#include "lua.hh"
#include "wad.hh"
#include "zip.hh"

namespace WADmake {

#include "luawad.lua.hh"

static const char META_LUMPS[] = "Lumps";

// Create an empty Lumps userdata
static int wad_createLumps(lua_State* L) {
	auto ptr = static_cast<std::shared_ptr<Directory>*>(lua_newuserdata(L, sizeof(std::shared_ptr<Directory>)));
	new(ptr) std::shared_ptr<Directory>(new Directory());
	luaL_setmetatable(L, WADmake::META_LUMPS);
	return 1;
}

// Read WAD file data and return the WAD type and lumps
static int wad_unpackwad(lua_State* L) {
	// Read WAD file data into stringstream.
	size_t len;
	const char* buffer = luaL_checklstring(L, 1, &len);
	std::string buffer_string(buffer, len);
	std::stringstream buffer_stream;
	buffer_stream << buffer_string;

	// Stream the data into Wad class to get our WAD type and lumps.
	Wad wad;
	try {
		buffer_stream >> wad;
	} catch (const std::runtime_error& e) {
		return luaL_error(L, e.what());
	}

	// Lump data
	auto ptr = static_cast<std::shared_ptr<Directory>*>(lua_newuserdata(L, sizeof(std::shared_ptr<Directory>)));
	new(ptr) std::shared_ptr<Directory>(wad.getLumps());
	luaL_setmetatable(L, WADmake::META_LUMPS);

	// WAD type
	Wad::Type wad_type = wad.getType();
	if (wad_type == Wad::Type::IWAD) {
		lua_pushstring(L, "iwad");
	}
	else if (wad_type == Wad::Type::PWAD) {
		lua_pushstring(L, "pwad");
	}

	return 2;
}

// Read Zip file data and return lumps contained therin
static int wad_unpackzip(lua_State* L) {
	// Read Zip file data into stringstream.
	size_t len;
	const char* buffer = luaL_checklstring(L, 1, &len);
	std::string buffer_string(buffer, len);
	std::stringstream buffer_stream;
	buffer_stream << buffer_string;

	// Stream the data into Zip class to get our lumps.
	Zip zip;
	try {
		buffer_stream >> zip;
	} catch (const std::runtime_error& e) {
		return luaL_error(L, e.what());
	}

	// Lump data
	auto ptr = static_cast<std::shared_ptr<Directory>*>(lua_newuserdata(L, sizeof(std::shared_ptr<Directory>)));
	new(ptr) std::shared_ptr<Directory>(zip.getLumps());
	luaL_setmetatable(L, WADmake::META_LUMPS);

	return 1;
}

// Find a lump with a given name
static int ulumps_find(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));

	std::string name = Lua::checkstring(L, 2);

	// If we passed a 'start' parameter, use it, otherwise the default
	// index to use is 1.
	lua_Integer start;
	if (lua_type(L, 3) != LUA_TNONE) {
		start = luaL_checkinteger(L, 3);
	} else {
		start = 1;
	}

	// Normalize start parameter.
	if (start == 0) {
		// Handle case where start is 0, pretend it's 1.
		start = 1;
	} else if (start < 0) {
		// Negative values are from the end
		start = ptr->size() + start + 1;
	}

	// If we go out-of-bounds, push a nil.
	if (static_cast<size_t>(start) > ptr->size()) {
		lua_pushnil(L);
		return 1;
	}

	bool success;
	size_t index;
	std::tie(success, index) = ptr->find_index(name, start - 1);

	if (success) {
		lua_pushinteger(L, index + 1);
	} else {
		lua_pushnil(L);
	}

	return 1;
}

// Get a lump at a particular position (1-indexed)
static int ulumps_get(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));

	size_t index = luaL_checkinteger(L, 2);
	if (index < 1 || index > ptr->size()) {
		luaL_argerror(L, 2, "out of range");
	}

	Lump lump = ptr->at(index - 1);
	const std::string name = lump.getName();
	const std::string data = lump.getData();

	lua_pushstring(L, name.c_str());
	lua_pushlstring(L, data.data(), data.size());

	return 2;
}

// Insert a new lump into a particular position (1-indexed)
static int ulumps_insert(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));

	if (lua_isinteger(L, 2)) {
		// Second parameter is index to push to
		size_t index = luaL_checkinteger(L, 2);
		if (index < 1 || index > ptr->size()) {
			luaL_argerror(L, 2, "index out of range");
		}

		Lump lump;
		lump.setName(Lua::checkstring(L, 3));
		lump.setData(Lua::checklstring(L, 4));

		ptr->insert_at(index - 1, std::move(lump));
	} else {
		// Append to end
		Lump lump;
		lump.setName(Lua::checkstring(L, 2));
		lump.setData(Lua::checklstring(L, 3));

		ptr->push_back(std::move(lump));
	}

	return 0;
}

// Remove a lump from a particular position
static int ulumps_remove(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));
	size_t index = luaL_checkinteger(L, 2);
	if (index < 1 || index > ptr->size()) {
		luaL_argerror(L, 2, "index out of range");
	}
	ptr->erase_at(index - 1);
	return 0;
}

// Set a lump at a particular position (1-indexed)
static int ulumps_set(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));

	// Second parameter is index to push into
	size_t index = luaL_checkinteger(L, 2);
	if (index < 1 || index > ptr->size()) {
		luaL_argerror(L, 2, "out of range");
	}

	int nametype = lua_type(L, 3);
	int datatype = lua_type(L, 4);

	// Name is string if present, nil if not
	if (nametype != LUA_TSTRING && nametype != LUA_TNIL) {
		luaL_argerror(L, 3, "must be string or nil");
	}

	// Data is string if present, nil if not
	if (datatype != LUA_TSTRING && datatype != LUA_TNONE) {
		luaL_argerror(L, 4, "must be string, if present");
	}

	if (nametype == LUA_TNIL || datatype == LUA_TNONE) {
		// If one of the parameters is missing, we need the original
		Lump lump = std::move(ptr->at(index - 1));

		if (nametype == LUA_TSTRING) {
			lump.setName(Lua::tostring(L, 3));
		}
		if (datatype == LUA_TSTRING) {
			lump.setData(Lua::tolstring(L, 4));
		}

		ptr->at(index - 1) = std::move(lump);
	} else {
		// Both parameters, so a brand new lump.
		Lump lump;
		lump.setName(Lua::tostring(L, 3));
		lump.setData(Lua::tolstring(L, 4));

		ptr->at(index - 1) = std::move(lump);
	}

	return 0;
}

// Write out a WAD file
static int ulumps_packwad(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));

	Wad wad(Wad::Type::PWAD);
	wad.setLumps(ptr);

	std::stringstream output;
	try {
		output << wad;
	} catch (const std::runtime_error& e) {
		return luaL_error(L, e.what());
	}

	std::string outstr = output.str();
	lua_pushlstring(L, outstr.data(), outstr.size());
	return 1;
}

// Write out a ZIP file
static int ulumps_packzip(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));

	Zip zip;
	zip.setLumps(ptr);

	std::stringstream output;
	try {
		output << zip;
	} catch (const std::runtime_error& e) {
		return luaL_error(L, e.what());
	}

	std::string outstr = output.str();
	lua_pushlstring(L, outstr.data(), outstr.size());
	return 1;
}

// Garbage-collect Lumps
static int ulumps_gc(lua_State* L) {
	auto ptr = static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));
	ptr->~shared_ptr();
	return 0;
}

// Return the length of the Lumps
static int ulumps_len(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));
	lua_pushinteger(L, ptr->size());
	return 1;
}

// Print Lumps as a string
static int ulumps_tostring(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));
	size_t size = ptr->size();
	if (size == 1) {
		lua_pushfstring(L, "%s: %p, %d lump", WADmake::META_LUMPS, ptr.get(), ptr->size());
	} else {
		lua_pushfstring(L, "%s: %p, %d lumps", WADmake::META_LUMPS,ptr.get(), ptr->size());
	}
	return 1;
}

// Functions attached to Lumps userdata
static const luaL_Reg ulumps_functions[] = {
	{"find", ulumps_find},
	{"get", ulumps_get},
	{"insert", ulumps_insert},
	{"remove", ulumps_remove},
	{"set", ulumps_set},
	{"packwad", ulumps_packwad},
	{"packzip", ulumps_packzip},
	{"__gc", ulumps_gc},
	{"__len", ulumps_len},
	{"__tostring", ulumps_tostring},
	{NULL, NULL}
};

// Functions in the wad package
static const luaL_Reg wad_functions[] = {
	{ "createLumps", wad_createLumps },
	{ "unpackwad", wad_unpackwad },
	{ "unpackzip", wad_unpackzip },
	{ NULL, NULL }
};

// Initialize the wad package
int luaopen_wad(lua_State* L) {
	// Fetch functions we created in Lua
	Lua::doBuffer(L, reinterpret_cast<char*>(luawad_lua), luawad_lua_len, "luawad.lua (internal)");
	// [wadfuncs][Lumpsfuncs]

	// Create "Lumps" userdata
	luaL_newmetatable(L, WADmake::META_LUMPS);
	// [wadfuncs][Lumpsfuncs][Lumpsmeta]
	lua_pushvalue(L, -1);
	// [wadfuncs][Lumpsfuncs][Lumpsmeta][Lumpsmeta]
	lua_setfield(L, -2, "__index");
	// [wadfuncs][Lumpsfuncs][Lumpsmeta]
	luaL_setfuncs(L, ulumps_functions, 0);
	Lua::settfuncs(L, -2);
	lua_pop(L, 2);
	// [wadfuncs]

	// Create "wad" package
	luaL_newlib(L, wad_functions);
	// [wadfuncs][wadlib]
	Lua::settfuncs(L, -2);
	lua_remove(L, -2);
	// [wadlib]

	return 1;
}

}

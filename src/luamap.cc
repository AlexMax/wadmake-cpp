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

#include "directory.hh"
#include "lualumps.hh"
#include "luamap.hh"
#include "map.hh"

namespace WADmake {

const char META_DOOMMAP[] = "DoomMap";

static int wad_createDoomMap(lua_State* L) {
	auto ptr = static_cast<std::shared_ptr<DoomMap>*>(lua_newuserdata(L, sizeof(std::shared_ptr<DoomMap>)));
	new(ptr) std::shared_ptr<DoomMap>(new DoomMap());
	luaL_setmetatable(L, WADmake::META_DOOMMAP);
	return 1;
}

// Given Lumps and an index (optional), unpack map data into a map userdata.
static int wad_unpackmap(lua_State* L) {
	auto lumps = *static_cast<std::shared_ptr<Directory>*>(luaL_checkudata(L, 1, WADmake::META_LUMPS));

	size_t index = 1;
	if (lua_isinteger(L, 2)) {
		index = lua_tointeger(L, 2);
	}

	DoomThings things;
	Sidedefs sidedefs;
	Vertexes vertexes;
	Sectors sectors;
	try {
		std::stringstream vertexesbuffer(lumps->at(index + 3).getData());
		vertexes.read(vertexesbuffer);
		std::stringstream sectorsbuffer(lumps->at(index + 7).getData());
		sectors.read(sectorsbuffer);
		std::stringstream sidedefsbuffer(lumps->at(index + 2).getData());
		sidedefs.read(sidedefsbuffer, sectors);
		std::stringstream thingsbuffer(lumps->at(index).getData());
		things.read(thingsbuffer);
	} catch (const std::runtime_error& e) {
		lua_pushstring(L, e.what());
		throw e;
	}

	auto ptr = static_cast<std::shared_ptr<DoomMap>*>(lua_newuserdata(L, sizeof(std::shared_ptr<DoomMap>)));
	new(ptr) std::shared_ptr<DoomMap>(new DoomMap());
	luaL_setmetatable(L, WADmake::META_DOOMMAP);

	(*ptr)->setThings(std::move(things));
	(*ptr)->setSidedefs(std::move(sidedefs));
	(*ptr)->setVertexes(std::move(vertexes));
	(*ptr)->setSectors(std::move(sectors));

	return 1;
}

static int udoommap_getsector(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	Sector sector;
	try {
		sector = ptr->getSectors().at(index - 1);
	} catch (const std::out_of_range& e) {
		lua_pushnil(L);
		return 1;
	}

	lua_newtable(L);
	lua_pushinteger(L, sector.floor);
	lua_setfield(L, -2, "floor");
	lua_pushinteger(L, sector.ceiling);
	lua_setfield(L, -2, "ceiling");
	lua_pushstring(L, sector.floortex.c_str());
	lua_setfield(L, -2, "floortex");
	lua_pushstring(L, sector.ceilingtex.c_str());
	lua_setfield(L, -2, "ceilingtex");
	lua_pushinteger(L, sector.light);
	lua_setfield(L, -2, "light");
	lua_pushinteger(L, sector.special);
	lua_setfield(L, -2, "special");
	lua_pushinteger(L, sector.tag);
	lua_setfield(L, -2, "tag");

	return 1;
}

static int udoommap_getsidedef(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	Sidedef sidedef;
	try {
		sidedef = ptr->getSidedefs().at(index - 1);
	} catch (const std::out_of_range& e) {
		lua_pushnil(L);
		return 1;
	}

	lua_newtable(L);
	lua_pushinteger(L, sidedef.xoffset);
	lua_setfield(L, -2, "xoffset");
	lua_pushinteger(L, sidedef.yoffset);
	lua_setfield(L, -2, "yoffset");
	lua_pushstring(L, sidedef.uppertex.c_str());
	lua_setfield(L, -2, "uppertex");
	lua_pushstring(L, sidedef.middletex.c_str());
	lua_setfield(L, -2, "middletex");
	lua_pushstring(L, sidedef.lowertex.c_str());
	lua_setfield(L, -2, "lowertex");
	lua_pushinteger(L, sidedef.sector.lock()->id + 1);
	lua_setfield(L, -2, "sector");

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

static int udoommap_getvertex(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	Vertex vertex;
	try {
		vertex = ptr->getVertexes().at(index - 1);
	} catch (const std::out_of_range& e) {
		lua_pushnil(L);
		return 1;
	}

	lua_newtable(L);
	lua_pushinteger(L, vertex.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, vertex.y);
	lua_setfield(L, -2, "y");

	return 1;
}

static int udoommap_setsector(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	Sector& sector = ptr->getSectors()[index - 1];

	lua_getfield(L, 3, "floor");
	if (!lua_isnil(L, -1)) {
		sector.floor = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "ceiling");
	if (!lua_isnil(L, -1)) {
		sector.ceiling = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "floortex");
	if (!lua_isnil(L, -1)) {
		sector.floortex = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "ceilingtex");
	if (!lua_isnil(L, -1)) {
		sector.ceilingtex = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "light");
	if (!lua_isnil(L, -1)) {
		sector.light = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "special");
	if (!lua_isnil(L, -1)) {
		sector.special = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "tag");
	if (!lua_isnil(L, -1)) {
		sector.tag = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	return 0;
}

static int udoommap_setsidedef(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	Sidedef& sidedef = ptr->getSidedefs()[index - 1];

	lua_getfield(L, 3, "xoffset");
	if (!lua_isnil(L, -1)) {
		sidedef.xoffset = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "yoffset");
	if (!lua_isnil(L, -1)) {
		sidedef.yoffset = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "uppertex");
	if (!lua_isnil(L, -1)) {
		sidedef.uppertex = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "middletex");
	if (!lua_isnil(L, -1)) {
		sidedef.middletex = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "lowertex");
	if (!lua_isnil(L, -1)) {
		sidedef.lowertex = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "sector");
	if (!lua_isnil(L, -1)) {
		size_t sectorid = lua_tointeger(L, -1);
		sidedef.sector = ptr->getSectors().lock(sectorid - 1);
	}
	lua_pop(L, 1);

	return 0;
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

static int udoommap_setvertex(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	Vertex& vertex = ptr->getVertexes()[index - 1];

	lua_getfield(L, 3, "x");
	if (!lua_isnil(L, -1)) {
		vertex.x = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "y");
	if (!lua_isnil(L, -1)) {
		vertex.y = lua_tointeger(L, -1);
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
	{"getsidedef", udoommap_getsidedef},
	{"getsector", udoommap_getsector},
	{"getthing", udoommap_getthing},
	{"getvertex", udoommap_getvertex},
	{"setsidedef", udoommap_setsidedef},
	{"setsector", udoommap_setsector},
	{"setthing", udoommap_setthing},
	{"setvertex", udoommap_setvertex},
	{"__gc", udoommap_gc},
	{NULL, NULL}
};

// Functions that go in the top-level wad package
static const luaL_Reg wad_functions[] = {
	{"createDoomMap", wad_createDoomMap},
	{"unpackmap", wad_unpackmap},
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

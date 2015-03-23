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
#include "lua.hh"
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
	DoomLinedefs linedefs;
	Sidedefs sidedefs;
	Vertexes vertexes;
	Sectors sectors;
	std::string segs, ssectors, nodes, reject, blockmap;
	try {
		std::stringstream vertexesbuffer(lumps->at(index + 3).getData());
		vertexes.read(vertexesbuffer);
		std::stringstream sectorsbuffer(lumps->at(index + 7).getData());
		sectors.read(sectorsbuffer);
		std::stringstream sidedefsbuffer(lumps->at(index + 2).getData());
		sidedefs.read(sidedefsbuffer, sectors);
		std::stringstream linedefsbuffer(lumps->at(index + 1).getData());
		linedefs.read(linedefsbuffer, vertexes, sidedefs);
		std::stringstream thingsbuffer(lumps->at(index).getData());
		things.read(thingsbuffer);
		std::stringstream segsbuffer(lumps->at(index + 4).getData());
		segs = segsbuffer.str();
		std::stringstream ssectorsbuffer(lumps->at(index + 5).getData());
		ssectors = segsbuffer.str();
		std::stringstream nodesbuffer(lumps->at(index + 6).getData());
		nodes = segsbuffer.str();
		std::stringstream rejectbuffer(lumps->at(index + 8).getData());
		reject = segsbuffer.str();
		std::stringstream blockmapbuffer(lumps->at(index + 9).getData());
		blockmap = segsbuffer.str();
	} catch (const std::runtime_error& e) {
		lua_pushstring(L, e.what());
		throw e;
	}

	auto ptr = static_cast<std::shared_ptr<DoomMap>*>(lua_newuserdata(L, sizeof(std::shared_ptr<DoomMap>)));
	new(ptr) std::shared_ptr<DoomMap>(new DoomMap());
	luaL_setmetatable(L, WADmake::META_DOOMMAP);

	(*ptr)->setThings(std::move(things));
	(*ptr)->setLinedefs(std::move(linedefs));
	(*ptr)->setSidedefs(std::move(sidedefs));
	(*ptr)->setVertexes(std::move(vertexes));
	(*ptr)->setSegs(std::move(segs));
	(*ptr)->setSsectors(std::move(ssectors));
	(*ptr)->setNodes(std::move(nodes));
	(*ptr)->setSectors(std::move(sectors));
	(*ptr)->setReject(std::move(reject));
	(*ptr)->setBlockmap(std::move(blockmap));

	return 1;
}

static int udoommap_getlinedef(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	DoomLinedef linedef;
	try {
		linedef = ptr->getLinedefs().at(index - 1);
	} catch (const std::out_of_range& e) {
		lua_pushnil(L);
		return 1;
	}

	lua_newtable(L);
	lua_pushinteger(L, linedef.startvertex.lock()->id + 1);
	lua_setfield(L, -2, "startvertex");
	lua_pushinteger(L, linedef.endvertex.lock()->id + 1);
	lua_setfield(L, -2, "endvertex");
	lua_pushinteger(L, linedef.flags.to_ulong());
	lua_setfield(L, -2, "flags");
	lua_pushinteger(L, linedef.special);
	lua_setfield(L, -2, "special");
	lua_pushinteger(L, linedef.tag);
	lua_setfield(L, -2, "tag");
	auto frontsidedef = linedef.frontsidedef.lock();
	if (frontsidedef) {
		lua_pushinteger(L, frontsidedef->id + 1);
		lua_setfield(L, -2, "frontsidedef");
	}
	auto backsidedef = linedef.backsidedef.lock();
	if (backsidedef) {
		lua_pushinteger(L, backsidedef->id + 1);
		lua_setfield(L, -2, "backsidedef");
	}

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

static int udoommap_packmap(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	// Check for map name parameter
	std::string name = Lua::checkstring(L, 2);

	// Create Directory for map data
	auto dir = static_cast<std::shared_ptr<Directory>*>(lua_newuserdata(L, sizeof(std::shared_ptr<Directory>)));
	new(dir) std::shared_ptr<Directory>(new Directory());
	luaL_setmetatable(L, WADmake::META_LUMPS);

	// Header
	Lump header;
	header.setName(std::move(name));
	(*dir)->push_back(std::move(header));

	// THINGS
	Lump things;
	things.setName("THINGS");
	std::stringstream thingsbuffer;
	ptr->getThings().write(thingsbuffer);
	things.setData(thingsbuffer.str());
	(*dir)->push_back(std::move(things));

	// LINEDEFS
	Lump linedefs;
	linedefs.setName("LINEDEFS");
	std::stringstream linedefsbuffer;
	ptr->getLinedefs().write(linedefsbuffer);
	linedefs.setData(linedefsbuffer.str());
	(*dir)->push_back(std::move(linedefs));

	// SIDEDEFS
	Lump sidedefs;
	sidedefs.setName("SIDEDEFS");
	std::stringstream sidedefsbuffer;
	ptr->getSidedefs().write(sidedefsbuffer);
	sidedefs.setData(sidedefsbuffer.str());
	(*dir)->push_back(std::move(sidedefs));

	// VERTEXES
	Lump vertexes;
	vertexes.setName("VERTEXES");
	std::stringstream vertexesbuffer;
	ptr->getVertexes().write(vertexesbuffer);
	vertexes.setData(vertexesbuffer.str());
	(*dir)->push_back(std::move(vertexes));

	// SEGS
	Lump segs;
	segs.setName("SEGS");
	std::string segsbuffer = ptr->getSegs();
	segs.setData(std::move(segsbuffer));
	(*dir)->push_back(std::move(segs));

	// SSECTORS
	Lump ssectors;
	ssectors.setName("SSECTORS");
	std::string ssectorsbuffer = ptr->getSsectors();
	ssectors.setData(std::move(ssectorsbuffer));
	(*dir)->push_back(std::move(ssectors));

	// NODES
	Lump nodes;
	nodes.setName("NODES");
	std::string nodesbuffer = ptr->getNodes();
	nodes.setData(std::move(nodesbuffer));
	(*dir)->push_back(std::move(nodes));

	// SECTORS
	Lump sectors;
	sectors.setName("SECTORS");
	std::stringstream sectorsbuffer;
	ptr->getSectors().write(sectorsbuffer);
	sectors.setData(sectorsbuffer.str());
	(*dir)->push_back(std::move(sectors));

	// REJECT
	Lump reject;
	reject.setName("REJECT");
	std::string rejectbuffer = ptr->getReject();
	reject.setData(std::move(rejectbuffer));
	(*dir)->push_back(std::move(reject));

	// BLOCKMAP
	Lump blockmap;
	blockmap.setName("BLOCKMAP");
	std::string blockmapbuffer = ptr->getBlockmap();
	reject.setData(std::move(blockmapbuffer));
	(*dir)->push_back(std::move(blockmap));

	return 1;
}

static int udoommap_setlinedef(lua_State* L) {
	auto ptr = *static_cast<std::shared_ptr<DoomMap>*>(luaL_checkudata(L, 1, WADmake::META_DOOMMAP));

	size_t index = luaL_checkinteger(L, 2);
	DoomLinedef& linedef = ptr->getLinedefs()[index - 1];

	lua_getfield(L, 3, "startvertex");
	if (!lua_isnil(L, -1)) {
		size_t vertexid = lua_tointeger(L, -1);
		linedef.startvertex = ptr->getVertexes().lock(vertexid - 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "endvertex");
	if (!lua_isnil(L, -1)) {
		size_t vertexid = lua_tointeger(L, -1);
		linedef.endvertex = ptr->getVertexes().lock(vertexid - 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "special");
	if (!lua_isnil(L, -1)) {
		linedef.special = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "tag");
	if (!lua_isnil(L, -1)) {
		linedef.tag = lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "frontsidedef");
	if (!lua_isnil(L, -1)) {
		size_t sidedefid = lua_tointeger(L, -1);
		linedef.frontsidedef = ptr->getSidedefs().lock(sidedefid - 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 3, "backsidedef");
	if (!lua_isnil(L, -1)) {
		size_t sidedefid = lua_tointeger(L, -1);
		linedef.backsidedef = ptr->getSidedefs().lock(sidedefid - 1);
	}
	lua_pop(L, 1);

	return 0;
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
	{"getlinedef", udoommap_getlinedef},
	{"getsector", udoommap_getsector},
	{"getsidedef", udoommap_getsidedef},
	{"getthing", udoommap_getthing},
	{"packmap", udoommap_packmap},
	{"getvertex", udoommap_getvertex},
	{"setlinedef", udoommap_setlinedef},
	{"setsector", udoommap_setsector},
	{"setsidedef", udoommap_setsidedef},
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

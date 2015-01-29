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

#define CATCH_CONFIG_MAIN
#include "catch.hh"

#include "lua.hh"
#include "wad.hh"

// We do not want to implement this method in the live binary, as its
// use in the codebase itself would almost certainly a mistake, but
// it's very handy for unit-testing the Lua environment.
LuaState* LuaEnvironment::getState() {
	return &(this->lua);
}

TEST_CASE("Wad can construct from buffer", "[wad]") {
	std::stringstream buffer;
	std::ifstream moo2d_wad("moo2d.wad", std::fstream::in | std::fstream::binary);
	Wad moo2d(moo2d_wad);
	Directory dir = moo2d.getLumps();
	REQUIRE(dir.size() == 11);
}

TEST_CASE("Environment should be created correctly", "[lwad]") {
	LuaEnvironment lua;
	LuaState* L = lua.getState();
	SECTION("Don't leave anything on the stack when creating the environment") {
		REQUIRE(lua_gettop(*L) == 0);
	}
	SECTION("wad package should exist") {
		REQUIRE(lua_getglobal(*L, "wad") == LUA_TTABLE);
		REQUIRE(lua_getfield(*L, -1, "readwad") == LUA_TFUNCTION);
		REQUIRE(lua_getfield(*L, -2, "openwad") == LUA_TFUNCTION);
	}
}

TEST_CASE("Lumps can be created from scratch", "[lwad]") {
	LuaEnvironment lua;
	lua.dostring("return wad.createLumps()");

	LuaState* L = lua.getState();
	REQUIRE(luaL_checkudata(*L, -1, "Lumps") != NULL);
	REQUIRE(luaL_len(*L, -1) == 0);
}

TEST_CASE("Lumps can be created from data", "[lwad]") {
	LuaEnvironment lua;
	lua.dostring("file = io.open('moo2d.wad');data = file:read('a');return wad.readwad(data)");

	LuaState* L = lua.getState();
	REQUIRE(lua_type(*L, -1) == LUA_TTABLE);

	REQUIRE(lua_getfield(*L, -1, "type") == LUA_TSTRING);
	REQUIRE(Lua::checkstring(*L, -1) == std::string("pwad"));
	lua_pop(*L, 1);

	REQUIRE(lua_getfield(*L, -1, "lumps") == LUA_TUSERDATA);
	REQUIRE(luaL_checkudata(*L, -1, "Lumps") != NULL);
	REQUIRE(luaL_len(*L, -1) == 11);
}

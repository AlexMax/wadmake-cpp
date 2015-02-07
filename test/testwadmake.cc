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

#include "buffer.hh"
#include "lua.hh"
#include "wad.hh"
#include "zip.hh"

// We do not want to implement this method in the live binary, as its
// use in the codebase itself would almost certainly a mistake, but
// it's very handy for unit-testing the Lua environment.
LuaState* LuaEnvironment::getState() {
	return &(this->lua);
}

TEST_CASE("ReadString can read into string given a stream and length", "[bit]") {
	std::stringstream buffer;
	buffer << 'A' << 'B' << '\0' << 'D';
	std::string actual = ReadString(buffer, 4);
	REQUIRE(actual.size() == 4);
	REQUIRE(actual == buffer.str());
}

TEST_CASE("ReadUInt16LE can read 16-bit Little Endian integers", "[bit]") {
	std::stringstream buffer;
	buffer << "\xFF\xFE";
	REQUIRE(ReadUInt16LE(buffer) == 65279);
}

TEST_CASE("ReadUInt32LE can read 32-bit Little Endian integers", "[bit]") {
	std::stringstream buffer;
	buffer << "\xFF\xFE\xFD\xFC";
	REQUIRE(ReadUInt32LE(buffer) == 4244504319);
}

TEST_CASE("ReadUInt64LE can read 64-bit Little Endian integers", "[bit]") {
	std::stringstream buffer;
	buffer << "\xFF\xFE\xFD\xFC\xFB\xFA\xF9\xF8";
	REQUIRE(ReadUInt64LE(buffer) == 18446744073659088639ull);
}

TEST_CASE("Wad can construct from buffer", "[wad]") {
	std::stringstream buffer;
	std::ifstream moo2d_wad("moo2d.wad", std::fstream::in | std::fstream::binary);

	Wad moo2d(Wad::Type::NONE);
	moo2d_wad >> moo2d;

	Directory dir = moo2d.getLumps();
	REQUIRE(dir.size() == 11);
}

TEST_CASE("Zip can construct from buffer", "[zip]") {
	std::stringstream buffer;
	std::ifstream duel32f_pk3("duel32f.pk3", std::fstream::in | std::fstream::binary);

	Zip duel32;
	duel32f_pk3 >> duel32;

	Directory dir = duel32.getLumps();
	REQUIRE(dir.size() == 369);
}

TEST_CASE("Environment should be created correctly", "[lua]") {
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

TEST_CASE("Environment should handle panics as exceptions", "[lua]") {
	LuaEnvironment lua;
	LuaState* L = lua.getState();
	REQUIRE_THROWS_AS(luaL_error(*L, "This error should not abort the program"), LuaPanic);
}

TEST_CASE("Lumps can be created from scratch", "[lwad]") {
	LuaEnvironment lua;
	lua.doString("return wad.createLumps()", "test");

	LuaState* L = lua.getState();
	REQUIRE(luaL_checkudata(*L, -1, "Lumps") != NULL);
	REQUIRE(luaL_len(*L, -1) == 0);
}

TEST_CASE("Lumps can be created from data", "[lwad]") {
	LuaEnvironment lua;
	lua.doString("return wad.openwad('moo2d.wad')", "test");

	LuaState* L = lua.getState();

	REQUIRE(Lua::checkstring(*L, -2) == "pwad");
	REQUIRE(luaL_checkudata(*L, -1, "Lumps") != NULL);
	REQUIRE(luaL_len(*L, -1) == 11);
}

TEST_CASE("Test Lumps:find()", "[lwad]") {
	LuaEnvironment lua;
	lua.doString("return wad.openwad('moo2d.wad')", "test");

	LuaState* L = lua.getState();
	SECTION("Find a lump index") {
		luaL_loadstring(*L, "return (...):find('SIDEDEFS')"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(*L, -1) == 4);
	}

	SECTION("Find a lump index given a starting index") {
		luaL_loadstring(*L, "return (...):find('SIDEDEFS', 2)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(*L, -1) == 4);
	}

	SECTION("Find a lump index given a negative starting index") {
		luaL_loadstring(*L, "return (...):find('SIDEDEFS', -9)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(*L, -1) == 4);
	}

	SECTION("Find a lump index given the precise starting index") {
		luaL_loadstring(*L, "return (...):find('SIDEDEFS', 4)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(*L, -1) == 4);
	}

	SECTION("Find a lump index given the precise negative starting index") {
		luaL_loadstring(*L, "return (...):find('SIDEDEFS', -8)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(*L, -1) == 4);
	}

	SECTION("Return nil if lump doesn't exist") {
		luaL_loadstring(*L, "return (...):find('MAP00')"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(lua_isnil(*L, -1));
	}

	SECTION("Return nil if our find operation starts past the correct lump") {
		luaL_loadstring(*L, "return (...):find('SIDEDEFS', 5)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(lua_isnil(*L, -1));
	}
}

TEST_CASE("Test Lumps:get()", "[lwad]") {
	LuaEnvironment lua;
	lua.doString("return wad.openwad('moo2d.wad')", "test");

	LuaState* L = lua.getState();

	SECTION("Returns a lump name and data") {
		luaL_loadstring(*L, "return (...):get(2)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [name][value]

		REQUIRE(Lua::checkstring(*L, -2) == "THINGS");
		REQUIRE(Lua::checklstring(*L, -1).size() == 150);
	}
}

TEST_CASE("Test Lumps:insert()", "[lwad]") {
	LuaEnvironment lua;
	lua.doString("return wad.openwad('moo2d.wad')", "test");

	LuaState* L = lua.getState();

	SECTION("Append a lump to the end") {
		luaL_loadstring(*L, "(...):insert('MAP02', 'hissy');return (...):get(12)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [name]

		REQUIRE(Lua::checkstring(*L, -2) == "MAP02");
		REQUIRE(Lua::checkstring(*L, -1) == "hissy");
	}

	SECTION("Insert a lump in the middle") {
		luaL_loadstring(*L, "(...):insert(2, 'TEST', 'hissy');return (...):get(2)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [name]

		REQUIRE(Lua::checkstring(*L, -2) == "TEST");
		REQUIRE(Lua::checkstring(*L, -1) == "hissy");
	}
}

TEST_CASE("Test Lumps:set()", "[lwad]") {
	LuaEnvironment lua;
	lua.doString("return wad.openwad('moo2d.wad')", "test");

	LuaState* L = lua.getState();

	SECTION("Set a lump name") {
		luaL_loadstring(*L, "(...):set(1, 'MAP02');return (...):get(1)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [name]

		REQUIRE(Lua::checkstring(*L, -2) == "MAP02");
	}

	SECTION("Set lump data") {
		luaL_loadstring(*L, "(...):set(1, nil, 'hissy');return (...):get(1)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [data]

		REQUIRE(Lua::checkstring(*L, -1) == "hissy");
	}

	SECTION("Set both name and data") {
		luaL_loadstring(*L, "(...):set(1, 'MAP02', 'hissy');return (...):get(1)"); // [lumps][function]
		lua_insert(*L, -2); // [function][lumps]
		lua_pcall(*L, 1, LUA_MULTRET, 0); // [name][data]

		REQUIRE(Lua::checkstring(*L, -2) == "MAP02");
		REQUIRE(Lua::checkstring(*L, -1) == "hissy");
	}
}

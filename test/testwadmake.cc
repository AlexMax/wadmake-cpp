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

namespace WADmake {

// We do not want to implement this method in the live binary, as its
// use in the codebase itself would almost certainly a mistake, but
// it's very handy for unit-testing the Lua environment.
lua_State* LuaEnvironment::getState() {
	return this->lua;
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
	buffer << "\xFE\xFF";
	REQUIRE(ReadUInt16LE(buffer) == 0xFFFE);
}

TEST_CASE("WriteUInt16LE can write 16-bit Little Endian integers", "[bit]") {
	std::stringstream buffer;
	WriteUInt16LE(buffer, 0xFFFE);
	REQUIRE(buffer.str()[0] == '\xFE');
	REQUIRE(buffer.str()[1] == '\xFF');
}

TEST_CASE("ReadUInt32LE can read 32-bit Little Endian integers", "[bit]") {
	std::stringstream buffer;
	buffer << "\xFC\xFD\xFE\xFF";
	REQUIRE(ReadUInt32LE(buffer) == 0xFFFEFDFC);
}

TEST_CASE("WriteUInt32LE can write 32-bit Little Endian integers", "[bit]") {
	std::stringstream buffer;
	WriteUInt32LE(buffer, 0xFFFEFDFC);
	REQUIRE(buffer.str()[0] == '\xFC');
	REQUIRE(buffer.str()[1] == '\xFD');
	REQUIRE(buffer.str()[2] == '\xFE');
	REQUIRE(buffer.str()[3] == '\xFF');
}

TEST_CASE("ReadUInt64LE can read 64-bit Little Endian integers", "[bit]") {
	std::stringstream buffer;
	buffer << "\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF";
	REQUIRE(ReadUInt64LE(buffer) == 0xFFFEFDFCFBFAF9F8);
}

TEST_CASE("WriteUInt64LE can write 64-bit Little Endian integers", "[bit]") {
	std::stringstream buffer;
	WriteUInt64LE(buffer, 0xFFFEFDFCFBFAF9F8);
	REQUIRE(buffer.str()[0] == '\xF8');
	REQUIRE(buffer.str()[1] == '\xF9');
	REQUIRE(buffer.str()[2] == '\xFA');
	REQUIRE(buffer.str()[3] == '\xFB');
	REQUIRE(buffer.str()[4] == '\xFC');
	REQUIRE(buffer.str()[5] == '\xFD');
	REQUIRE(buffer.str()[6] == '\xFE');
	REQUIRE(buffer.str()[7] == '\xFF');
}

TEST_CASE("Wad can construct from buffer", "[wad]") {
	std::stringstream buffer;
	std::ifstream moo2d_wad("moo2d.wad", std::fstream::in | std::fstream::binary);

	Wad moo2d(Wad::Type::NONE);
	moo2d_wad >> moo2d;

	auto dir = moo2d.getLumps();
	REQUIRE(dir->size() == 11);
}

TEST_CASE("Zip can construct from buffer", "[zip]") {
	std::stringstream buffer;
	std::ifstream duel32f_pk3("duel32f.pk3", std::fstream::in | std::fstream::binary);

	Zip duel32;
	duel32f_pk3 >> duel32;

	auto dir = duel32.getLumps();
	REQUIRE(dir->size() == 369);
}

TEST_CASE("Environment should be created correctly", "[lua]") {
	LuaEnvironment lua;
	lua_State* L = lua.getState();
	SECTION("Don't leave anything on the stack when creating the environment") {
		REQUIRE(lua_gettop(L) == 0);
	}
	SECTION("wad package should exist") {
		REQUIRE(lua_getglobal(L, "wad") == LUA_TTABLE);
		REQUIRE(lua_getfield(L, -1, "readwad") == LUA_TFUNCTION);
		REQUIRE(lua_getfield(L, -2, "openwad") == LUA_TFUNCTION);
	}
}

TEST_CASE("Environment should handle panics as exceptions", "[lua]") {
	LuaEnvironment lua;
	lua_State* L = lua.getState();
	REQUIRE_THROWS_AS(luaL_error(L, "This error should not abort the program"), LuaPanic);
}

TEST_CASE("Lumps can be created from scratch", "[luawad]") {
	LuaEnvironment lua;
	lua.doString("return wad.createLumps()", "test");

	lua_State* L = lua.getState();
	REQUIRE(luaL_checkudata(L, -1, "Lumps") != NULL);
	REQUIRE(luaL_len(L, -1) == 0);
}

TEST_CASE("Lumps can be created from WAD file", "[luawad]") {
	LuaEnvironment lua;
	lua.doString("return wad.openwad('moo2d.wad')", "test");

	lua_State* L = lua.getState();

	REQUIRE(Lua::checkstring(L, -1) == "pwad");
	REQUIRE(luaL_checkudata(L, -2, "Lumps") != NULL);
	REQUIRE(luaL_len(L, -2) == 11);
}

TEST_CASE("Test Lumps:find()", "[luawad]") {
	LuaEnvironment lua;
	lua.doString("lumps = wad.openwad('moo2d.wad')", "test");

	lua_State* L = lua.getState();
	SECTION("Find a lump index") {
		luaL_loadstring(L, "return lumps:find('SIDEDEFS')"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(L, -1) == 4);
	}

	SECTION("Find a lump index given a starting index") {
		luaL_loadstring(L, "return lumps:find('SIDEDEFS', 2)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(L, -1) == 4);
	}

	SECTION("Find a lump index given a negative starting index") {
		luaL_loadstring(L, "return lumps:find('SIDEDEFS', -9)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(L, -1) == 4);
	}

	SECTION("Find a lump index given the precise starting index") {
		luaL_loadstring(L, "return lumps:find('SIDEDEFS', 4)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(L, -1) == 4);
	}

	SECTION("Find a lump index given the precise negative starting index") {
		luaL_loadstring(L, "return lumps:find('SIDEDEFS', -8)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(luaL_checkinteger(L, -1) == 4);
	}

	SECTION("Return nil if lump doesn't exist") {
		luaL_loadstring(L, "return lumps:find('MAP00')"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(lua_isnil(L, -1));
	}

	SECTION("Return nil if our find operation starts past the correct lump") {
		luaL_loadstring(L, "return lumps:find('SIDEDEFS', 5)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [position]

		REQUIRE(lua_isnil(L, -1));
	}
}

TEST_CASE("Test Lumps:get()", "[luawad]") {
	LuaEnvironment lua;
	lua.doString("lumps = wad.openwad('moo2d.wad')", "test");

	lua_State* L = lua.getState();

	SECTION("Returns a lump name and data") {
		luaL_loadstring(L, "return lumps:get(2)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [name][value]

		REQUIRE(Lua::checkstring(L, -2) == "THINGS");
		REQUIRE(Lua::checklstring(L, -1).size() == 150);
	}
}

TEST_CASE("Test Lumps:insert()", "[luawad]") {
	LuaEnvironment lua;
	lua.doString("lumps = wad.openwad('moo2d.wad')", "test");

	lua_State* L = lua.getState();

	SECTION("Append a lump to the end") {
		luaL_loadstring(L, "lumps:insert('MAP02', 'hissy');return lumps:get(12)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [name]

		REQUIRE(Lua::checkstring(L, -2) == "MAP02");
		REQUIRE(Lua::checkstring(L, -1) == "hissy");
	}

	SECTION("Insert a lump in the middle") {
		luaL_loadstring(L, "lumps:insert(2, 'TEST', 'hissy');return lumps:get(2)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [name]

		REQUIRE(Lua::checkstring(L, -2) == "TEST");
		REQUIRE(Lua::checkstring(L, -1) == "hissy");
	}
}

TEST_CASE("Test Lumps:set()", "[luawad]") {
	LuaEnvironment lua;
	lua.doString("lumps = wad.openwad('moo2d.wad')", "test");

	lua_State* L = lua.getState();

	SECTION("Set a lump name") {
		luaL_loadstring(L, "lumps:set(1, 'MAP02');return lumps:get(1)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [name]

		REQUIRE(Lua::checkstring(L, -2) == "MAP02");
	}

	SECTION("Set lump data") {
		luaL_loadstring(L, "lumps:set(1, nil, 'hissy');return lumps:get(1)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [data]

		REQUIRE(Lua::checkstring(L, -1) == "hissy");
	}

	SECTION("Set both name and data") {
		luaL_loadstring(L, "lumps:set(1, 'MAP02', 'hissy');return lumps:get(1)"); // [lumps][function]
		lua_insert(L, -2); // [function][lumps]
		lua_pcall(L, 1, LUA_MULTRET, 0); // [name][data]

		REQUIRE(Lua::checkstring(L, -2) == "MAP02");
		REQUIRE(Lua::checkstring(L, -1) == "hissy");
	}
}

TEST_CASE("Test Lumps:writewad()", "[luawad]") {
	LuaEnvironment lua;
	lua.doString("x = wad.openwad('moo2d.wad');y = x:writewad('pwad');z = wad.readwad(y)", "test");

	lua_State* L = lua.getState();

	SECTION("See if we can successfully read the WAD data we output") {
		luaL_dostring(L, "return z:get(1)");

		REQUIRE(Lua::checkstring(L, -2) == "MAP01");
		REQUIRE(Lua::checkstring(L, -1) == "");
	}
}

}

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

#ifndef LUA_HH
#define LUA_HH

#include <stdexcept>
#include <string>
#include <memory>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace WADmake {

class LuaPanic : public std::runtime_error {
public:
	LuaPanic() : std::runtime_error("") { }
	LuaPanic(const char* what) : std::runtime_error(what) { }
};

class LuaState {
	std::unique_ptr<lua_State, decltype(&lua_close)> lua;
	static int panic(lua_State* L);
public:
	LuaState();
	operator lua_State*();
};

class LuaEnvironment {
	LuaState lua;
public:
	LuaEnvironment();
	void doFile(const char* filename);
	void doBuffer(const char* str, size_t len, const char* name);
	void doString(const std::string& str, const char* name);
	lua_State* getState(); // Test-only
	int gettop();
	std::ostream& writeStack(std::ostream& buffer);
};

class Lua {
public:
	static std::string checklstring(lua_State* L, int arg);
	static std::string checkstring(lua_State* L, int arg);
	static void doBuffer(lua_State* L, const char* str, size_t len, const char* name);
	static void settfuncs(lua_State* L, int index);
	static std::string tolstring(lua_State* L, int index);
	static std::string tostring(lua_State* L, int index);
};

}

#endif

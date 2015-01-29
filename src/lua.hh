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

#include <memory>
#include <string>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

class LuaState {
	std::unique_ptr<lua_State, decltype(&lua_close)> lua;
public:
	LuaState() : lua(luaL_newstate(), lua_close) { }
	operator lua_State*();
};

class LuaEnvironment {
	LuaState lua;
public:
	LuaEnvironment();
	void dofile(const char* filename);
	void dostring(const char* str);
	LuaState* getState(); // Test-only
};

class Lua {
public:
	static std::string checklstring(lua_State* L, int arg);
	static std::string checkstring(lua_State* L, int arg);
	static void settfuncs(lua_State* L, int index);
	static std::string tolstring(lua_State* L, int index);
	static std::string tostring(lua_State* L, int index);
};

#endif

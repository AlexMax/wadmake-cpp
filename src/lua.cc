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
#include <stdexcept>

#include "lua.hh"
#include "lwad.hh"

#include "init.lua.hh"

LuaState::operator lua_State*() {
	return this->lua.get();
}

LuaEnvironment::LuaEnvironment() {
	luaL_requiref(this->lua, "_G", luaopen_base, 1);
	luaL_requiref(this->lua, LUA_LOADLIBNAME, luaopen_package, 1);
	luaL_requiref(this->lua, LUA_IOLIBNAME, luaopen_io, 1);
	luaL_requiref(this->lua, "wad", luaopen_wad, 1);
	lua_pop(this->lua, 4);

	if (luaL_loadbuffer(this->lua, (char*)src_lua_init_lua, src_lua_init_lua_len, "init") != LUA_OK) {
		std::stringstream error;
		error << "internal lua error" << std::endl << lua_tostring(this->lua, -1);
		throw std::runtime_error(error.str());
	}

	if (lua_pcall(this->lua, 0, LUA_MULTRET, 0) != LUA_OK) {
		std::stringstream error;
		error << "internal lua runtime error" << std::endl << lua_tostring(this->lua, -1);
		throw std::runtime_error(error.str());
	}
}

void LuaEnvironment::dofile(const char* filename) {
	if (luaL_dofile(this->lua, filename) == 1) {
		std::stringstream error;
		error << "runtime error" << std::endl << lua_tostring(this->lua, -1);
		throw std::runtime_error(error.str());
	}
}

void LuaEnvironment::dostring(const char* str) {
	if (luaL_dostring(this->lua, str) == 1) {
		std::stringstream error;
		error << "runtime error" << std::endl << lua_tostring(this->lua, -1);
		throw std::runtime_error(error.str());
	}
}

std::string Lua::checklstring(lua_State* L, int arg) {
	size_t len;
	const char* buffer = luaL_checklstring(L, arg, &len);
	return std::string(buffer, len);
}

std::string Lua::checkstring(lua_State* L, int arg) {
	return std::string(luaL_checkstring(L, arg));
}

// Registers all functions in the table on top of the stack to the table
// pointed to by index.  Pops the table of functions off the stack.
void Lua::settfuncs(lua_State* L, int index) {
	// [tfuncs]
	lua_pushnil(L);
	// [tfuncs][nil]
	while (lua_next(L, -2) != 0) {
		// [tfuncs][key][value]
		lua_pushvalue(L, -2);
		// [tfuncs][key][value][key]
		lua_insert(L, -2);
		// [tfuncs][key][key][value]
		lua_settable(L, index - 3);
		// [tfuncs][key]
	}
	// [tfuncs]
	lua_pop(L, 1);
}

std::string Lua::tolstring(lua_State* L, int index) {
	size_t len;
	const char* buffer = lua_tolstring(L, index, &len);
	return std::string(buffer, len);
}

std::string Lua::tostring(lua_State* L, int index) {
	return std::string(lua_tostring(L, index));
}

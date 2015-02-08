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

#include "lua.hh"
#include "luawad.hh"

#include "init.lua.hh"

namespace WADmake {

int LuaState::panic(lua_State* L) {
	if (lua_type(L, -1) == LUA_TSTRING) {
		throw LuaPanic(lua_tostring(L, -1));
	} else {
		throw LuaPanic();
	}
}

LuaState::LuaState() : lua(luaL_newstate(), lua_close) {
	lua_atpanic(this->lua.get(), this->panic);
}

LuaState::operator lua_State*() {
	return this->lua.get();
}

LuaEnvironment::LuaEnvironment() {
	luaL_requiref(this->lua, "_G", luaopen_base, 1);
	luaL_requiref(this->lua, LUA_IOLIBNAME, luaopen_io, 1);
	luaL_requiref(this->lua, LUA_LOADLIBNAME, luaopen_package, 1);
	luaL_requiref(this->lua, LUA_STRLIBNAME, luaopen_string, 1);
	luaL_requiref(this->lua, LUA_TABLIBNAME, luaopen_table, 1);
	luaL_requiref(this->lua, "wad", luaopen_wad, 1);
	lua_pop(this->lua, 6);

	if (luaL_loadbuffer(this->lua, (char*)init_lua, init_lua_len, "init") != LUA_OK) {
		std::stringstream error;
		error << "internal lua error" << std::endl << lua_tostring(this->lua, -1);
		lua_pop(this->lua, 1);
		throw std::runtime_error(error.str());
	}

	if (lua_pcall(this->lua, 0, LUA_MULTRET, 0) != LUA_OK) {
		std::stringstream error;
		error << "internal lua runtime error" << std::endl << lua_tostring(this->lua, -1);
		lua_pop(this->lua, 1);
		throw std::runtime_error(error.str());
	}
}

void LuaEnvironment::doFile(const char* filename) {
	if (luaL_loadfile(this->lua, filename) != LUA_OK) {
		std::stringstream error;
		error << "lua error: " << lua_tostring(this->lua, -1);
		lua_pop(this->lua, 1);
		throw std::runtime_error(error.str());
	}

	if (lua_pcall(this->lua, 0, LUA_MULTRET, 0) != LUA_OK) {
		std::stringstream error;
		error << "lua runtime error: " << lua_tostring(this->lua, -1);
		lua_pop(this->lua, 1);
		throw std::runtime_error(error.str());
	}
}

void LuaEnvironment::doBuffer(const char* str, size_t len, const char* name) {
	if (luaL_loadbuffer(this->lua, str, len , name) != LUA_OK) {
		std::stringstream error;
		error << "lua error: " << lua_tostring(this->lua, -1);
		lua_pop(this->lua, 1);
		throw std::runtime_error(error.str());
	}

	if (lua_pcall(this->lua, 0, LUA_MULTRET, 0) != LUA_OK) {
		std::stringstream error;
		error << "lua runtime error: " << lua_tostring(this->lua, -1);
		lua_pop(this->lua, 1);
		throw std::runtime_error(error.str());
	}
}

void LuaEnvironment::doString(const std::string& str, const char* name) {
	if (luaL_loadbuffer(this->lua, str.data(), str.size(), name) != LUA_OK) {
		std::stringstream error;
		error << "lua error: " << lua_tostring(this->lua, -1);
		lua_pop(this->lua, 1);
		throw std::runtime_error(error.str());
	}

	if (lua_pcall(this->lua, 0, LUA_MULTRET, 0) != LUA_OK) {
		std::stringstream error;
		error << "lua runtime error: " << lua_tostring(this->lua, -1);
		lua_pop(this->lua, 1);
		throw std::runtime_error(error.str());
	}
}

int LuaEnvironment::gettop() {
	return lua_gettop(this->lua);
}

std::ostream& LuaEnvironment::writeStack(std::ostream& buffer) {
	for (int i = 1;i <= lua_gettop(this->lua);i++) {
		if (i > 1) {
			buffer << ", ";
		}
		buffer << Lua::checklstring(this->lua, i);
	}
	lua_settop(this->lua, 0);
	return buffer;
}

std::string Lua::checklstring(lua_State* L, int arg) {
	size_t len;
	const char* buffer = luaL_checklstring(L, arg, &len);
	return std::string(buffer, len);
}

std::string Lua::checkstring(lua_State* L, int arg) {
	return std::string(luaL_checkstring(L, arg));
}

void Lua::doBuffer(lua_State* L, const char* str, size_t len, const char* name) {
	if (luaL_loadbuffer(L, str, len, name) != LUA_OK) {
		std::stringstream error;
		error << "lua error: " << lua_tostring(L, -1);
		lua_pop(L, 1);
		throw std::runtime_error(error.str());
	}

	if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
		std::stringstream error;
		error << "lua runtime error: " << lua_tostring(L, -1);
		lua_pop(L, 1);
		throw std::runtime_error(error.str());
	}
}

// Registers all functions in the table pointed to by index to the table
// on top of the stack.
void Lua::settfuncs(lua_State* L, int index) {
	// [dest]
	lua_pushnil(L);
	// [dest][nil]
	while (lua_next(L, index - 1) != 0) {
		// [dest][fname][func]
		lua_pushvalue(L, -2);
		// [dest][fname][func][fname]
		lua_insert(L, -2);
		// [dest][fname][fname][func]
		lua_settable(L, -4);
		// [dest][fname]
	}
	// [dest]
}

std::string Lua::tolstring(lua_State* L, int index) {
	size_t len;
	const char* buffer = lua_tolstring(L, index, &len);
	return std::string(buffer, len);
}

std::string Lua::tostring(lua_State* L, int index) {
	return std::string(lua_tostring(L, index));
}

}
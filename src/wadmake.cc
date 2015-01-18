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

#include <iostream>
#include <stdexcept>
#include <memory>

#include "lua.hh"
#include "lwad.hh"
#include "wad.hh"
#include "init.lua.hh"

int main() {
	try {
		const char* filename = "wadmakefile";

		LuaState Lua;

		luaL_requiref(Lua, "_G", luaopen_base, 1);
		luaL_requiref(Lua, LUA_LOADLIBNAME, luaopen_package, 1);
		luaL_requiref(Lua, LUA_IOLIBNAME, luaopen_io, 1);
		luaL_requiref(Lua, "wad", luaopen_wad, 1);
		lua_pop(Lua, 1);

		if (luaL_loadbuffer(Lua, (char*)src_lua_init_lua, src_lua_init_lua_len, "init") != LUA_OK) {
			std::stringstream error;
			error << "internal lua error " << std::endl << lua_tostring(Lua, -1);
			throw std::runtime_error(error.str());
		}

		if (lua_pcall(Lua, 0, LUA_MULTRET, 0) != LUA_OK) {
			std::stringstream error;
			error << "internal lua runtime error " << std::endl << lua_tostring(Lua, -1);
			throw std::runtime_error(error.str());
		}

		if (luaL_dofile(Lua, filename) == 1) {
			std::stringstream error;
			error << "runtime error " << filename;
			throw std::runtime_error(error.str());
		}

		return EXIT_SUCCESS;
	} catch (std::exception& e) {
		std::cerr << "wadmake: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

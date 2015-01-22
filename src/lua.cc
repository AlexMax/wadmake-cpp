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

#include "lua.hh"

LuaState::operator lua_State*() {
	return this->lua.get();
}

std::string Lua::checklstring(lua_State* L, int arg) {
	size_t len;
	const char* buffer = luaL_checklstring(L, arg, &len);
	return std::string(buffer, len);
}

std::string Lua::checkstring(lua_State* L, int arg) {
	return std::string(luaL_checkstring(L, arg));
}

std::string Lua::tolstring(lua_State* L, int index) {
	size_t len;
	const char* buffer = lua_tolstring(L, index, &len);
	return std::string(buffer, len);
}

std::string Lua::tostring(lua_State* L, int index) {
	return std::string(lua_tostring(L, index));
}

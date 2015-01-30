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

#ifndef WAD_HH
#define WAD_HH

#include <sstream>
#include <string>
#include <vector>

class Lump {
	std::string name;
	std::string data;
public:
	const std::string& getName();
	const std::string& getData();
	void setName(std::string&& name);
	void setData(std::string&& name);
	void setData(std::vector<char>&& data);
};

class Directory {
	std::vector<Lump> index;
public:
	Lump& at(size_t n);
	void erase_at(size_t index);
	void insert_at(size_t index, Lump&& lump);
	void push_back(Lump&& lump);
	size_t size();
};

class Wad {
public:
	enum class Type { IWAD, PWAD };
	Wad(Wad::Type type);
	Wad(std::istream& buffer);
	Wad::Type getType();
	const Directory& getLumps();
private:
	Type type;
	Directory lumps;
};

#endif

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

#ifndef DIRECTORY_HH
#define DIRECTORY_HH

#include <string>
#include <tuple>
#include <vector>

namespace WADmake {

class Lump {
	std::string name;
	std::string data;
public:
	const std::string getName() const;
	const std::string getData() const;
	void setName(std::string&& name);
	void setData(std::string&& name);
	void setData(std::vector<char>&& data);
};

class Directory {
	std::vector<Lump> index;
public:
	Lump& at(size_t n);
	std::vector<Lump>::const_iterator begin();
	std::vector<Lump>::const_iterator end();
	void erase_at(size_t index);
	std::tuple<bool, size_t> find_index(const std::string& name, size_t start);
	void insert_at(size_t index, Lump&& lump);
	void push_back(Lump&& lump);
	size_t size();
};

}

#endif

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

#include <algorithm>

#include "directory.hh"

namespace WADmake {

const std::string Lump::getName() const {
	return this->name;
}

const std::string Lump::getData() const {
	return this->data;
}

void Lump::setName(std::string&& name) {
	this->name = std::move(name);
}

void Lump::setData(std::string&& data) {
	this->data = std::move(data);
}

void Lump::setData(std::vector<char>&& data) {
	this->data = std::string(std::begin(data), std::end(data));
}

size_t Directory::size() {
	return this->index.size();
}

Lump& Directory::at(size_t n) {
	return this->index.at(n);
}

std::vector<Lump>::const_iterator Directory::begin() {
	return this->index.begin();
}

std::vector<Lump>::const_iterator Directory::end() {
	return this->index.end();
}

void Directory::erase_at(size_t index) {
	std::vector<Lump>::iterator it = this->index.begin();
	this->index.erase(it + index);
}

std::tuple<bool, size_t> Directory::find_index(const std::string& name, size_t start) {
	std::vector<Lump>::iterator result = std::find_if(this->index.begin() + start, this->index.end(), [name](Lump lump) {
		return name == lump.getName();
	});
	if (result == this->index.end()) {
		return std::make_tuple(false, 0);
	} else {
		return std::make_tuple(true, result - this->index.begin());
	}
}

void Directory::insert_at(size_t index, Lump&& lump) {
	std::vector<Lump>::iterator it = this->index.begin();
	this->index.insert(it + index, std::move(lump));
}

void Directory::push_back(Lump&& lump) {
	this->index.push_back(std::move(lump));
}

}

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

#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "wad.hh"

const std::string& Lump::getName() {
	return this->name;
}

const std::string& Lump::getData() {
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

void Directory::erase_at(size_t index) {
	std::vector<Lump>::iterator it = this->index.begin();
	this->index.erase(it + index);
}

void Directory::insert_at(size_t index, Lump&& lump) {
	std::vector<Lump>::iterator it = this->index.begin();
	this->index.insert(it + index, std::move(lump));
}

void Directory::push_back(Lump&& lump) {
	this->index.push_back(std::move(lump));
}

Wad::Wad(Wad::Type type) {
	this->type = type;
}

Wad::Wad(std::istream& buffer) {
	char identifier[4];
	int32_t numlumps, infotablefs;

	// [AM] Assume that the current stream position is the first byte
	//      of a WAD file.
	size_t start = buffer.tellg();

	// WAD identifier
	if (!buffer.read(identifier, sizeof(identifier))) {
		throw std::out_of_range("Couldn't read WAD identifier");
	}

	if (std::strncmp(identifier, "IWAD", 4) == 0) {
		this->type = Type::IWAD;
	} else if (std::strncmp(identifier,"PWAD", 4) == 0) {
		this->type = Type::PWAD;
	} else {
		throw std::logic_error("Invalid WAD identifier");
	}

	// Number of lumps
	if (!buffer.read(reinterpret_cast<char*>(&numlumps), sizeof(numlumps))) {
		throw std::out_of_range("Couldn't read number of lumps");
	}

	if (numlumps < 0) {
		std::stringstream error;
		error << "Too many lumps in WAD (found " << numlumps << ", max " << INT32_MAX << ")";
		throw std::out_of_range(error.str());
	}

	// Infotable pointer
	if (!buffer.read(reinterpret_cast<char*>(&infotablefs), sizeof(infotablefs))) {
		throw std::out_of_range("Couldn't read infotable location");
	}

	if (infotablefs < 0 || start > SIZE_MAX - infotablefs) {
		throw std::out_of_range("Position of infotable is out of range");
	}

	if (!buffer.seekg(start + infotablefs)) {
		throw std::out_of_range("Couldn't find infotable");
	}

	for (int32_t i = 0;i < numlumps;i++) {
		// Read a directory entry
		int32_t filepos, size;
		std::vector<char> name(9);

		if (!buffer.read(reinterpret_cast<char*>(&filepos), sizeof(filepos))) {
			std::stringstream error;
			error << "Couldn't read file position of lump " << i;
			throw std::out_of_range(error.str());
		}

		if (!buffer.read(reinterpret_cast<char*>(&size), sizeof(size))) {
			std::stringstream error;
			error << "Couldn't read size of lump " << i;
			throw std::out_of_range(error.str());
		}

		if (!buffer.read(name.data(), 8)) {
			std::stringstream error;
			error << "Couldn't read name of lump " << i;
			throw std::out_of_range(error.str());
		}
		name[8] = '\0';
		std::string namestring(name.data());

		// Create lump
		Lump lump;
		lump.setName(std::move(namestring));

		// If the size is 0, the file position could be complete
		// nonsense, so only attempt to read data if size is not 0.
		if (size > 0) {
			if (filepos < 0 || start > SIZE_MAX - filepos) {
				std::stringstream error;
				error << "Position of lump " << i << " is out of range";
				throw std::out_of_range(error.str());
			}

			size_t info = buffer.tellg();
			std::vector<char> data(size);

			buffer.seekg(start + filepos);
			buffer.read(data.data(), size);
			buffer.seekg(info);

			lump.setData(std::move(data));
		} else if (size < 0) {
			std::stringstream error;
			error << "Size of lump " << i << " is out of range";
			throw std::out_of_range(error.str());
		}

		this->lumps.push_back(std::move(lump));
	}
}

Wad::Type Wad::getType() {
	return this->type;
}

const Directory& Wad::getLumps() {
	return this->lumps;
}

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
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "buffer.hh"
#include "wad.hh"

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

Wad::Wad() : type(Wad::Type::NONE) { }

Wad::Wad(Wad::Type type) : type(type) { }

const Directory& Wad::getLumps() {
	return this->lumps;
}

void Wad::setLumps(Directory& lump) {
	this->lumps = lump;
}

Wad::Type Wad::getType() {
	return this->type;
}

std::istream& operator>>(std::istream& buffer, Wad& wad) {
	// WAD identifier
	std::vector<char> identifier = ReadBuffer(buffer, 4);
	if (std::memcmp(identifier.data(), "IWAD", identifier.size()) == 0) {
		wad.type = Wad::Type::IWAD;
	}
	else if (std::memcmp(identifier.data(), "PWAD", identifier.size()) == 0) {
		wad.type = Wad::Type::PWAD;
	}
	else {
		throw std::logic_error("Invalid WAD identifier");
	}

	// Number of lumps
	int32_t numlumps = ReadInt32LE(buffer);
	if (numlumps < 0) {
		std::stringstream error;
		error << "Too many lumps in WAD (found " << numlumps << ", max " << INT32_MAX << ")";
		throw std::out_of_range(error.str());
	}

	// Infotable pointer
	int32_t infotablefs = ReadInt32LE(buffer);
	if (infotablefs < 0) {
		throw std::out_of_range("Position of infotable is out of range");
	}

	if (!buffer.seekg(infotablefs)) {
		throw std::out_of_range("Couldn't find infotable");
	}

	for (int32_t i = 0; i < numlumps; i++) {
		// Read a directory entry
		int32_t filepos = ReadInt32LE(buffer);
		int32_t size = ReadInt32LE(buffer);

		// Read name
		std::vector<char> name = ReadBuffer(buffer, 8);
		name.push_back(0);
		std::string namestring(name.data());

		// Create lump
		Lump lump;
		lump.setName(std::move(namestring));

		// If the size is 0, the file position could be complete
		// nonsense, so only attempt to read data if size is not 0.
		if (size > 0) {
			if (filepos < 0) {
				std::stringstream error;
				error << "Position of lump " << i << " is out of range";
				throw std::out_of_range(error.str());
			}

			auto info = buffer.tellg();
			buffer.seekg(filepos);
			std::vector<char> data = ReadBuffer(buffer, size);
			buffer.seekg(info);

			lump.setData(std::move(data));
		}
		else if (size < 0) {
			std::stringstream error;
			error << "Size of lump " << i << " is out of range";
			throw std::out_of_range(error.str());
		}

		wad.lumps.push_back(std::move(lump));
	}

	return buffer;
}

std::ostream& operator<<(std::ostream& buffer, Wad& wad) {
	// Write WAD type to buffer
	if (wad.type == Wad::Type::IWAD) {
		buffer << "IWAD";
	} else if (wad.type == Wad::Type::PWAD) {
		buffer << "PWAD";
	} else {
		throw std::runtime_error("Can't write Wad of type NONE");
	}
	
	// Write number of lumps
	if (wad.lumps.size() > std::numeric_limits<int32_t>::max()) {
		throw std::runtime_error("Too many lumps");
	}

	std::stringstream alldata;
	std::stringstream infotable;

	std::for_each(wad.lumps.begin(), wad.lumps.end(), [&alldata, &infotable](const Lump& lump) {
		std::string data = lump.getData();

		// Write lump position
		WriteInt32LE(infotable, alldata.tellp() + static_cast<std::char_traits<char>::pos_type>(12));

		// Write lump data
		alldata.write(data.data(), data.size());

		// Write lump size
		std::string name = lump.getName();
		if (data.size() > std::numeric_limits<int32_t>::max()) {
			throw std::runtime_error("Lump " + name + " is too large");
		}
		WriteInt32LE(infotable, static_cast<int32_t>(data.size()));

		// Write lump name.  If the name is 8 characters, there is no null terminator.
		if (name.size() > 8) {
			throw std::runtime_error("Lump name " + name + " is longer than 8 characters");
		}
		char namebuffer[8] = { 0 };
		std::memmove(namebuffer, name.c_str(), name.size());
		infotable.write(namebuffer, sizeof(namebuffer));
	});

	// Write offset of infotable
	WriteInt32LE(buffer, alldata.tellp() + static_cast<std::char_traits<char>::pos_type>(12));

	// Write data
	if (!(buffer << alldata.rdbuf())) {
		throw std::runtime_error("Couldn't write WAD data");
	}

	// Write infotable
	if (!(buffer << infotable.rdbuf())) {
		throw std::runtime_error("Couldn't write infotable");
	}

	return buffer;
}

}

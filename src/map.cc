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

#include "buffer.hh"
#include "map.hh"

#include <istream>
#include <ostream>

namespace WADmake {

std::istream& operator>>(std::istream& buffer, DoomThing& thing) {
	// X coordinate
	thing.x = ReadInt16LE(buffer);

	// Y coordinate
	thing.y = ReadInt16LE(buffer);

	// Angle
	thing.angle = ReadUInt16LE(buffer);

	// Type
	thing.type = ReadUInt16LE(buffer);

	// Flags
	thing.flags = ReadUInt16LE(buffer);

	return buffer;
}

std::ostream& operator<<(std::ostream& buffer, DoomThing& thing) {
	// X coordinate
	WriteInt16LE(buffer, thing.x);

	// Y coordinate
	WriteInt16LE(buffer, thing.y);

	// Angle
	WriteUInt16LE(buffer, thing.angle);

	// Type
	WriteUInt16LE(buffer, thing.type);

	// Flags
	WriteUInt16LE(buffer, thing.flags.to_ulong());

	return buffer;
}

DoomThings::DoomThings() : nextid(1) { }

DoomThing DoomThings::at(size_t index) {
	auto ptr = this->thingids.at(index).lock();
	if (ptr) {
		return *ptr;
	} else {
		throw std::runtime_error("Invalid index");
	}
}

void DoomThings::push_back(DoomThing&& thing) {
	if (this->nextid == 0) {
		throw std::runtime_error("Too many Thing IDs");
	}

	thing.id = this->nextid;
	auto thingptr = std::make_shared<DoomThing>(std::move(thing));
	this->thingids.emplace(std::make_pair(this->nextid, thingptr));
	this->things.push_back(std::move(thingptr));
	this->nextid += 1;
}

void DoomThings::reindex() {
	this->thingids.clear();
	this->nextid = 1;
	for (auto thingptr : this->things) {
		thingptr->id = this->nextid;
		this->thingids.emplace(std::make_pair(this->nextid, thingptr));
		this->nextid += 1;
	}
}

std::istream& operator>>(std::istream& buffer, DoomThings& things) {
	while (!buffer.eof()) {
		DoomThing thing;
		buffer >> thing;
		things.push_back(std::move(thing));
	}

	return buffer;
}

std::ostream& operator<<(std::ostream& buffer, DoomThings& things) {
	for (auto thing : things.things) {
		buffer << thing;
	}

	return buffer;
}

}

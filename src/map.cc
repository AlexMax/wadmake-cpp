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

template <class T>
IndexedMap<T>::IndexedMap() : nextid(1) { }

template <class T>
T& IndexedMap<T>::at(size_t pos) {
	auto ptr = this->thingids.at(pos).lock();
	if (ptr) {
		return *ptr;
	}
	else {
		throw std::runtime_error("Invalid index");
	}
}

template <class T>
void IndexedMap<T>::push_back(T&& element) {
	if (this->nextid == 0) {
		throw std::runtime_error("Too many Element IDs");
	}

	element.id = this->nextid;
	auto eleptr = std::make_shared<T>(std::move(element));
	this->elementids.emplace(std::make_pair(this->nextid, eleptr));
	this->elements.push_back(std::move(eleptr));
	this->nextid += 1;
}

template <class T>
void IndexedMap<T>::reindex() {
	this->elementids.clear();
	this->nextid = 1;
	for (auto eleptr : this->elements) {
		eleptr->id = this->nextid;
		this->elementids.emplace(std::make_pair(this->nextid, eleptr));
		this->nextid += 1;
	}
}

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

std::istream& operator>>(std::istream& buffer, DoomThings& things) {
	while (!buffer.eof()) {
		DoomThing thing;
		buffer >> thing;
		things.push_back(std::move(thing));
	}

	return buffer;
}

std::ostream& operator<<(std::ostream& buffer, DoomThings& things) {
	for (auto thing : things.elements) {
		buffer << thing;
	}

	return buffer;
}

}

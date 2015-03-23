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

std::istream& Vertex::read(std::istream& buffer) {
	// X coordinate
	this->x = ReadInt16LE(buffer);

	// Y coordinate
	this->y = ReadInt16LE(buffer);

	return buffer;
}

std::ostream& Vertex::write(std::ostream& buffer) {
	// X coordinate
	WriteInt16LE(buffer, this->x);

	// Y coordinate
	WriteInt16LE(buffer, this->y);

	return buffer;
}

std::istream& Vertexes::read(std::istream& buffer) {
	for (;;) {
		buffer.peek(); // Trigger EOF if we're at the end
		if (buffer.eof()) {
			break;
		}
		Vertex vertex;
		vertex.read(buffer);
		this->push_back(std::move(vertex));
	}

	return buffer;
}

std::ostream& Vertexes::write(std::ostream& buffer) {
	for (auto vertex : this->elements) {
		vertex->write(buffer);
	}

	return buffer;
}

std::istream& Sector::read(std::istream& buffer) {
	// Floor height
	this->floor = ReadInt16LE(buffer);

	// Ceiling height
	this->ceiling = ReadInt16LE(buffer);

	// Floor texture
	this->floortex = ReadCString(buffer, 8);

	// Ceiling texture
	this->ceilingtex = ReadCString(buffer, 8);

	// Light level
	this->light = ReadInt16LE(buffer);

	// Sector special
	this->special = ReadInt16LE(buffer);

	// Sector Tag
	this->tag = ReadInt16LE(buffer);

	return buffer;
}

std::ostream& Sector::write(std::ostream& buffer) {
	// Floor height
	WriteInt16LE(buffer, this->floor);

	// Ceiling height
	WriteInt16LE(buffer, this->ceiling);

	// Floor texture
	WriteCString(buffer, this->floortex, 8);

	// Ceiling texture
	WriteCString(buffer, this->ceilingtex, 8);

	// Light level
	WriteInt16LE(buffer, this->light);

	// Sector special
	WriteInt16LE(buffer, this->special);

	// Sector Tag
	WriteInt16LE(buffer, this->tag);

	return buffer;
}

std::istream& Sectors::read(std::istream& buffer) {
	for (;;) {
		buffer.peek(); // Trigger EOF if we're at the end
		if (buffer.eof()) {
			break;
		}
		Sector sector;
		sector.read(buffer);
		this->push_back(std::move(sector));
	}

	return buffer;
}

std::ostream& Sectors::write(std::ostream& buffer) {
	for (auto sector : this->elements) {
		sector->write(buffer);
	}

	return buffer;
}

std::istream& Sidedef::read(std::istream& buffer, Sectors& sectors) {
	// X Texture Offset
	this->xoffset = ReadInt16LE(buffer);

	// Y Texture Offset
	this->yoffset = ReadInt16LE(buffer);

	// Upper Texture
	this->uppertex = ReadCString(buffer, 8);

	// Middle Texture
	this->middletex = ReadCString(buffer, 8);

	// Lower Texture
	this->lowertex = ReadCString(buffer, 8);

	// Sector
	int16_t sectorid = ReadInt16LE(buffer);
	this->sector = sectors.lock(sectorid);

	return buffer;
}

std::ostream& Sidedef::write(std::ostream& buffer) {
	// X Texture Offset
	WriteInt16LE(buffer, this->xoffset);

	// Y Texture Offset
	WriteInt16LE(buffer, this->yoffset);

	// Upper Texture
	WriteCString(buffer, this->uppertex, 8);

	// Middle Texture
	WriteCString(buffer, this->middletex, 8);

	// Lower Texture
	WriteCString(buffer, this->lowertex, 8);

	// Sector
	WriteInt16LE(buffer, this->sector.lock()->id);

	return buffer;
}

std::istream& Sidedefs::read(std::istream& buffer, Sectors& sectors) {
	for (;;) {
		buffer.peek(); // Trigger EOF if we're at the end
		if (buffer.eof()) {
			break;
		}
		Sidedef sidedef;
		sidedef.read(buffer, sectors);
		this->push_back(std::move(sidedef));
	}

	return buffer;
}

std::ostream& Sidedefs::write(std::ostream& buffer) {
	for (auto sidedef : this->elements) {
		sidedef->write(buffer);
	}

	return buffer;
}

std::istream& DoomLinedef::read(std::istream& buffer, Vertexes& vertexes, Sidedefs& sidedefs) {
	// Start vertex
	int16_t startvertexid = ReadInt16LE(buffer);
	this->startvertex = vertexes.lock(startvertexid);

	// End vertex
	int16_t endvertexid = ReadInt16LE(buffer);
	this->endvertex = vertexes.lock(endvertexid);

	// Flags
	this->flags = ReadUInt16LE(buffer);

	// Line special
	this->special = ReadInt16LE(buffer);

	// Line tag
	this->tag = ReadInt16LE(buffer);

	// Front sidedef
	int16_t frontsidedefid = ReadInt16LE(buffer);
	if (frontsidedefid != -1) {
		this->frontsidedef = sidedefs.lock(frontsidedefid);
	}

	// Back sidedef
	int16_t backsidedefid = ReadInt16LE(buffer);
	if (backsidedefid != -1) {
		this->backsidedef = sidedefs.lock(backsidedefid);
	}

	return buffer;
}

std::ostream& DoomLinedef::write(std::ostream& buffer) {
	// Start vertex
	auto startvertex = this->startvertex.lock();
	if (startvertex) {
		WriteInt16LE(buffer, startvertex->id);
	} else {
		throw std::runtime_error("Linedef is missing start vertex");
	}

	// End vertex
	auto endvertex = this->endvertex.lock();
	if (endvertex) {
		WriteInt16LE(buffer, endvertex->id);
	} else {
		throw std::runtime_error("Linedef is missing end vertex");
	}

	// Flags
	WriteUInt16LE(buffer, this->flags.to_ulong());

	// Line special
	WriteInt16LE(buffer, this->special);

	// Line tag
	WriteInt16LE(buffer, this->tag);

	// Front sidedef
	auto frontsidedef = this->frontsidedef.lock();
	if (frontsidedef) {
		WriteInt16LE(buffer, frontsidedef->id);
	} else {
		WriteInt16LE(buffer, -1);
	}

	// Back sidedef
	auto backsidedef = this->backsidedef.lock();
	if (backsidedef) {
		WriteInt16LE(buffer, backsidedef->id);
	} else {
		WriteInt16LE(buffer, -1);
	}

	return buffer;
}

std::istream& DoomLinedefs::read(std::istream& buffer, Vertexes& vertexes, Sidedefs& sidedefs) {
	for (;;) {
		buffer.peek(); // Trigger EOF if we're at the end
		if (buffer.eof()) {
			break;
		}
		DoomLinedef linedef;
		linedef.read(buffer, vertexes, sidedefs);
		this->push_back(std::move(linedef));
	}

	return buffer;
}

std::ostream& DoomLinedefs::write(std::ostream& buffer) {
	for (auto linedef : this->elements) {
		linedef->write(buffer);
	}

	return buffer;
}

std::istream& DoomThing::read(std::istream& buffer) {
	// X coordinate
	this->x = ReadInt16LE(buffer);

	// Y coordinate
	this->y = ReadInt16LE(buffer);

	// Angle
	this->angle = ReadUInt16LE(buffer);

	// Type
	this->type = ReadUInt16LE(buffer);

	// Flags
	this->flags = ReadUInt16LE(buffer);

	return buffer;
}

std::ostream& DoomThing::write(std::ostream& buffer) {
	// X coordinate
	WriteInt16LE(buffer, this->x);

	// Y coordinate
	WriteInt16LE(buffer, this->y);

	// Angle
	WriteUInt16LE(buffer, this->angle);

	// Type
	WriteUInt16LE(buffer, this->type);

	// Flags
	WriteUInt16LE(buffer, this->flags.to_ulong());

	return buffer;
}

std::istream& DoomThings::read(std::istream& buffer) {
	for (;;) {
		buffer.peek(); // Trigger EOF if we're at the end
		if (buffer.eof()) {
			break;
		}
		DoomThing thing;
		thing.read(buffer);
		this->push_back(std::move(thing));
	}

	return buffer;
}

std::ostream& DoomThings::write(std::ostream& buffer) {
	for (auto thing : this->elements) {
		thing->write(buffer);
	}

	return buffer;
}

std::string& DoomMap::getBlockmap() {
	return this->blockmap;
}

DoomLinedefs& DoomMap::getLinedefs() {
	return this->linedefs;
}

std::string& DoomMap::getNodes() {
	return this->nodes;
}

std::string& DoomMap::getReject() {
	return this->reject;
}

Sectors& DoomMap::getSectors() {
	return this->sectors;
}

std::string& DoomMap::getSegs() {
	return this->segs;
}

std::string& DoomMap::getSsectors() {
	return this->ssectors;
}

Sidedefs& DoomMap::getSidedefs() {
	return this->sidedefs;
}

DoomThings& DoomMap::getThings() {
	return this->things;
}

Vertexes& DoomMap::getVertexes() {
	return this->vertexes;
}

void DoomMap::setBlockmap(std::string&& blockmap) {
	this->blockmap = std::move(blockmap);
}

void DoomMap::setLinedefs(DoomLinedefs&& linedefs) {
	this->linedefs = std::move(linedefs);
}

void DoomMap::setNodes(std::string&& nodes) {
	this->nodes = std::move(nodes);
}

void DoomMap::setReject(std::string&& reject) {
	this->reject = std::move(reject);
}

void DoomMap::setSegs(std::string&& segs) {
	this->segs = std::move(segs);
}

void DoomMap::setSectors(Sectors&& sectors) {
	this->sectors = std::move(sectors);
}

void DoomMap::setSsectors(std::string&& ssectors) {
	this->ssectors = std::move(ssectors);
}

void DoomMap::setSidedefs(Sidedefs&& sidedefs) {
	this->sidedefs = std::move(sidedefs);
}

void DoomMap::setThings(DoomThings&& things) {
	this->things = std::move(things);
}

void DoomMap::setVertexes(Vertexes&& vertexes) {
	this->vertexes = std::move(vertexes);
}

}

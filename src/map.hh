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

#ifndef MAP_HH
#define MAP_HH

#include <bitset>
#include <memory>
#include <vector>

#include "indexedmap.hh"

namespace WADmake {

struct Vertex {
	size_t id;
	int16_t x;
	int16_t y;
	std::istream& read(std::istream& buffer);
	std::ostream& write(std::ostream& buffer);
};

class Vertexes : public IndexedMap<Vertex> {
public:
	std::istream& read(std::istream& buffer);
	std::ostream& write(std::ostream& buffer);
};

struct Sector {
	size_t id;
	int16_t floor;
	int16_t ceiling;
	std::string floortex;
	std::string ceilingtex;
	int16_t light;
	int16_t special;
	int16_t tag;
	std::istream& read(std::istream& buffer);
	std::ostream& write(std::ostream& buffer);
};

class Sectors : public IndexedMap<Sector> {
public:
	std::istream& read(std::istream& buffer);
	std::ostream& write(std::ostream& buffer);
};

struct Sidedef {
	size_t id;
	int16_t xoffset;
	int16_t yoffset;
	std::string uppertex;
	std::string middletex;
	std::string lowertex;
	std::weak_ptr<Sector> sector;
	std::istream& read(std::istream& buffer, Sectors& sectors);
	std::ostream& write(std::ostream& buffer);
};

class Sidedefs : public IndexedMap<Sidedef> {
public:
	std::istream& read(std::istream& buffer, Sectors& sectors);
	std::ostream& write(std::ostream& buffer);
};

struct DoomLinedef {
	size_t id;
	std::weak_ptr<Vertex> startvertex;
	std::weak_ptr<Vertex> endvertex;
	std::bitset<sizeof(uint16_t)> flags;
	int16_t special;
	int16_t tag;
	std::weak_ptr<Sidedef> frontsidedef;
	std::weak_ptr<Sidedef> backsidedef;
	std::istream& read(std::istream& buffer, Vertexes& vertexes, Sidedefs& sidedefs);
	std::ostream& write(std::ostream& buffer);
};

class DoomLinedefs : public IndexedMap<DoomLinedef> {
public:
	std::istream& read(std::istream& buffer, Vertexes& vertexes, Sidedefs& sidedefs);
	std::ostream& write(std::ostream& buffer);
};

struct DoomThing {
	size_t id;
	int16_t x;
	int16_t y;
	uint16_t angle;
	uint16_t type;
	std::bitset<16> flags;
	std::istream& read(std::istream& buffer);
	std::ostream& write(std::ostream& buffer);
};

class DoomThings : public IndexedMap<DoomThing> {
public:
	std::istream& read(std::istream& buffer);
	std::ostream& write(std::ostream& buffer);
};

class DoomMap {
	std::string blockmap;
	DoomLinedefs linedefs;
	std::string nodes;
	std::string reject;
	std::string segs;
	Sectors sectors;
	std::string ssectors;
	Sidedefs sidedefs;
	DoomThings things;
	Vertexes vertexes;
public:
	std::string& getBlockmap();
	DoomLinedefs& getLinedefs();
	std::string& getNodes();
	std::string& getReject();
	std::string& getSegs();
	Sectors& getSectors();
	std::string& getSsectors();
	Sidedefs& getSidedefs();
	DoomThings& getThings();
	Vertexes& getVertexes();
	void setBlockmap(std::string&& blockmap);
	void setLinedefs(DoomLinedefs&& linedefs);
	void setNodes(std::string&& nodes);
	void setReject(std::string&& reject);
	void setSegs(std::string&& segs);
	void setSectors(Sectors&& sectors);
	void setSsectors(std::string&& ssectors);
	void setSidedefs(Sidedefs&& sidedefs);
	void setThings(DoomThings&& things);
	void setVertexes(Vertexes&& vertexes);
};

}

#endif

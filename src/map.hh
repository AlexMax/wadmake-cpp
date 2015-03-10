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
	int16_t x;
	int16_t y;
};
typedef std::vector<std::shared_ptr<Vertex>> Vertexes;

struct Sector {
	int16_t floor;
	int16_t ceiling;
	std::string floortex;
	std::string ceilingtex;
	int16_t light;
	int16_t special;
	int16_t tag;
};
typedef std::vector<std::shared_ptr<Sector>> Sectors;

struct Sidedef {
	int16_t xoffset;
	int16_t yoffset;
	std::string uppertex;
	std::string middletex;
	std::string lowertex;
	std::weak_ptr<Sector> sector;
};
typedef std::vector<std::shared_ptr<Sidedef>> Sidedefs;

struct DoomLinedef {
	std::weak_ptr<Vertex> startvertex;
	std::weak_ptr<Vertex> endvertex;
	std::bitset<sizeof(uint16_t)> flags;
	int16_t special;
	int16_t tag;
	std::weak_ptr<Vertex> front;
	std::weak_ptr<Vertex> back;
};
typedef std::vector<std::shared_ptr<DoomLinedef>> DoomLinedefs;

struct DoomThing {
	size_t id;
	int16_t x;
	int16_t y;
	uint16_t angle;
	uint16_t type;
	std::bitset<sizeof(uint16_t)> flags;
	friend std::istream& operator>>(std::istream& buffer, DoomThing& thing);
	friend std::ostream& operator<<(std::ostream& buffer, DoomThing& thing);
};

class DoomThings : public IndexedMap<DoomThing> {
public:
	friend std::istream& operator>>(std::istream& buffer, DoomThings& things);
	friend std::ostream& operator<<(std::ostream& buffer, DoomThings& things);
};

class DoomMap {
	DoomThings things;
	DoomLinedefs linedefs;
	Sidedefs sidedefs;
	Sectors sectors;
	Vertexes vertexes;
public:
	DoomThings& getThings();
	void setThings(DoomThings&& things);
};

}

#endif

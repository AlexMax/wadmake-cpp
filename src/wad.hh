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

#include <iosfwd>
#include <memory>

#include "directory.hh"

namespace WADmake {

class Wad {
public:
	enum class Type { NONE, IWAD, PWAD };
	Wad();
	Wad(Wad::Type type);
	std::shared_ptr<Directory> getLumps();
	Wad::Type getType();
	void setLumps(const std::shared_ptr<Directory>& lumps);
	void setLumps(Directory&& lumps);
	friend std::istream& operator>>(std::istream& buffer, Wad& wad);
	friend std::ostream& operator<<(std::ostream& buffer, Wad& wad);
private:
	Type type;
	std::shared_ptr<Directory> lumps;
};

}

#endif

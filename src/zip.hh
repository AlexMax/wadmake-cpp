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

#ifndef ZIP_HH
#define ZIP_HH

#include <cstdint>
#include <iosfwd>
#include <memory>

#include "wad.hh"

namespace WADmake {

class Zip {
	static const char localFileHeader[];
	static const char centralDirectoryHeader[];
	static const char endOfCentralDirectoryHeader[];
	static const uint16_t version;
	enum compression : uint16_t { STORE = 0, DEFLATE = 8 };

	size_t filesize;
	std::shared_ptr<Directory> lumps;
	void parseLocalFile(std::istream& buffer);
	void parseCentralDirectory(std::istream& buffer);
	void parseEndCentralDirectory(std::istream& buffer);
public:
	Zip();
	std::shared_ptr<Directory> getLumps();
	void setLumps(const std::shared_ptr<Directory>& lumps);
	void setLumps(Directory&& lumps);
	friend std::istream& operator>>(std::istream& buffer, Zip& zip);
	friend std::ostream& operator<<(std::ostream& buffer, Zip& zip);
};

}

#endif

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

#define CATCH_CONFIG_MAIN
#include "catch.hh"

#include "wad.hh"

TEST_CASE("Wad can construct from buffer", "[wad]") {
	std::stringstream buffer;
	std::ifstream moo2d_wad("moo2d.wad", std::fstream::in | std::fstream::binary);
	Wad moo2d(moo2d_wad);
	Directory dir = moo2d.getLumps();
	REQUIRE(dir.size() == 11);
}

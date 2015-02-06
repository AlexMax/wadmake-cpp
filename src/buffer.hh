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

#ifndef BUFFER_HH
#define BUFFER_HH

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

std::string ReadString(std::istream& buffer, size_t len);
std::vector<char> ReadBuffer(std::istream& buffer, size_t len);
uint8_t ReadUInt8(std::istream& buffer);
uint16_t ReadUInt16LE(std::istream& buffer);
uint32_t ReadUInt32LE(std::istream& buffer);
uint64_t ReadUInt64LE(std::istream& buffer);

#endif

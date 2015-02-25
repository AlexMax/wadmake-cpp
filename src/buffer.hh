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
#include <string>
#include <vector>

namespace WADmake {

std::string ReadString(std::istream& buffer, size_t len);
void WriteString(std::ostream& buffer, const std::string& str);
std::vector<char> ReadBuffer(std::istream& buffer, size_t len);
int8_t ReadInt8(std::istream& buffer);
void WriteInt8(std::ostream& buffer, int8_t data);
uint8_t ReadUInt8(std::istream& buffer);
void WriteUInt8(std::ostream& buffer, uint8_t data);
int16_t ReadInt16LE(std::istream& buffer);
void WriteInt16LE(std::ostream& buffer, int16_t data);
uint16_t ReadUInt16LE(std::istream& buffer);
void WriteUInt16LE(std::ostream& buffer, uint16_t data);
int32_t ReadInt32LE(std::istream& buffer);
void WriteInt32LE(std::ostream& buffer, int32_t data);
uint32_t ReadUInt32LE(std::istream& buffer);
void WriteUInt32LE(std::ostream& buffer, uint32_t data);
int64_t ReadInt64LE(std::istream& buffer);
void WriteInt64LE(std::ostream& buffer, int64_t data);
uint64_t ReadUInt64LE(std::istream& buffer);
void WriteUInt64LE(std::ostream& buffer, uint64_t data);

}

#endif

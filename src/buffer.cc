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

#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace WADmake {

std::string ReadString(std::istream& buffer, size_t len) {
	if (len == 0) {
		return std::string();
	}

	std::vector<char> result(len);
	if (!buffer.read(result.data(), len)) {
		std::stringstream err;
		err << "Couldn't read " << len << " bytes from stream";
		throw std::runtime_error(err.str());
	}

	return std::string(result.begin(), result.end());
}

std::vector<char> ReadBuffer(std::istream& buffer, size_t len) {
	if (len == 0) {
		return std::vector<char>();
	}

	std::vector<char> result(len);
	if (!buffer.read(result.data(), len)) {
		std::stringstream err;
		err << "Couldn't read " << len << " bytes from stream";
		throw std::runtime_error(err.str());
	}

	return result;
}

int8_t ReadInt8(std::istream& buffer) {
	int8_t result;
	if (!buffer.read(reinterpret_cast<char*>(&result), sizeof(result))) {
		throw std::runtime_error("Couldn't read int8_t from stream");
	}
	return result;
}

void WriteInt8(std::ostream& buffer, int8_t data) {
	if (!buffer.write(reinterpret_cast<char*>(data), sizeof(data))) {
		throw std::runtime_error("Couldn't write int8_t to stream");
	}
}

uint8_t ReadUInt8(std::istream& buffer) {
	uint8_t result;
	if (!buffer.read(reinterpret_cast<char*>(&result), sizeof(result))) {
		throw std::runtime_error("Couldn't read uint8_t from stream");
	}
	return result;
}

void WriteUInt8(std::ostream& buffer, uint8_t data) {
	if (!buffer.write(reinterpret_cast<char*>(data), sizeof(data))) {
		throw std::runtime_error("Couldn't write uint8_t to stream");
	}
}

int16_t ReadInt16LE(std::istream& buffer) {
	uint8_t raw[sizeof(int16_t)];
	if (!buffer.read(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't read int16_t from stream");
	}
	int16_t result = (raw[0] << 0) | (raw[1] << 8);
	return result;
}

void WriteInt16LE(std::ostream& buffer, int16_t data) {
	uint8_t raw[sizeof(int16_t)];
	raw[0] = static_cast<uint8_t>(data & 0x00FF);
	raw[1] = static_cast<uint8_t>(data >> 8);
	if (!buffer.write(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't write int16_t to stream");
	}
}

uint16_t ReadUInt16LE(std::istream& buffer) {
	uint8_t raw[sizeof(uint16_t)];
	if (!buffer.read(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't read uint16_t from stream");
	}
	uint16_t result = (raw[0] << 0) | (raw[1] << 8);
	return result;
}

void WriteUInt16LE(std::ostream& buffer, uint16_t data) {
	uint8_t raw[sizeof(uint16_t)];
	raw[0] = static_cast<uint8_t>(data & 0x00FF);
	raw[1] = static_cast<uint8_t>(data >> 8);
	if (!buffer.write(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't write uint16_t to stream");
	}
}

int32_t ReadInt32LE(std::istream& buffer) {
	uint8_t raw[sizeof(int32_t)];
	if (!buffer.read(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't read int32_t from stream");
	}
	int32_t result = (raw[0] << 0) | (raw[1] << 8) | (raw[2] << 16) | (raw[3] << 24);
	return result;
}

void WriteInt32LE(std::ostream& buffer, int32_t data) {
	uint8_t raw[sizeof(int32_t)];
	raw[0] = static_cast<uint8_t>(data & 0x000000FF);
	raw[1] = static_cast<uint8_t>((data & 0x0000FF00) >> 8);
	raw[2] = static_cast<uint8_t>((data & 0x00FF0000) >> 16);
	raw[3] = static_cast<uint8_t>(data >> 24);
	if (!buffer.write(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't write int32_t to stream");
	}
}

uint32_t ReadUInt32LE(std::istream& buffer) {
	uint8_t raw[sizeof(uint32_t)];
	if (!buffer.read(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't read uint32_t from stream");
	}
	uint32_t result = (raw[0] << 0) | (raw[1] << 8) | (raw[2] << 16) | (raw[3] << 24);
	return result;
}

void WriteUInt32LE(std::ostream& buffer, uint32_t data) {
	uint8_t raw[sizeof(uint32_t)];
	raw[0] = static_cast<uint8_t>(data & 0x000000FF);
	raw[1] = static_cast<uint8_t>((data & 0x0000FF00) >> 8);
	raw[2] = static_cast<uint8_t>((data & 0x00FF0000) >> 16);
	raw[3] = static_cast<uint8_t>(data >> 24);
	if (!buffer.write(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't write uint32_t to stream");
	}
}

int64_t ReadInt64LE(std::istream& buffer) {
	uint8_t raw[sizeof(int64_t)];
	if (!buffer.read(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't read int64_t from stream");
	}
	int64_t result = (static_cast<int64_t>(raw[0]) << 0) |
	                 (static_cast<int64_t>(raw[1]) << 8) |
	                 (static_cast<int64_t>(raw[2]) << 16) |
	                 (static_cast<int64_t>(raw[3]) << 24) |
	                 (static_cast<int64_t>(raw[4]) << 32) |
	                 (static_cast<int64_t>(raw[5]) << 40) |
	                 (static_cast<int64_t>(raw[6]) << 48) |
	                 (static_cast<int64_t>(raw[7]) << 56);
	return result;
}

void WriteInt64LE(std::ostream& buffer, int64_t data) {
	uint8_t raw[sizeof(int64_t)];
	raw[0] = static_cast<uint8_t>(data & 0x00000000000000FF);
	raw[1] = static_cast<uint8_t>((data & 0x000000000000FF00) >> 8);
	raw[2] = static_cast<uint8_t>((data & 0x0000000000FF0000) >> 16);
	raw[3] = static_cast<uint8_t>((data & 0x00000000FF000000) >> 24);
	raw[4] = static_cast<uint8_t>((data & 0x000000FF00000000) >> 32);
	raw[5] = static_cast<uint8_t>((data & 0x0000FF0000000000) >> 40);
	raw[6] = static_cast<uint8_t>((data & 0x00FF000000000000) >> 48);
	raw[7] = static_cast<uint8_t>(data >> 56);
	if (!buffer.write(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't write int64_t to stream");
	}
}

uint64_t ReadUInt64LE(std::istream& buffer) {
	uint8_t raw[sizeof(uint64_t)];
	if (!buffer.read(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't read uint64_t from stream");
	}
	uint64_t result = (static_cast<uint64_t>(raw[0]) << 0) |
	                  (static_cast<uint64_t>(raw[1]) << 8) |
	                  (static_cast<uint64_t>(raw[2]) << 16) |
	                  (static_cast<uint64_t>(raw[3]) << 24) |
	                  (static_cast<uint64_t>(raw[4]) << 32) |
	                  (static_cast<uint64_t>(raw[5]) << 40) |
	                  (static_cast<uint64_t>(raw[6]) << 48) |
	                  (static_cast<uint64_t>(raw[7]) << 56);
	return result;
}

void WriteUInt64LE(std::ostream& buffer, uint64_t data) {
	uint8_t raw[sizeof(uint64_t)];
	raw[0] = static_cast<uint8_t>(data & 0x00000000000000FF);
	raw[1] = static_cast<uint8_t>((data & 0x000000000000FF00) >> 8);
	raw[2] = static_cast<uint8_t>((data & 0x0000000000FF0000) >> 16);
	raw[3] = static_cast<uint8_t>((data & 0x00000000FF000000) >> 24);
	raw[4] = static_cast<uint8_t>((data & 0x000000FF00000000) >> 32);
	raw[5] = static_cast<uint8_t>((data & 0x0000FF0000000000) >> 40);
	raw[6] = static_cast<uint8_t>((data & 0x00FF000000000000) >> 48);
	raw[7] = static_cast<uint8_t>(data >> 56);
	if (!buffer.write(reinterpret_cast<char*>(raw), sizeof(raw))) {
		throw std::runtime_error("Couldn't write uint64_t to stream");
	}
}

}

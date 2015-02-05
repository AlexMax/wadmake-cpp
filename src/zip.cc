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

#include <cstring>
#include <new>
#include <sstream>
#include <stdexcept>

#include <zlib.h>

#include "zip.hh"

static std::string zlibInflate(std::istream& buffer, size_t in_len, size_t out_len) {
	z_stream strm;

	// Initialize inflate state
	strm.next_in = Z_NULL;
	strm.avail_in = 0;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;

	int success = inflateInit2(&strm, -MAX_WBITS);
	switch (success) {
		case Z_OK:
			break; // Do nothing
		case Z_MEM_ERROR:
			throw std::bad_alloc();
		case Z_VERSION_ERROR:
			throw std::logic_error("Incompatible zlib version");
		case Z_STREAM_ERROR:
			throw std::logic_error(strm.msg);
		default:
			break; // Do nothing
	}

	// Input buffer
	std::vector<char> data_in(in_len);
	if (!buffer.read(data_in.data(), data_in.size())) {
		throw std::out_of_range("Couldn't read buffer to infliate");
	}

	// Output buffer
	std::vector<char> data_out(out_len);
	std::stringstream output;

	strm.avail_in = data_in.size();
	strm.next_in = reinterpret_cast<Bytef*>(data_in.data());
	strm.avail_out = data_out.size();
	strm.next_out = reinterpret_cast<Bytef*>(data_out.data());

	for (;;) {
		// Read a little bit out of the buffer
		success = inflate(&strm, Z_SYNC_FLUSH);
		switch (success) {
			case Z_OK:
			case Z_STREAM_END:
				break;
			case Z_NEED_DICT:
				throw std::runtime_error("Preset dictionary required");
			case Z_DATA_ERROR:
				throw std::runtime_error("Buffer corrupted");
			case Z_STREAM_ERROR:
				throw std::runtime_error("Stream state corrupted");
			case Z_MEM_ERROR:
				throw std::bad_alloc();
			case Z_BUF_ERROR:
				throw std::runtime_error("Inflation progress impossible");
			default:
				throw std::runtime_error(strm.msg);
		}

		if (success == Z_STREAM_END) {
			// We're done!
			output.write(data_out.data(), data_out.size() - strm.avail_out);
			break;
		} else {
			// More stuff to process.
			output.write(data_out.data(), data_out.size());
			strm.avail_out = data_out.size();
			strm.next_out = reinterpret_cast<Bytef*>(data_out.data());
		}
	}

	inflateEnd(&strm);

	return std::string(output.str());
}

const Directory& Zip::getLumps() {
	return this->lumps;
}

std::istream& operator>>(std::istream& buffer, Zip& zip) {
	char identifier[4];

	while (!buffer.eof()) {
		// Either a local file header or a central directory header
		if (!buffer.read(identifier, sizeof(identifier))) {
			throw std::out_of_range("Couldn't read ZIP header");
		}

		if (std::memcmp(identifier, "PK\x03\x04", sizeof(identifier)) == 0) {
			// Local file
			Lump lump;

			// Version needed to extract
			uint16_t version;
			if (!buffer.read(reinterpret_cast<char*>(&version), sizeof(version))) {
				throw std::out_of_range("Couldn't read file's version requirement");
			}

			// Check bitflags
			uint16_t bitflags;
			if (!buffer.read(reinterpret_cast<char*>(&bitflags), sizeof(bitflags))) {
				throw std::out_of_range("Couldn't read file's bitflags");
			}

			// Check compression method
			Zip::Compression compression;
			if (!buffer.read(reinterpret_cast<char*>(&compression), sizeof(compression))) {
				throw std::out_of_range("Couldn't read file's compression type");
			}

			switch (compression) {
				case Zip::Compression::STORE:
				case Zip::Compression::DEFLATE:
					break; // We can handle
				default:
					throw std::out_of_range("Unsupported file compression method");
			}

			// We don't care about modified date/time
			if (!buffer.seekg(4, buffer.cur)) {
				throw std::out_of_range("Couldn't skip past file date/time");
			}

			// CRC32
			uint32_t crc;
			if (!buffer.read(reinterpret_cast<char*>(&crc), sizeof(crc))) {
				throw std::out_of_range("Couldn't read file's CRC32");
			}

			// Compressed size
			uint32_t compressed_size;
			if (!buffer.read(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size))) {
				throw std::out_of_range("Couldn't read file's compressed size");
			}

			// Uncompressed size
			uint32_t uncompressed_size;
			if (!buffer.read(reinterpret_cast<char*>(&uncompressed_size), sizeof(uncompressed_size))) {
				throw std::out_of_range("Couldn't read file's uncompressed size");
			}

			// Filename length
			uint16_t filename_len;
			if (!buffer.read(reinterpret_cast<char*>(&filename_len), sizeof(filename_len))) {
				throw std::out_of_range("Couldn't read file's filename length");
			}

			// Extra field length
			uint16_t extra_len;
			if (!buffer.read(reinterpret_cast<char*>(&extra_len), sizeof(extra_len))) {
				throw std::out_of_range("Couldn't read file's extra field length");
			}

			// Filename
			std::vector<char> filename(filename_len);
			if (!buffer.read(filename.data(), filename_len)) {
				throw std::out_of_range("Couldn't read file's filename");
			}

			lump.setName(std::string(filename.data(), filename_len));

			// Skip extra field
			if (!buffer.seekg(extra_len, buffer.cur)) {
				throw std::out_of_range("Couldn't skip past file's extra field");
			}

			if (compressed_size > 0) {
				switch (compression) {
					case Zip::Compression::STORE:
						break;
					case Zip::Compression::DEFLATE:
						lump.setData(zlibInflate(buffer, compressed_size, uncompressed_size));
						break;
					default:
						throw std::out_of_range("Compression not implemented");
				}
			}

			zip.lumps.push_back(std::move(lump));
		} else if (std::memcmp(identifier, "\x04\x03KP", sizeof(identifier)) == 0) {
			// Central directory
		} else {
			throw std::logic_error("Invalid ZIP header");
		}
	}

	return buffer;
}

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
#include <limits>
#include <sstream>

#include <zlib.h>

#include "buffer.hh"
#include "directory.hh"
#include "zip.hh"

namespace WADmake {

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

	if (data_in.size() > std::numeric_limits<uInt>::max()) {
		throw std::out_of_range("Input buffer too large");
	}
	strm.avail_in = static_cast<uInt>(data_in.size());
	strm.next_in = reinterpret_cast<Bytef*>(data_in.data());

	if (data_in.size() > std::numeric_limits<uInt>::max()) {
		throw std::out_of_range("Output buffer too large");
	}
	strm.avail_out = static_cast<uInt>(data_out.size());
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
			strm.avail_out = static_cast<uInt>(data_out.size());
			strm.next_out = reinterpret_cast<Bytef*>(data_out.data());
		}
	}

	inflateEnd(&strm);

	return std::string(output.str());
}

// Header for Local File
const char Zip::localFileHeader[] = { 'P', 'K', 0x03, 0x04 };

// Header for Central Directory
const char Zip::centralDirectoryHeader[] = { 'P', 'K', 0x01, 0x02 };

// Header for End of Central Directory
const char Zip::endOfCentralDirectoryHeader[] = { 'P', 'K', 0x05, 0x06 };

// Version of ZIP standards that Zip obeys
const uint16_t Zip::version = 8;

// Parse a local file entry.  Assumes the buffer is set to the location
// of the local file entry's magic number.
void Zip::parseLocalFile(std::istream& buffer) {
	// Identifier
	std::vector<char> identifier = ReadBuffer(buffer, 4);
	if (std::memcmp(identifier.data(), Zip::localFileHeader, identifier.size()) != 0) {
		throw std::runtime_error("Not a valid local file entry");
	}

	// Version needed to extract
	ReadUInt16LE(buffer);

	// General purpose bitflag
	ReadUInt16LE(buffer);

	// Compression method
	Zip::compression compression = static_cast<Zip::compression>(ReadUInt16LE(buffer));
	switch (compression) {
	case Zip::compression::STORE:
	case Zip::compression::DEFLATE:
		break;
	default:
		throw std::runtime_error("Unsupported compression");
	}

	// Last modified file time
	ReadUInt16LE(buffer);

	// Last modified file date
	ReadUInt16LE(buffer);

	// CRC32
	uint32_t crc_expected = ReadUInt32LE(buffer);

	// Compressed size
	uint32_t compressed_size = ReadUInt32LE(buffer);

	// Uncompressed size
	uint32_t uncompressed_size = ReadUInt32LE(buffer);

	// Filename length
	uint16_t filename_len = ReadUInt16LE(buffer);

	// Extra field length
	uint16_t extra_len = ReadUInt16LE(buffer);

	// Filename
	std::string filename = ReadString(buffer, filename_len);

	// Extra field
	ReadBuffer(buffer, extra_len);

	// File data
	Lump lump;
	lump.setName(std::move(filename));

	if (compressed_size > 0) {
		switch (compression) {
		case Zip::compression::STORE:
			lump.setData(ReadBuffer(buffer, compressed_size));
			break;
		case Zip::compression::DEFLATE:
			lump.setData(zlibInflate(buffer, compressed_size, uncompressed_size));
			break;
		default:
			throw std::runtime_error("Unsupported compression");
		}
	}

	// Check the CRC32 sum.
	std::string data = lump.getData();
	if (data.size() > std::numeric_limits<uInt>::max()) {
		throw std::runtime_error("File is too big for CRC check");
	}
	uint32_t crc_actual = crc32(0, reinterpret_cast<const Bytef*>(data.data()), static_cast<uInt>(data.size()));
	if (crc_expected != crc_actual) {
		throw std::runtime_error("CRC check failed");
	}

	this->lumps->push_back(std::move(lump));
}

// Parse a Central Directory entry.  Assumes the buffer is set to the
// location of the Central Directory entry's magic number.
void Zip::parseCentralDirectory(std::istream& buffer) {
	// Identifier
	std::vector<char> identifier = ReadBuffer(buffer, 4);
	if (std::memcmp(identifier.data(), Zip::centralDirectoryHeader, identifier.size()) != 0) {
		throw std::runtime_error("Not a valid central directory entry");
	}

	// Version made by
	ReadUInt16LE(buffer);

	// Version needed to extract
	ReadUInt16LE(buffer);

	// General purpose bitflag
	ReadUInt16LE(buffer);

	// Compression method
	ReadUInt16LE(buffer);

	// Last modified file time
	ReadUInt16LE(buffer);

	// Last modified file date
	ReadUInt16LE(buffer);

	// CRC32
	ReadUInt32LE(buffer);

	// Compressed size
	ReadUInt32LE(buffer);

	// Uncompressed size
	ReadUInt32LE(buffer);

	// Filename length
	uint16_t filename_len = ReadUInt16LE(buffer);

	// Extra field length
	uint16_t extra_len = ReadUInt16LE(buffer);

	// File comment length
	uint16_t comment_len = ReadUInt16LE(buffer);

	// Disk number start
	if (ReadUInt16LE(buffer) != 0) {
		throw std::runtime_error("Multi-part ZIP files are not supported");
	}

	// Internal file attibutes
	ReadUInt16LE(buffer);

	// External file attibutes
	ReadUInt32LE(buffer);

	// Relative offset of local file header
	uint32_t offset = ReadUInt32LE(buffer);
	if (offset > this->filesize) {
		throw std::runtime_error("Invalid local file header offset");
	}

	// Filename
	ReadString(buffer, filename_len);

	// Extra field
	ReadBuffer(buffer, extra_len);

	// Comment
	ReadBuffer(buffer, comment_len);

	// We've parsed a directory entry, but we still need to parse the
	// actual file itself.
	auto save = buffer.tellg();
	buffer.seekg(offset);
	this->parseLocalFile(buffer);
	buffer.seekg(save);
}

// Parse the End of Central Directory header in a ZIP file, assuming we
// have already parsed the magic number.
void Zip::parseEndCentralDirectory(std::istream& buffer) {
	// Disk number
	if (ReadUInt16LE(buffer) != 0) {
		throw std::runtime_error("Multi-part ZIP files are not supported");
	}

	// Disk number of central directory
	if (ReadUInt16LE(buffer) != 0) {
		throw std::runtime_error("Multi-part ZIP files are not supported");
	}

	// Central directory entries
	uint16_t cdentries = ReadUInt16LE(buffer);

	// Total number of central directory entries
	uint16_t total_cdentries = ReadUInt16LE(buffer);
	if (cdentries != total_cdentries) {
		throw std::runtime_error("Central directory entry count does not equal total");
	}

	// Size of the central directory
	ReadUInt32LE(buffer);

	// Offset of central directory
	uint32_t cdoffset = ReadUInt32LE(buffer);
	if (cdoffset > this->filesize) {
		throw std::runtime_error("Invalid central directory offset");
	}

	// Read every entry in the central directory
	buffer.seekg(cdoffset);
	for (size_t index = 0;index < cdentries;index++) {
		this->parseCentralDirectory(buffer);
	}
}

Zip::Zip() : filesize(0), lumps(new Directory) { }

std::shared_ptr<Directory> Zip::getLumps() {
	return this->lumps;
}

std::istream& operator>>(std::istream& buffer, Zip& zip) {
	// Ensure our buffer is big enough to be a ZIP file
	buffer.seekg(0, buffer.end);
	zip.filesize = buffer.tellg();
	if (zip.filesize < 22) {
		throw std::runtime_error("Buffer is not ZIP file - too small");
	}

	// Start at the end and work backwards until we find an end of
	// central directory record.  22 is the closest the End of Central
	// Directory header could possibly be to the end.
	buffer.seekg(-22, buffer.end);

	for (;;) {
		std::vector<char> identifier = ReadBuffer(buffer, 4);
		if (std::memcmp(identifier.data(), Zip::endOfCentralDirectoryHeader, identifier.size()) == 0) {
			zip.parseEndCentralDirectory(buffer);
			break;
		}

		if (buffer.tellg() == static_cast<std::char_traits<char>::pos_type>(0)) {
			throw std::runtime_error("Buffer is not ZIP file - can't find identifier");
		}

		buffer.seekg(-1, buffer.cur);
	}

	return buffer;
}

std::ostream& operator<<(std::ostream& buffer, Zip& zip) {
	std::stringstream localFiles;
	std::stringstream centralDirectory;
	std::stringstream endOfCentralDirectory;

	for (Lump lump : *(zip.lumps)) {
		std::string name = lump.getName();
		std::string data = lump.getData();

		// Store local file position so we can write it later
		auto filepos = localFiles.tellg();

		// Headers
		localFiles.write(Zip::localFileHeader, sizeof(Zip::localFileHeader));
		centralDirectory.write(Zip::centralDirectoryHeader, sizeof(Zip::centralDirectoryHeader));

		// Version made by (only in Central Directory)
		WriteUInt16LE(centralDirectory, Zip::version);

		// Version needed to extract
		WriteUInt16LE(localFiles, Zip::version);
		WriteUInt16LE(centralDirectory, Zip::version);

		// General purpose bitflag
		WriteUInt16LE(localFiles, 0);
		WriteUInt16LE(centralDirectory, 0);

		// Compression method
		WriteUInt16LE(localFiles, Zip::compression::DEFLATE);
		WriteUInt16LE(centralDirectory, Zip::compression::DEFLATE);

		// Last modified file time
		WriteUInt16LE(localFiles, 0);
		WriteUInt16LE(centralDirectory, 0);

		// Last modified file date
		WriteUInt16LE(localFiles, 0);
		WriteUInt16LE(centralDirectory, 0);

		// CRC32
		if (data.size() > std::numeric_limits<uInt>::max()) {
			throw std::runtime_error("Lump is too big for CRC check");
		}
		uint32_t crc = crc32(0, reinterpret_cast<const Bytef*>(data.data()), static_cast<uInt>(data.size()));
		WriteUInt32LE(localFiles, crc);
		WriteUInt32LE(centralDirectory, crc);

		// Compressed size
		WriteUInt32LE(localFiles, 0);
		WriteUInt32LE(centralDirectory, 0);

		// Uncompressed size
		if (data.size() > std::numeric_limits<uint32_t>::max()) {
			throw std::runtime_error("Lump " + name + " is too large");
		}
		WriteUInt32LE(localFiles, static_cast<uint32_t>(data.size()));
		WriteUInt32LE(centralDirectory, static_cast<uint32_t>(data.size()));

		// Filename length
		if (name.size() > std::numeric_limits<uint16_t>::max()) {
			throw std::runtime_error("Lump name " + name + " is too large");
		}
		WriteUInt16LE(localFiles, static_cast<uint16_t>(name.size()));
		WriteUInt16LE(centralDirectory, static_cast<uint16_t>(name.size()));

		// Extra field length
		WriteUInt16LE(localFiles, 0);
		WriteUInt16LE(centralDirectory, 0);

		// File comment length (only in Central Directory)
		WriteUInt16LE(centralDirectory, 0);

		// Disk number (only in Central Directory)
		WriteUInt16LE(centralDirectory, 0);

		// Internal file attibutes (only in Central Directory)
		WriteUInt16LE(centralDirectory, 0);

		// External file attibutes (only in Central Directory)
		WriteUInt32LE(centralDirectory, 0);

		// Local header location (only in Central Directory)
		if (filepos > std::numeric_limits<uint32_t>::max()) {
			throw std::runtime_error("Cannot write local header location of lump " + name);
		}
		WriteUInt32LE(centralDirectory, static_cast<uint32_t>(filepos));

		// Filename
		WriteString(localFiles, name);
		WriteString(centralDirectory, name);

		// Extra field & File comment (skipped)
	}

	return buffer;
}

}

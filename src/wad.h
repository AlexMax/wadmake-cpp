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

#ifndef WAD_H
#define WAD_H

#include <stdint.h>

#include <bstrlib.h>

typedef struct {
	bstring name;
	void*   data;
	int32_t data_size;
} Lump;

typedef struct {
	Lump**  lumps;
    int32_t lumps_size;
} Lumps;

typedef enum {
	WADTYPE_IWAD,
	WADTYPE_PWAD,
	WADTYPE_MAX,
	WADTYPE_NULL
} WadType;

extern const char WadTypeIdentifiers[][4];

typedef struct {
	Lumps*  filesystem;
	WadType type;
} Wad;

extern Error Lumps_Construct(Lumps* lumps);
extern void Lumps_Destruct(Lumps* lumps);
extern Error Wad_Construct(const WadType wad_type, Wad* wad);
extern Error Wad_ConstructFromBuffer(const char* buffer, const size_t size, Wad* wad);
extern void Wad_Destruct(Wad* wad);

#endif

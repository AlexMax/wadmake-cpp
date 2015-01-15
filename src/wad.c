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

#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "wad.h"

const char WadTypeIdentifiers[][4] = {
	{'I', 'W', 'A', 'D'},
	{'P', 'W', 'A', 'D'}
};

Error Lumps_Construct(Lumps* lumps) {
	lumps = malloc(sizeof(Lumps));
	if (lumps == NULL) {
		return ERR_ALLOC;
	}

	lumps->lumps = NULL;
	lumps->lumps_size = 0;

	return SUCCESS;
}

void Lumps_Destruct(Lumps* lumps) {
	free(lumps);
}

Error Wad_Construct(const WadType wad_type, Wad* wad) {
	Error error;

	wad = malloc(sizeof(Wad));
	if (wad == NULL) {
		return ERR_ALLOC;
	}

	wad->type = wad_type;
	if ((error = Lumps_Construct(wad->filesystem)) != SUCCESS) {
		free(wad);
		return error;
	}

	return SUCCESS;
}

Error Wad_ConstructFromBuffer(const char* buffer, const size_t size, Wad* wad) {
	Error error;

	if (size < 12) {
		// Buffer too small
		return ERR_WADPARSE;
	}

	wad = malloc(sizeof(Wad));
	if (wad == NULL) {
		return ERR_ALLOC;
	}

	// Determine if it's an IWAD or a PWAD
	wad->type = WADTYPE_NULL;
	for (int i = 0;i < WADTYPE_MAX;i++) {
		if (memcmp(WadTypeIdentifiers[i], buffer, 4) == 0) {
			wad->type = i;
		}
	}
	if (wad->type == WADTYPE_NULL) {
		// Invalid WAD identifier
		free(wad);
		return ERR_WADPARSE;
	}

	int32_t numlumps;
	memcpy(&numlumps, buffer + 4, 4);
	if (numlumps < 0) {
		// Lump count wraparound
		free(wad);
		return ERR_WADPARSE;
	}

	// Find WAD directory
	size_t infotablefs;
	memcpy(&infotablefs, buffer + 8, 4);
	if (infotablefs + (numlumps * 16) > size) {
		// Incomplete infotable
		free(wad);
		return ERR_WADPARSE;
	}
	const char* infotable = buffer + infotablefs;

	if ((error = Lumps_Construct(wad->filesystem)) != SUCCESS) {
		free(wad);
		return error;
	}

	for (int i = 0;i < numlumps;i++) {
		size_t filepos;
		memcpy(&filepos, infotable + (i * 16), 4);
		size_t size;
		memcpy(&size, infotable + (i * 16) + 4, 4);
		char name[9];
		memcpy(&name, infotable + (i * 16) + 8, 8);
		name[8] = 0;
		printf("name: %s\n", name);
	}

	return SUCCESS;
}

void Wad_Destruct(Wad* wad) {
	Lumps_Destruct(wad->filesystem);
	free(wad);
}

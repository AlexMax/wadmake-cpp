#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "wad.h"

const char WadTypeIdentifiers[][4] = {
	{'I', 'W', 'A', 'D'},
	{'P', 'W', 'A', 'D'}
};

Lumps* Lumps_Construct() {
	Lumps* lumps = malloc(sizeof(Lumps));
	if (lumps == NULL) {
		return NULL;
	}

	lumps->lumps = NULL;	
	lumps->lumps_size = 0;

	return lumps;
}

void Lumps_Destruct(Lumps* lumps) {
	free(lumps);
}

Wad* Wad_Construct(const WadType wad_type) {
	Wad* wad = malloc(sizeof(Wad));
	if (wad == NULL) {
		return NULL;
	}

	wad->type = wad_type;
	wad->filesystem = Lumps_Construct();
	if (wad->filesystem == NULL) {
		free(wad);
		return NULL;
	}

	return wad;
}

Wad* Wad_ConstructFromBuffer(const char* buffer, size_t size) {
	if (size < 12) {
		// Buffer too small
		return NULL;
	}

	Wad* wad = malloc(sizeof(Wad));
	if (wad == NULL) {
		return NULL;
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
		return NULL;
	}

	int32_t numlumps;
	memcpy(&numlumps, buffer + 4, 4);
	if (numlumps < 0) {
		// Lump count wraparound
		free(wad);
		return NULL;
	}

	// Find WAD directory
	size_t infotablefs;
	memcpy(&infotablefs, buffer + 8, 4);
	if (infotablefs + (numlumps * 16) > size) {
		// Incomplete infotable
		free(wad);
		return NULL;
	}
	const char* infotable = buffer + infotablefs;

	wad->filesystem = Lumps_Construct();
	if (wad->filesystem == NULL) {
		free(wad);
		return NULL;
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

	return wad;
}

void Wad_Destruct(Wad* wad) {
	Lumps_Destruct(wad->filesystem);
	free(wad);
}

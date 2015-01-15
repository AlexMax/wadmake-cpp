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

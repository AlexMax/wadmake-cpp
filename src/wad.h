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

extern Lumps* Lumps_Construct();
extern void Lumps_Destruct(Lumps* lumps);
extern Wad* Wad_Construct(const WadType wad_type);
extern Wad* Wad_ConstructFromBuffer(const char* buffer, size_t size);
extern void Wad_Destruct(Wad* wad);

#endif

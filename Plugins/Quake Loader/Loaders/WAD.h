#pragma once

#include <algorithm>
#include <vector>
#include "../Quake.h"
#include "../DLLExports.h"
#include "../VKFormat.h"
#include <stdint.h>

extern Package* LoadPackageWAD(FILE* file);
extern int LoadWADTexture(FILE* file, void* data, uint64_t datasize, const lumpinfo_t& lump, int w, int h);
extern uint64_t GetWADTextureSize(FILE* file, const lumpinfo_t& lump, int w, int h);
#pragma once

#include <algorithm>
#include <vector>
#include "../Quake.h"
#include "../DLLExports.h"
#include "../VKFormat.h"

extern Package* LoadPackageBSP(FILE* file);
//extern Package* LoadPackageBSP(const wchar_t* cpath, void* mem, uint64_t sz);
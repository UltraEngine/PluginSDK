#pragma once

#include <algorithm>
#include <vector>
#include "../Quake.h"
#include "../DLLExports.h"
#include "../VKFormat.h"

extern void* LoadTextureSPR(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
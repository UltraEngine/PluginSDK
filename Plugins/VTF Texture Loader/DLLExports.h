#pragma once
#include <string>
#include "../../Source/Libraries/PluginSDK/GMFSDK.h"
#include "../../Source/Libraries/PluginSDK/TextureInfo.h"
#include "../../Source/Libraries/nlohmann_json/single_include/nlohmann/json.hpp"
#include "VTFLib/VTFLib.h"
#include "../../Source/Libraries/PluginSDK/MemWriter.h"
#include "../../Source/Libraries/PluginSDK/Utilities.h"

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

using namespace GMFSDK;

struct Context
{
	vlUInt mtl;
	bool materialbound;
	MemWriter* writer;
	std::string materialstring;
	Context();
};

extern "C"
{
	/// <summary>
	/// test
	/// </summary>
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport void* LoadTexture(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport Context* CreateContext();
	DLLExport void FreeContext(Context*);
	DLLExport void* LoadMaterial(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out);
	DLLExport void* SaveTexture(Context* context, wchar_t* extension, const int type, const int width, const int height, const int format, void** mipchain, int* sizechain, const int mipcount, const int layers, uint64_t& returnsize, int flags);
}
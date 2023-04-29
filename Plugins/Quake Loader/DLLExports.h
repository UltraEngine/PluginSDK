#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <stdint.h>
#include <direct.h>
#include <wchar.h>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <shobjidl_core.h>
	#include <Shlobj.h>
#endif
#include "../../PluginSDK/PluginSDK.h"
#include "../../PluginSDK/MemReader.h"
#include "../../PluginSDK/MemWriter.h"
#include "../../PluginSDK/Utilities.h"
#include "VKFormat.h"
#include "Quake.h"
#include "Context.h"
#include "Package.h"

// Declare structures
struct PackageFile;
struct Package;
struct Context;

using namespace UltraEngine::PluginSDK;

//----------------------------------------------
// Export functions
//----------------------------------------------

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

extern "C"
{
	//DLLExport void* LoadModel(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out);
	DLLExport int ReadStream(Package* pak, wchar_t* path, void* data, uint64_t sz);
	DLLExport void* LoadPackageTexture(Context* context, Package* pak, wchar_t* cpath, uint64_t& returnsize);
	DLLExport void* LoadTexture(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport Package* LoadPackage(const wchar_t* path);
	DLLExport int ReadStream(Package* pak, wchar_t* path, void* data, uint64_t sz);
	DLLExport void FreePackage(Package* package);
	DLLExport int LoadDir(Package* pak, wchar_t* path, int types);
	DLLExport const wchar_t* GetLoadedFile(Package* pak, int index);
	DLLExport int FileType(Package* pak, wchar_t* path);
	DLLExport uint64_t FileSize(Package* pak, wchar_t* path);
	DLLExport int ListFiles(Package* pak);
	DLLExport int PackageValid(wchar_t* path);
	DLLExport Context* CreateContext();
	DLLExport void FreeContext(Context* ctx);
	DLLExport void* LoadModel(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
}

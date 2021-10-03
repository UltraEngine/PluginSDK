#pragma once
#include <string>
#include <stdint.h>
#include "../../PluginSDK/GMFSDK.h"
#include "../../PluginSDK/Utilities.h"
#include "MDLStructs.h"

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

struct Context
{
	GMFSDK::MemWriter writer;
};

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport void* LoadModel(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport Context* CreateContext();
	DLLExport void FreeContext(Context* context);
	DLLExport void SetFunctionPointer(wchar_t* cname, void* ptr);
}
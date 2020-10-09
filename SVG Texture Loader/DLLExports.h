#pragma once
#include <string>
#include "../SDK/GMFSDK.h"
#include "../SDK/TextureInfo.h"
#include "VKFormat.h"
//NanoSVG
#define NANOSVG_IMPLEMENTATION
#include "nanosvg/src/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/src/nanosvgrast.h"

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

struct Context
{
	GMFSDK::MemWriter* writer;
	
	Context();
	~Context();
};

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport Context* CreateContext();
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport void* LoadTexture(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport void FreeContext(Context*);
}
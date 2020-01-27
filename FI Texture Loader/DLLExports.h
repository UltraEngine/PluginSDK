#pragma once
#include <string>
#include "../SDK/GMFSDK.h"
#include "FreeImage/FreeImage.h"
#include "VKFormat.h"

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport void* LoadTexture(void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport void Cleanup();
}
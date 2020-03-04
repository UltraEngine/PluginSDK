#pragma once
#include <string>
#include "../SDK/GMFSDK.h"
#include "../SDK/TextureInfo.h"
#include "../SDK/MemReader.h"

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport void* LoadTexture(void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport void* SavePixmap(int width, int height, int format, void* pixels, uint64_t size, wchar_t* extension, uint64_t& returnsize);
	DLLExport void Cleanup();
}
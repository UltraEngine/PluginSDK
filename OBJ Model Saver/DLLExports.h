#pragma once
#include <string>
#include "../SDK/GMFSDK.h"

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport void* SaveModel(void* data, uint64_t size, wchar_t* filepath, uint64_t& returnsize, int flags);
	DLLExport void Cleanup();
}
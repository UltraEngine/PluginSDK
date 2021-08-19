#pragma once
#include <string>
#include "../../Source/Libraries/PluginSDK/GMFSDK.h"
#include "../../Source/Libraries/PluginSDK/TextureInfo.h"

#ifdef _WIN32
	#ifdef _WIN64
		#include "FreeImage/Dist/x64/FreeImage.h"
	#else
		#include "FreeImage/Dist/x32/FreeImage.h"
	#endif
#endif
#ifdef __APPLE__
    #include "FreeImage/Dist/FreeImage.h"
#endif
#ifdef __linux__
    #include <FreeImage.h>
#endif
#include "VKFormat.h"

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#endif
#ifdef __APPLE__
    #define DLLExport __attribute__((visibility("default")))
#endif
#ifdef __linux__
	#define DLLExport
#endif

struct Context
{
	GMFSDK::MemWriter* writer;
	std::vector<FIBITMAP*> loadeddata;
	std::vector<FIMEMORY*> loadedmem;

	Context();
	~Context();
};

extern "C"
{
#ifdef _WIN32
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
#endif
    DLLExport Context* CreateContext();
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport void* LoadTexture(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport void* SaveTexture(Context*, wchar_t* extension, const int type, const int width, const int height, const int format, void** mipchain, int* sizechain, const int mipcount, const int layers, uint64_t& returnsize, int flags);
	DLLExport void FreeContext(Context*);
}

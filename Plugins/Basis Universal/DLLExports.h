#pragma once
#include <string>
#include "../../PluginSDK/PluginSDK.h"
#include "../../PluginSDK/TextureInfo.h"
#include "../../PluginSDK/MemWriter.h"
#include "../../PluginSDK/Utilities.h"

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

struct Context
{
	/*MemWriter* writer;
	basisu_transcoder* transcoder;
	std::vector<void*> allocedmem;
	basis_compressor* compressor;
	basist::etc1_global_selector_codebook* sel_codebook;

	Context();
	~Context();*/
};

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport void* LoadTexture(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	//DLLExport void* SavePixmap(int width, int height, int format, void* pixels, uint64_t size, wchar_t* extension, uint64_t& returnsize);
	DLLExport Context* CreateContext();
	DLLExport void FreeContext(Context*);
	DLLExport void* SaveTexture(Context*, wchar_t* extension, const int type, const int width, const int height, const int format, void** mipchain, int* sizechain, const int mipcount, const int layers, uint64_t& returnsize, const int flags);
}
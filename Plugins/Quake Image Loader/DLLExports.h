#pragma once
#include <string>
#include "../../PluginSDK/GMFSDK.h"
#include "../../PluginSDK/TextureInfo.h"
#include "../../PluginSDK/MemReader.h"
#include "../../PluginSDK/MemWriter.h"
#include "../../PluginSDK/Utilities.h"

using namespace GMFSDK;

typedef struct
{
	int id;                // "IDSP"
	long ver1;                   // Version = 1
	long ver12;                  // 1 or 2 (maybe minor version number?)
	float radius;                // Radius of the largest frame
	long maxwidth;               // Width of the largest frame
	long maxheight;              // Height of the largest frame
	long nframes;                // Number of frames
	long uk0;                    // ? (always 0)
	long uk01;                   // ? (0 or 1)
} spr_t;

typedef struct
{
	long ofsx;                   // horizontal offset, in 3D space
	long ofsy;                   // vertical offset, in 3D space
	long width;                  // width of the picture
	long height;                 // height of the picture
	//char* Pixels;   // array of pixels (flat bitmap)
} picture;

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

struct Context
{
	void* mem;
	MemWriter* writer;
};

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport Context* CreateContext();
	DLLExport void* LoadTexture(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport void FreeContext(void*);
}
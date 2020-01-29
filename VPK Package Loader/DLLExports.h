#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <shobjidl_core.h>
	#include <Shlobj.h>
#endif
#include "HLLib/lib/HLLib.h"
#include "../SDK/Utilities.h"
#include <direct.h>

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

struct PackageFile
{
	HLDirectoryItem* hlitem;
	std::string name;
	std::wstring path;
	std::string spath;
	uint64_t size;
};

struct Package
{
	hlUInt hlpak;
	std::vector<PackageFile> files;
};

void LoadDir(HLDirectoryItem* item, const std::string& path, Package* pkg);

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport Package* LoadPackage(void* data, uint64_t size);
	DLLExport int CountPackageFiles(Package* package);
	DLLExport int GetPackageFileName(Package* package, const int index, wchar_t* path, int maxsize);
	DLLExport uint64_t GetPackageFileSize(Package* package, const int index);
	DLLExport int LoadPackageFile(Package* package, int index, void* data, uint64_t size);
	DLLExport void FreePackage(Package* package);
}
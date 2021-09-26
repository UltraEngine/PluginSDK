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
#include "HLLib/HLLib.h"
#include "../../Source/Libraries/PluginSDK/Utilities.h"
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
	HLPackageType type;
	std::vector<std::wstring> loadedfiles;
};

void LoadDir(HLDirectoryItem* item, const std::string& path, Package* pkg);

extern "C"
{
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport Package* LoadPackage(const wchar_t* path);
	DLLExport int GetPackageFileName(HLDirectoryItem* item, wchar_t* path, int maxsize);
	DLLExport uint64_t GetPackageFileSize(HLDirectoryItem* item);
	DLLExport int GetPackageFileType(HLDirectoryItem* item);
	DLLExport int ReadStream(Package* pak, wchar_t* path, void* data, uint64_t sz);
	DLLExport void FreePackage(Package* package);
	DLLExport HLDirectoryItem* GetPackageRoot(Package* pak);
	DLLExport int CountPackageFolderFiles(HLDirectoryItem* item);
	DLLExport int LoadDir(Package* pak, wchar_t* path, int types);
	DLLExport wchar_t* GetLoadedFile(Package* pak, const int index);
	DLLExport int FileType(Package* pak, wchar_t* path);
	DLLExport uint64_t FileSize(Package* pak, wchar_t* path);
	DLLExport int FoldersSupported(Package* pak);
}

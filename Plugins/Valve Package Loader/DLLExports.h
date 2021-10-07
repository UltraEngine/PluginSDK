#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <shobjidl_core.h>
	#include <Shlobj.h>
#endif
#include "HLLib/HLLib.h"
#include "../../Source/Libraries/PluginSDK/MemReader.h"
#include "../../Source/Libraries/PluginSDK/Utilities.h"
#include <direct.h>

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

//----------------------------------------------
// Quake stuff
#define MAXTEXTURENAME 16
#define MIPLEVELS 4
typedef struct _BSPMIPTEX
{
	char szName[MAXTEXTURENAME];  // Name of texture
	uint32_t nWidth, nHeight;     // Extends of the texture
	uint32_t nOffsets[MIPLEVELS]; // Offsets to texture mipmaps BSPMIPTEX;
} BSPMIPTEX;

#define	CMP_NONE		0
#define	CMP_LZSS		1

#define	TYP_NONE		0
#define	TYP_LABEL		1

#define	TYP_LUMPY		64				// 64 + grab command number
#define	TYP_PALETTE		64
#define	TYP_QTEX		65
#define	TYP_QPIC		66
#define	TYP_SOUND		67
#define	TYP_MIPTEX		68

typedef struct
{
	int			width, height;
	byte		data[4];			// variably sized
} qpic_t;

typedef struct
{
	char		identification[4];		// should be WAD2 or 2DAW
	int			numlumps;
	int			infotableofs;
} wadinfo_t;

typedef struct
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[16];				// must be null terminated
} lumpinfo_t;

//----------------------------------------------

struct PackageFile
{
	HLDirectoryItem* hlitem;
	std::string name;
	std::wstring path;
	std::string spath;
	uint64_t size;
};

struct BSPTexture
{
	int width, height;
	void* data;
	std::string name;
	uint32_t offset, size;
};

struct Package
{
	bool isquakewad;
	bool isquakebsp;
	hlUInt hlpak;
	HLPackageType type;
	std::vector<std::wstring> loadedfiles;
	std::map<std::wstring, int> fileindex;
	std::vector<std::wstring> files;
	std::vector<lumpinfo_t> lumps;
	wadinfo_t info;
	FILE* file;
	std::map<std::wstring, Package*> quakesubpackages;
	void* membuffer;
	uint64_t memsize;
	std::vector<BSPTexture> textures;

	Package();
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
	DLLExport int ListFiles(Package* pak);
	DLLExport int PackageValid(const wchar_t* path);
}

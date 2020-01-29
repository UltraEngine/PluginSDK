#include "DLLExports.h"

//Callbacks
extern "C"
{
	hlVoid ExtractItemStartCallback(HLDirectoryItem* pItem)
	{
		int n = 0;
	}

	hlVoid FileProgressCallback(HLDirectoryItem* pFile, hlUInt uiBytesExtracted, hlUInt uiBytesTotal, hlBool* pCancel)
	{
		int n = 0;
	}

	hlVoid ExtractItemEndCallback(HLDirectoryItem* pItem, hlBool bSuccess)
	{
		int n = 0;
	}

	hlVoid DefragmentProgressCallback(HLDirectoryItem* pFile, hlUInt uiFilesDefragmented, hlUInt uiFilesTotal, hlULongLong uiBytesDefragmented, hlULongLong uiBytesTotal, hlBool* pCancel)
	{
		int n = 0;
	}
}

//DLL entry point function
#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hlInitialize();
		//hlSetBoolean(HL_OVERWRITE_FILES, true);
		//hlSetBoolean(HL_FORCE_DEFRAGMENT, false);
		//hlSetVoid(HL_PROC_EXTRACT_ITEM_START, ExtractItemStartCallback);
		//hlSetVoid(HL_PROC_EXTRACT_ITEM_END, ExtractItemEndCallback);
		//hlSetVoid(HL_PROC_EXTRACT_FILE_PROGRESS, FileProgressCallback);
		//hlSetVoid(HL_PROC_VALIDATE_FILE_PROGRESS, FileProgressCallback);
		//hlSetVoid(HL_PROC_DEFRAGMENT_PROGRESS_EX, DefragmentProgressCallback);
		break;
	case DLL_PROCESS_DETACH:
		hlShutdown();
		break;
	}
	return TRUE;
}
#else
int main(int argc, const char* argv[])
{
	return 0;
}
#endif

//Returns all plugin information in a JSON string
int GetPluginInfo(unsigned char* cs, int maxsize)
{
	std::string s =
	"{"
		"\"turboPlugin\":{"
			"\"title\":\"Valve Package Loader.\","
			"\"description\":\"Load Valve package files.\","
			"\"author\":\"Josh Klint\","
			"\"url\":\"www.leadwerks.com\","
			"\"extension\": [\"vpk\"],"
			"\"filter\": [\"Valve Package Format (*.vpk):vpk\"]"
		"}"
	"}";
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
	return s.length();
}

Package* LoadPackage(void* data, uint64_t size)
{
	// Check DLL version
	int version = hlGetUnsignedInteger(HL_VERSION);
	if (version < HL_VERSION_NUMBER)
	{
		printf("Wrong HLLib version: v%s.\n", hlGetString(HL_VERSION));
		return nullptr;
	}

	// Package stuff.
	HLPackageType ePackageType = HL_PACKAGE_NONE;
	hlUInt uiPackage = HL_ID_INVALID, uiMode = HL_MODE_INVALID;
	HLDirectoryItem* pItem = 0;
	
	// Get package type
	ePackageType = hlGetPackageTypeFromMemory(data, size);
	if (ePackageType == HL_PACKAGE_NONE)
	{
		return nullptr;
	}

	// Create
	if (!hlCreatePackage(ePackageType, &uiPackage))
	{
		return nullptr;
	}

	// Bind package
	hlBindPackage(uiPackage);

	// Open package
	//auto uiMode = HL_MODE_READ;// | (bDefragment ? HL_MODE_WRITE : 0);
	//uiMode |= !bFileMapping ? HL_MODE_NO_FILEMAPPING : 0;
	//uiMode |= bQuickFileMapping ? HL_MODE_QUICK_FILEMAPPING : 0;
	//uiMode |= bVolatileAccess ? HL_MODE_VOLATILE : 0;



	// Open the package.
	// Of the above modes, only HL_MODE_READ is required.  HL_MODE_WRITE is present
	// only for future use.  File mapping is recommended as an efficient way to load
	// packages.  Quick file mapping maps the entire file (instead of bits as they are
	// needed) and thus should only be used in Windows 2000 and up (older versions of
	// Windows have poor virtual memory management which means large files won't be able
	// to find a continues block and will fail to load).  Volatile access allows HLLib
	// to share files with other applications that have those file open for writing.
	// This is useful for, say, loading .gcf files while Steam is running.
	
	auto ss = "C:/Program Files (x86)/Steam/steamapps/common/Half-Life 2/hl2/hl2_textures_dir.vpk";	
	if (!hlPackageOpenFile(ss, HL_MODE_READ))
	//if (!hlPackageOpenMemory(data, size, HL_MODE_READ))
	{
		return nullptr;
	}

	// If we have a .ncf file, the package file data is stored externally.  In order to
	// validate the file data etc., HLLib needs to know where to look.  Tell it where.
	if (ePackageType == HL_PACKAGE_NCF)
	{
		hlNCFFileSetRootPath(".");
	}

	// Load package contents recursively
	Package* pkg = new Package;
	pkg->hlpak = uiPackage;
	LoadDir(hlPackageGetRoot(), "", pkg);

	return pkg;
}

static hlChar lpDestination[MAX_PATH] = "";

void LoadDir(HLDirectoryItem* dir, const std::string& path, Package* pkg)
{
	//if (!pkg->files.empty()) return;

	PackageFile file;
	std::string subpath;
	auto count = hlFolderGetFileCount(dir, false) + hlFolderGetFolderCount(dir, false);
	hlUInt size = 0;
	for (uint32_t n = 0; n < count; ++n)
	{
		auto item = hlFolderGetItem(dir, n);
		auto name = hlItemGetName(item);
		//printf(name);
		//printf("\n");
		subpath = path;
		if (subpath != "") subpath += "/";
		subpath += name;
		switch (hlItemGetType(item))
		{
		case HL_ITEM_FOLDER:
			LoadDir(item, subpath, pkg);
			//if (!pkg->files.empty()) return;
			break;
		case HL_ITEM_FILE:
			if (!hlItemGetSize(item, &size)) continue;
			if (size == 0) continue;
			file.name = name;
			file.spath = subpath;
			file.path = GMFSDK::WString(file.spath);
			file.size = size;
			file.hlitem = item;

			auto canextract = hlFileGetExtractable(item);

			int f = 3;

			//HLStream* pStream = 0;
			//if (!hlFileCreateStream(item, &pStream)) return;

			//if (!hlItemGetSize(item, &size)) continue;
			//if (size != file.size) printf("SIZE ERROR\n");
#ifdef DEBUG
			//printf((subpath + std::string("\n")).c_str());
#endif
			pkg->files.push_back(file);
			//return;
			break;
		}
	}
}

int CountPackageFiles(Package* package)
{
	return package->files.size();
}

int GetPackageFileName(Package* package, const int index, wchar_t* path, int maxsize)
{
	if (index < 0 or index >= package->files.size())
	{
		printf("Index out of range.\n");
		return 0;
	}
	package->files[index];
	int sz = package->files[index].path.size();
	if (sz < maxsize && sz > 0)
	{
		memcpy(path, package->files[index].path.c_str(), sz * sizeof(path[0]));
	}
	return sz;
}

uint64_t GetPackageFileSize(Package* package, const int index)
{
	if (index < 0 or index >= package->files.size())
	{
		printf("Index out of range.\n");
		return 0;
	}
	return package->files[index].size;
}

int LoadPackageFile(Package* package, int index, void* data, uint64_t sz)
{
	if (index < 0 or index >= package->files.size())
	{
		printf("Index out of range.\n");
		return 0;
	}


	//This check is failing and I don't know why
	//Second call to hlItemGetSize() returns a value that is too big but stays constant with each run
	//always 3722361786
	hlUInt size;
	hlBool succ = hlItemGetSize(package->files[index].hlitem, &size);
	if (!succ) return 0;
	if (size != package->files[index].size)
	{
		return 0;
	}


	if (sz != package->files[index].size)
	{
		printf("Error: Data size does not match package file.");
		return 0;
	}

	// Bind package
	//hlBindPackage(package->hlpak);
	
	// Extra checking just to be sure, seems to work fine...
	HLFindType ftype = HL_FIND_FILES;
	const hlChar* name = package->files[index].spath.c_str();
	auto item = hlFolderGetItemByPath(hlPackageGetRoot(), name, ftype);
	if (item != package->files[index].hlitem)
	{
		return 0;
	}
	
	const hlChar* iname = hlItemGetName(item);
	if (strcmp(iname, package->files[index].name.c_str()) != 0)
	{
		return 0;
	}

	if (hlItemGetType(item) != HL_ITEM_FILE)
	{
		return 0;
	}

	//This check is failing and I don't know why
	//Second call to hlItemGetSize() returns a value that is too big but stays constant with each run
	//always 3722361786
	//hlUInt size;
	succ = hlItemGetSize(item, &size);
	if (!succ) return 0;
	if (size != package->files[index].size)
	{
		return 0;
	}

	if (hlFileGetExtractable(item) == 0)
	{
		return 0;
	}

	CHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szPath)))
	{
		auto loc = std::string(szPath) + "\\VPK Loader";
		_mkdir(loc.c_str());
		if (hlItemExtract(item, loc.c_str()) != 0)
		{ 
			FILE* file = fopen((loc + "\\" + package->files[index].name).c_str(), "rb");
			if (file != 0)
			{
				fseek(file, 0L, SEEK_END);
				if (ftell(file) == size)
				{
					rewind(file);
					fread(data, size, 1, file);
					fclose(file);
					//remove((loc + "\\" + package->files[index].name).c_str());
					return 1;
				}
			}
		}

	}

	return 0;
}

void FreePackage(Package* package)
{
	if (package->hlpak != 0)
	{
		hlDeletePackage(package->hlpak);
		package->hlpak = 0;
	}
	delete package;
}
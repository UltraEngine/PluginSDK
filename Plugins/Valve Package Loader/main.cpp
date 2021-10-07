#include "DLLExports.h"

using namespace GMFSDK;

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

Package::Package()
{
	membuffer = NULL;
	file = NULL;
	hlpak = -1;
	isquakewad = false;
}

//DLL entry point function
#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hlInitialize();
		hlSetBoolean(HL_OVERWRITE_FILES, true);
		//hlSetBoolean(HL_FORCE_DEFRAGMENT, false);
		//hlSetVoid(HL_PROC_EXTRACT_ITEM_START, ExtractItemStartCallback);
		//hlSetVoid(HL_PROC_EXTRACT_ITEM_END, ExtractItemEndCallback);
		hlSetVoid(HL_PROC_EXTRACT_FILE_PROGRESS, FileProgressCallback);
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
		"\"plugin\":{"
			"\"title\":\"Valve Package Loader\","
			"\"description\":\"Load Valve package files.\","
			"\"author\":\"Josh Klint\","
			"\"threadSafe\":false,"
			"\"loadPackageExtensions\": [\"vpk\",\"bsp\",\"vbsp\",\"wad\",\"pak\"],"
			"\"loadPackageFilters\": [\"Valve Package Format (*.vpk):vpk\",\"Half-Life WAD files (*.wad):wad\"]"
		"}"
	"}";
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
	return s.length();
}

//void(*)(std::wstring&) pp;

void SetPrintFunction(void func(std::wstring&))
{
	//pp = func;
	std::wstring s = L"TEST";
	func(s);
}

void AddFiles(HLDirectoryItem* item, std::vector<std::wstring>& files)
{
	int filecount = hlFolderGetFolderCount(item, false);;
	auto count = hlFolderGetFileCount(item, false) + filecount;
	std::string file;
	char cp[4096];
	//auto sz = files.size();
	//files.resize(sz + filecount);
	files.reserve(files.size() + filecount);
	//int i = 0;
	for (int n = 0; n < count; ++n)
	{
		auto subitem = hlFolderGetItem(item, n);
		switch (hlItemGetType(subitem))
		{
		case HL_ITEM_FOLDER:
			AddFiles(subitem, files);
			continue;
		}
		hlItemGetPath(subitem, cp, 4096);
		file = std::string(&cp[0]);
		//files[sz + i] = GMFSDK::WString(file);
		files.push_back(GMFSDK::WString(file));
		//++i;
	}
}

int ListFiles(Package* pak)
{
	pak->loadedfiles.clear();
	hlBindPackage(pak->hlpak);
	auto item = hlPackageGetRoot();
	if (item == NULL) return 0;
	AddFiles(item, pak->loadedfiles);
	return pak->loadedfiles.size();
}

int PackageValid(const wchar_t* path)
{
	auto spath = GMFSDK::String(path);

	// Get package type
	auto ePackageType = hlGetPackageTypeFromName(spath.c_str());

	if (ePackageType == HL_PACKAGE_VPK)
	{
		//Detect "_XXX.vpk" naming scheme
		if (spath.size() > 7)
		{
			char c[4];
			c[3] = 0;
			c[2] = spath[spath.size() - 5];
			c[1] = spath[spath.size() - 6];
			c[0] = spath[spath.size() - 7];
			std::string s = "1" + std::string(c);
			auto i = atoi(s.c_str());
			if (GMFSDK::String(i) == s) return 0;
		}
	}
	return 1;
}

Package* QLoadPackage(const wchar_t* cpath)
{
	std::wstring path = std::wstring(cpath);
	wadinfo_t header;
	MemReader reader(&header, sizeof(wadinfo_t));
	lumpinfo_t lump;

	//if (Lower(RIght(path,3)) != L"wad") return NULL;

	FILE* file = _wfopen(cpath, L"rb");

	fread(&header, sizeof(header), 1, file);

	if (header.identification[0] != 'W' or header.identification[1] != 'A' or header.identification[2] != 'D' or header.identification[3] != '2')
	{
		fclose(file);
		return NULL;
	}

	auto wad_numlumps = header.numlumps;
	auto infotableofs = header.infotableofs;
	//auto wad_lumps = (lumpinfo_t*)(wad_base + infotableofs);

	Package* pak = new Package;
	pak->isquakewad = true;
	std::wstring wadfile;

	fseek(file, header.infotableofs, 0);

	for (int i = 0; i < wad_numlumps; i++)
	{
		fread(&lump, sizeof(lump), 1, file);
		pak->lumps.push_back(lump);
		wadfile = WString(std::string(lump.name));
		//if (lump.compression) continue;
		//switch (lump.type)
		//{
		//case TYP_QTEX:
		//case TYP_MIPTEX:
		//case TYP_QPIC:
			//(String(lump.type).c_str());
		pak->fileindex[wadfile] = pak->files.size();
		pak->files.push_back(wadfile);
		//	break;
		//}
		//	lump_p->filepos = lump_p.filepos;
		//	lump_p->size = lump_p.size;
			//W_CleanupName(lump_p->name, lump_p->name);
			//if (lump_p.type == TYP_QPIC)
			//	SwapPic((qpic_t*)(wad_base + lump_p->filepos));

	}
	pak->info = header;
	pak->file = file;
	return pak;
}

Package* QReadPackage(const wchar_t* cpath, void* data, uint64_t sz)
{
	MemReader reader(data, sz);

	wadinfo_t header;
	lumpinfo_t lump;

	//if (Lower(RIght(path,3)) != L"wad") return NULL;

	reader.Read(&header, sizeof(header));

	if (header.identification[0] != 'W' or header.identification[1] != 'A' or header.identification[2] != 'D' or header.identification[3] != '2')
	{
		return NULL;
	}

	auto wad_numlumps = header.numlumps;
	auto infotableofs = header.infotableofs;
	//auto wad_lumps = (lumpinfo_t*)(wad_base + infotableofs);

	Package* pak = new Package;
	pak->isquakewad = true;
	std::wstring wadfile;
	
	reader.Seek(header.infotableofs);

	for (int i = 0; i < wad_numlumps; i++)
	{
		reader.Read(&lump, sizeof(lump));
		pak->lumps.push_back(lump);
		wadfile = WString(std::string(lump.name));
		pak->fileindex[wadfile] = pak->files.size();
		pak->files.push_back(wadfile);
	}
	pak->info = header;
	return pak;
}

Package* LoadPackage(const wchar_t* path)
{
	auto spath = GMFSDK::String(path);

	// Check DLL version
	int version = hlGetUnsignedInteger(HL_VERSION);
	if (version < HL_VERSION_NUMBER)
	{
		//printf("Wrong HLLib version: v%s.\n", hlGetString(HL_VERSION));
		return NULL;
	}

	// Package stuff.
	HLPackageType ePackageType = HL_PACKAGE_NONE;
	hlUInt uiPackage = HL_ID_INVALID, uiMode = HL_MODE_INVALID;
	HLDirectoryItem* pItem = 0;
	
	// Get package type
	ePackageType = hlGetPackageTypeFromName(spath.c_str());

	if (ePackageType == HL_PACKAGE_VPK)
	{
		//Detect "_XXX.vpk" naming scheme
		if (spath.size() > 7)
		{
			char c[4];
			c[3] = 0;
			c[2] = spath[spath.size() - 5];
			c[1] = spath[spath.size() - 6];
			c[0] = spath[spath.size() - 7];
			std::string s = "1" + std::string(c);
			auto i = atoi(s.c_str());
			if (GMFSDK::String(i) == s) return NULL;
		}
	}

	if (ePackageType == HL_PACKAGE_NONE)
	{
		auto ext = ExtractExt(spath);
		if (ext == "bsp") ePackageType = HL_PACKAGE_BSP;
		if (ext == "gcf") ePackageType = HL_PACKAGE_GCF;
		//if (ext == "zip" or ext == "pk3" or ext == "pk4")
		//{
		//	ePackageType = HL_PACKAGE_ZIP;
		//}
		if (ePackageType == HL_PACKAGE_NONE)
		{
			return QLoadPackage(path);
		}
	}

	// Create
	if (!hlCreatePackage(ePackageType, &uiPackage))
	{
		return QLoadPackage(path);
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
	
	//auto ss = "C:/Program Files (x86)/Steam/steamapps/common/Half-Life 2/hl2/hl2_textures_dir.vpk";	
	if (!hlPackageOpenFile(spath.c_str(), HL_MODE_READ))
	//if (!hlPackageOpenMemory(data, size, HL_MODE_READ))
	{
		hlDeletePackage(uiPackage);
		return QLoadPackage(path);
	}

	// If we have a .ncf file, the package file data is stored externally.  In order to
	// validate the file data etc., HLLib needs to know where to look.  Tell it where.
	if (ePackageType == HL_PACKAGE_NCF)
	{
		hlNCFFileSetRootPath(".");
	}

	auto pak = new Package;
	pak->hlpak = uiPackage;

	auto root = GetPackageRoot(pak);
	if (root == NULL)
	{
		delete pak;
		return NULL;
	}
	/*if (CountPackageFolderFiles(root) == 0)
	{
		delete pak;
		return NULL;
	}*/

	return pak;
}

static hlChar lpDestination[MAX_PATH] = "";

HLDirectoryItem* GetPackageRoot(Package* package)
{
	hlBindPackage(package->hlpak);
	return (void*)hlPackageGetRoot();
}

/*HLDirectoryItem* FindPackageFile(Package* pak, wchar_t* path)
{
	auto dir = GetItem()
	return NULL;
}*/

int QLoadDir(Package* pak, wchar_t* path, int types)
{
	for (int n = 0; n < pak->lumps.size(); ++n)
	{
		std::string s = std::string(pak->lumps[n].name);
		std::wstring file = WString(s);
		pak->loadedfiles.push_back(file);
	}
	return pak->lumps.size();
}

wchar_t* QGetLoadedFile(Package* pak, const int index)
{
	if (index < 0 or index >= pak->files.size())
	{
		return NULL;
	}
	const auto& s = pak->files[index];
	return (wchar_t*)s.c_str();
}

int QFileType(Package* pak, wchar_t* path)
{
	std::wstring s = std::wstring(path);
	if (pak->fileindex.find(s) == pak->fileindex.end()) return 0;
	return 1;
}

uint64_t QFileSize(Package* pak, wchar_t* path)
{
	std::wstring s = std::wstring(path);
	auto it = pak->fileindex.find(s);
	if (it == pak->fileindex.end()) return 0;
	return pak->lumps[it->second].size;
}

int QReadStream(Package* pak, wchar_t* path, void* data, uint64_t sz)
{
	std::wstring s = std::wstring(path);
	auto it = pak->fileindex.find(s);
	if (it == pak->fileindex.end()) return 0;
	if (pak->file)
	{
		fseek(pak->file, pak->lumps[it->second].filepos, 0);
		fread(data, pak->lumps[it->second].size, 1, pak->file);
	}
	else
	{
		MemReader reader(pak->membuffer,pak->memsize);
		reader.Seek(pak->lumps[it->second].filepos);
		reader.Read(data, pak->lumps[it->second].size);
	}
	char c[5];
	c[4] = 0;
	memcpy(c, data, 4);
	std::string ss = std::string(c);

	return 1;
}

int GetPackageFileName(HLDirectoryItem* item, wchar_t* path, int maxsize)
{
	return 0;
}

int GetPackageFileType(HLDirectoryItem* item)
{
	switch (hlItemGetType(item))
	{
	case HL_ITEM_FOLDER:
		return 2;
	case HL_ITEM_FILE:
		return 1;
	}
	return 0;
}

int CountPackageFolderFiles(HLDirectoryItem* item)
{
	if (hlItemGetType(item) != HL_ITEM_FOLDER) return 0;// should never happen!
	return hlFolderGetFolderCount(item, false) + hlFolderGetFileCount(item, false);
}

int LoadDir(Package* pak, wchar_t* cpath, int types)
{
	std::wstring path = std::wstring(cpath);
	if (pak->isquakewad)
	{
		return QLoadDir(pak, cpath, types);
	}

	printf(String(path + std::wstring(L"\n")).c_str());

	if (pak->quakesubpackages.find(path) != pak->quakesubpackages.end())
	{
		wchar_t wc = 0;
		auto r = QLoadDir(pak->quakesubpackages[path], &wc, types);
		pak->loadedfiles = pak->quakesubpackages[path]->loadedfiles;
		return r;
	}

	hlBindPackage(pak->hlpak);

	bool findfiles = ((1 & types) != 0);
	bool findfolders = ((2 & types) != 0);
	if (!findfiles)
	{
		switch (hlPackageGetType())
		{
		case HL_PACKAGE_WAD:
		case HL_PACKAGE_BSP:
			return 0;
		}
	}

	auto root = hlPackageGetRoot();
	auto s = GMFSDK::String(path);

	auto item = hlFolderGetItemByPath(root, s.c_str(), HL_FIND_ALL);
	if (item == NULL) return 0;
	auto count = hlFolderGetFileCount(item, false) + hlFolderGetFolderCount(item, false);
	std::string file;
	char cp[4096];
	pak->loadedfiles.clear();

	//Maybe this is an old Quake WAD file...
	if (count == 0 and ExtractExt(cpath) == L"wad")
	{
		auto sz = FileSize(pak,cpath);
		void* mem = malloc(sz);
		if (ReadStream(pak, cpath, mem, sz) == 1)
		{
			auto qpak = QReadPackage(cpath, mem, sz);
			qpak->membuffer = mem;
			pak->memsize = sz;
			pak->quakesubpackages[path] = qpak;
		}
		else
		{
			free(mem);
		}
	}

	for (int n = 0; n < count; ++n)
	{
		auto subitem = hlFolderGetItem(item, n);
		switch (hlItemGetType(subitem))
		{
		case HL_ITEM_FILE:
			if (!findfiles) continue;
			break;
		case HL_ITEM_FOLDER:
			if (!findfolders) continue;
			break;
		}
		//hlItemGetPath(item, cp, 4096);
		//std::string path = std::string(cp);
		file = std::string(hlItemGetName(subitem));
		pak->loadedfiles.push_back(GMFSDK::WString(file));
	}
	return pak->loadedfiles.size();
}

int FileType(Package* pak, wchar_t* path)
{
	if (pak->isquakewad) return QFileType(pak, path);

	auto s = GMFSDK::String(path);
	std::string ext;
	std::string name;
	hlBindPackage(pak->hlpak);
	auto root = hlPackageGetRoot();
	auto item = hlFolderGetItemByPath(root, s.c_str(), HL_FIND_ALL);
	if (item == NULL) return 0;
	switch (hlItemGetType(item))
	{
	case HL_ITEM_FILE:
		name = std::string(hlItemGetName(item));
		ext = ExtractExt(name);
		if (ext == "bsp" or ext == "vbsp" or ext == "wad")
		{
			//if (hlPackageOpenMemory())
			return 3;
		}
		return 1;
	case HL_ITEM_FOLDER:
		return 2;
	}
	return 0;
}

uint64_t FileSize(Package* pak, wchar_t* path)
{
	if (pak->isquakewad) return QFileSize(pak, path);
	auto dir = ExtractDir(std::wstring(path));
	if (pak->quakesubpackages.find(dir) != pak->quakesubpackages.end())
	{
		auto file = StripDir(std::wstring(path));
		return QFileSize(pak->quakesubpackages[dir], (wchar_t*)file.c_str());
	}

	auto s = GMFSDK::String(path);
	hlBindPackage(pak->hlpak);
	auto root = hlPackageGetRoot();
	auto item = hlFolderGetItemByPath(root, s.c_str(), HL_FIND_ALL);
	if (item == NULL) return 0;
	hlUInt sz;
	if (hlItemGetSize(item,&sz)) return sz;
	return 0;
}

wchar_t* GetLoadedFile(Package* pak, const int index)
{
	if (pak->isquakewad) return QGetLoadedFile(pak, index);
	return (wchar_t*)pak->loadedfiles[index].c_str();
}

HLDirectoryItem* GetPackageFolderFile(HLDirectoryItem* item, int index)
{
	return NULL;
	//hlFolderGetItemByPath(item, path, HL_FIND_ALL)
	//return hlFolderGetItem(item, index);
}

uint64_t GetPackageFileSize(HLDirectoryItem* item)
{
	if (hlItemGetType(item) != HL_ITEM_FILE) return 0;// should never happen!
	hlUInt sz = 0;
	if (!hlItemGetSize(item, &sz)) return 0;
	return sz;
}

int ReadStream(Package* pak, wchar_t* path, void* data, uint64_t sz)
{
	if (pak->isquakewad) return QReadStream(pak, path, data, sz);

	auto dir = ExtractDir(std::wstring(path));
	if (pak->quakesubpackages.find(dir) != pak->quakesubpackages.end())
	{
		auto s = StripDir(std::wstring(path));
		return QReadStream(pak->quakesubpackages[dir], (wchar_t*)s.c_str(), data, sz);
	}

	auto s = GMFSDK::String(path);
	hlBindPackage(pak->hlpak);
	auto root = hlPackageGetRoot();
	auto item = hlFolderGetItemByPath(root, s.c_str(), HL_FIND_ALL);
	if (item == NULL) return 0;

	//This check is failing and I don't know why
	//Second call to hlItemGetSize() returns a value that is too big but stays constant with each run
	//always 3722361786
	hlUInt size;
	hlBool succ = hlItemGetSize(item, &size);
	if (!succ)
	{
		return 0;
	}

	// Extra checking just to be sure, seems to work fine...
	if (hlItemGetType(item) != HL_ITEM_FILE)
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
		auto loc = std::string(szPath) + "\\Ultra Engine";
		_mkdir(loc.c_str());
		loc += "\\Temp";
		_mkdir(loc.c_str());
		if (hlItemExtract(item, loc.c_str()) != 0)
		{
			s = loc + std::string("\\") + std::string(hlItemGetName(item));
			FILE* file = fopen(s.c_str(), "rb");
			if (file != 0)
			{
				fseek(file, 0L, SEEK_END);
				if (ftell(file) == size)
				{
					rewind(file);
					fread(data, size, 1, file);
					fclose(file);
					remove(s.c_str());
					return 1;
				}
			}
		}
	}
	return 0;
}

void FreePackage(Package* pak)
{
	hlDeletePackage(pak->hlpak);
	if (pak->file != NULL) fclose(pak->file);
	pak->hlpak = 0;
	if (pak->membuffer != NULL) free(pak->membuffer);
	delete pak;
}
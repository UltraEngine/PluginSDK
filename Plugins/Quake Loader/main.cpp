#include "DLLExports.h"
#include "Loaders/SPR.h"
#include "Loaders/WAD.h"
#include "Loaders/BSP.h"
#include "Loaders/LMP.h"
#include "Loaders/PAK.h"
#include "Loaders/MDL.h"
#include "Package.h"

using namespace UltraEngine::PluginSDK;

Context::Context() : mem(NULL) {}

//DLL entry point function
#ifdef _WIN32

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
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
	nlohmann::json j3;
	j3["plugin"]["title"] = "Quake File Loader";
	j3["plugin"]["description"] = "Load Quake game files";
	j3["plugin"]["author"] = "Ultra Software, id Software, Ryan Gregg";
	j3["plugin"]["threadSafe"] = false;
	j3["plugin"]["extensions"] = nlohmann::json::array();

	nlohmann::json ext;

	//Package formats
	ext["extension"] = "bsp";
	ext["description"] = "Quake BSP";
	j3["plugin"]["loadPackageExtensions"].push_back(ext);
	
	ext["extension"] = "pak";
	ext["description"] = "Quake Package";
	j3["plugin"]["loadPackageExtensions"].push_back(ext);
	
	ext["extension"] = "wad";
	ext["description"] = "Quake Texture WAD";
	j3["plugin"]["loadPackageExtensions"].push_back(ext);
	
	//Texture formats
	ext["extension"] = "spr";
	ext["description"] = "Quake Sprite";
	j3["plugin"]["loadTextureExtensions"].push_back(ext);

	ext["extension"] = "lmp";
	ext["description"] = "Quake Image";
	j3["plugin"]["loadTextureExtensions"].push_back(ext);

	//Model formats
	ext["extension"] = "mdl";
	ext["description"] = "Quake Model";
	j3["plugin"]["loadModelExtensions"].push_back(ext);

	ext["extension"] = "bsp";
	ext["description"] = "Quake BSP";
	j3["plugin"]["loadModelExtensions"].push_back(ext);

	std::string s = j3.dump(1, '	');
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize, s.length()));
	return s.length();
}

void* LoadModel(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	auto result = LoadModelMDL(context, data, size, cpath, returnsize);
	if (result != NULL) return result;
	result = LoadModelBSP(context, data, size, cpath, returnsize);
	if (result != NULL) return result;
	return NULL;
}

void* LoadTexture(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	auto result = LoadTextureSPR(context, data, size, cpath, returnsize);
	if (result != NULL) return result;
	result = LoadTextureLMP(context, data, size, cpath, returnsize);
	if (result != NULL) return result;
	return NULL;
}

int FileType(Package* pak, wchar_t* cpath)
{
	auto path = std::wstring(cpath);
	if (pak->rebuild) pak->BuildDirectoryStructure();// rebuild only when needed
	auto it = pak->filemap.find(path);
	if (it == pak->filemap.end()) return 0;
	auto dir = (*it).second;
	return dir->type;
}

uint64_t FileSize(Package* pak, wchar_t* cpath)
{
	auto path = std::wstring(cpath);
	if (pak->rebuild) pak->BuildDirectoryStructure();// rebuild only when needed
	auto it = pak->filemap.find(path);
	if (it == pak->filemap.end()) return 0;
	auto file = (*it).second;
	
	// Calculate size of conveted DDS files
	if ((file->flags & PACKAGEFILE_WADTEXTURE) != 0 or (file->flags & PACKAGEFILE_BSPTEXTURE) != 0)
	{
		fseek(pak->file, file->offset, 0);
		return GetWADTextureSize(pak->file, file->lump, file->bspmiptex.nWidth, file->bspmiptex.nHeight);
	}
	return file->size;
}

int LoadDir(Package* pak, wchar_t* cpath, int types)
{
	auto path = std::wstring(cpath);
	if (pak->rebuild) pak->BuildDirectoryStructure();// rebuild only when needed
	auto it = pak->filemap.find(path);
	if (it == pak->filemap.end()) return 0;
	auto dir = (*it).second;
	if ((dir->type & 2) == 0) return 0;
	pak->loadedfiles.clear();
	for (auto pair : dir->kids)
	{
		auto sarr = Split(pair.second->path, L"/");
		pak->loadedfiles.push_back(sarr[sarr.size() - 1]);
	}
	return pak->loadedfiles.size();
}

/*void* LoadPackageTexture(Context* context, Package* pak, wchar_t* cpath, uint64_t& returnsize)
{
	auto path = std::wstring(cpath);
	if (pak->rebuild) pak->BuildDirectoryStructure();// rebuild only when needed
	auto it = pak->filemap.find(Lower(path));
	if (it == pak->filemap.end()) return 0;
	auto file = (*it).second;
	if (file->type != 1) return 0;
	if (file->lump.size == 0) return 0;
	fseek(pak->file, file->offset, 0);
	return LoadWADTexture(context, pak->file, file->lump, returnsize, file->bspmiptex.nWidth, file->bspmiptex.nHeight);
}*/

Context* CreateContext()
{
	return new Context;
}

void FreeContext(Context* ctx)
{
	if (ctx->mem) free(ctx->mem);
	for (auto m : ctx->memblocks)
	{
		free(m);
	}
	ctx->memblocks.clear();
	delete ctx;
}

Package* LoadPackage(const wchar_t* path)
{
	auto file = _wfopen(path, L"rb");
	if (file == NULL) return NULL;
	auto pak = LoadPackagePAK(file);
	if (pak) return pak;
	fseek(file, 0, 0);
	pak = LoadPackageWAD(file);
	if (pak) return pak;
	fseek(file, 0, 0);
	pak = LoadPackageBSP(file);
	if (pak) return pak;
	fclose(file);
	return NULL;
}

int ReadStream(Package* pak, wchar_t* cpath, void* data, uint64_t sz)
{
	auto path = std::wstring(cpath);
	if (pak->rebuild) pak->BuildDirectoryStructure();// rebuild only when needed
	auto it = pak->filemap.find(path);
	if (it == pak->filemap.end()) return 0;
	auto file = (*it).second;
	if ((file->type & 1) == 0) return 0;

	// Convert embedded image data into DDS since the raw data is useless anyways
	if ((file->flags & PACKAGEFILE_WADTEXTURE) != 0 or (file->flags & PACKAGEFILE_BSPTEXTURE) != 0)
	{
		fseek(pak->file, file->offset, 0);
		return LoadWADTexture(pak->file, data, sz, file->lump, file->bspmiptex.nWidth, file->bspmiptex.nHeight);
	}
	if ((file->flags & PACKAGEFILE_BSPTEXTURE) != 0)
	{

	}

	if (file->size > sz) return 0;
	fseek(pak->file, file->offset, 0);
	return fread(data, sz, 1, pak->file);
}

void FreePackage(Package* package)
{
	if (package->file) fclose(package->file);
	package->file = NULL;
	delete package;
}

const wchar_t* GetLoadedFile(Package* pak, int index)
{
	if (index < 0 or index >= pak->loadedfiles.size()) return nullptr;
	return pak->loadedfiles[index].c_str();
}
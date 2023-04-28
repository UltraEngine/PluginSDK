#pragma once
#include "DLLExports.h"

enum PackageFileFlags
{
	PACKAGEFILE_WADTEXTURE = 1,
	PACKAGEFILE_BSPTEXTURE = 2
};

struct PackageFile
{
	_BSPMIPTEX bspmiptex;
	std::string name;
	std::wstring path;
	std::string spath;
	int type;
	uint64_t size, offset;
	std::map<std::wstring, std::shared_ptr<PackageFile> > kids;
	lumpinfo_t lump;
	PackageFileFlags flags;

	PackageFile();
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
	bool rebuild;
	std::vector<PackageFile> files;
	std::map<std::wstring, std::shared_ptr<PackageFile> > filemap;
	std::shared_ptr<PackageFile> root;
	bool isquakewad;
	bool isquakebsp;
	bool isquakepak;
	bool modified;
	std::vector<std::wstring> loadedfiles;
	std::map<std::wstring, int> fileindex;
	std::vector<lumpinfo_t> lumps;
	wadinfo_t info;
	FILE* file;
	std::map<std::wstring, Package*> quakesubpackages;
	void* membuffer;
	uint64_t memsize;
	std::vector<BSPTexture> textures;

	Package();

	void BuildDirectoryStructure();
};
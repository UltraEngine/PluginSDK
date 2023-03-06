#include "Package.h"

PackageFile::PackageFile()
{
	type = 0; size = 0; offset = 0;
	lump = {};
	bspmiptex = {};
	flags = PackageFileFlags(0);
}

Package::Package()
{
	modified = false;
	membuffer = NULL;
	file = NULL;
	isquakewad = false;
	isquakebsp = false;
	isquakepak = false;
	rebuild = true;
}

void Package::BuildDirectoryStructure()
{
	filemap.clear();
	rebuild = false;
	root = std::make_shared<PackageFile>();
	root->type = 2;
	filemap[L""] = root;
	for (auto file : files)
	{
		auto sarr = Split(file.path, L"/");
		auto dir = root;
		std::wstring subpath;
		for (int n = 0; n < sarr.size(); ++n)
		{
			const auto& part = sarr[n];
			if (not subpath.empty()) subpath += L"/";
			subpath += part;
			auto subdir = dir->kids[Lower(part)];
			if (subdir == NULL)
			{
				subdir = std::make_shared<PackageFile>();
				filemap[Lower(subpath)] = subdir;
				subdir->path = subpath;
				dir->kids[Lower(part)] = subdir;
				subdir->type = 2;
				if (n == sarr.size() - 1)
				{
					subdir->type = file.type;
					subdir->offset = file.offset;
					subdir->size = file.size;
					subdir->lump = file.lump;
					subdir->bspmiptex = file.bspmiptex;
					subdir->flags = file.flags;
				}
			}
			dir = subdir;
		}
	}
}
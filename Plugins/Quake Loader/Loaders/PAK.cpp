#include "PAK.h"

Package* LoadPackagePAK(FILE* file)
{
	pakheader_t header = {};

	auto start = _ftelli64(file);

	_fseeki64(file, 0, SEEK_END);
	auto fs = _ftelli64(file) - start;
	_fseeki64(file, start, SEEK_SET);

	if (fs < sizeof(header))
	{
		return NULL;
	}

	fread(&header, sizeof(header), 1, file);

	if (header.magic[0] != 'P' or header.magic[1] != 'A' or header.magic[2] != 'C' or header.magic[3] != 'K')
	{
		return NULL;
	}

	Package* pak = new Package;
	pak->isquakepak = true;
	pak->file = file;

	_fseeki64(file, header.diroffset, 0);
	pakentry_t entry = {};
	int count = header.dirsize / 0x40;
	PackageFile pfile;
	for (int n = 0; n < count; ++n)
	{
		fread(&entry, sizeof(entry), 1, file);
		pfile.path = WString(entry.filename);
		pfile.type = 1;
		pfile.size = entry.size;
		pfile.offset = entry.offset + start;
		pak->files.push_back(pfile);

		auto ext = Lower(ExtractExt(entry.filename));
		if (ext == "wad")
		{
			auto here = ftell(file);
			fseek(file, pfile.offset, 0);
			auto subpak = LoadPackageWAD(file);
			if (subpak)
			{
				pak->files[pak->files.size() - 1].type = 3;
				for (auto subfile : subpak->files)
				{
					subfile.path = pfile.path + L"/" + subfile.path;
					pak->files.push_back(subfile);
				}
				delete subpak;
			}
			fseek(file, here, 0);
		}
		else if (ext == "bsp")
		{
			auto here = ftell(file);
			fseek(file, pfile.offset, 0);
			auto subpak = LoadPackageBSP(file);
			if (subpak)
			{
				pak->files[pak->files.size() - 1].type = 3;
				for (auto subfile : subpak->files)
				{
					subfile.path = pfile.path + L"/" + subfile.path;
					pak->files.push_back(subfile);
				}
				delete subpak;
			}
			fseek(file, here, 0);
		}
	}
	return pak;
}
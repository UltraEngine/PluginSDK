#include "WAD.h"
#include "DDS.h"

/*Package* LoadPackageWAD(const wchar_t* cpath, void* data, uint64_t sz)
{
	MemReader reader(data, sz);

	wadinfo_t header;
	lumpinfo_t lump;

	//if (Lower(RIght(path,3)) != L"wad") return NULL;

	reader.Read(&header, sizeof(header));

	if (header.identification[0] != 'W' or header.identification[1] != 'A' or header.identification[2] != 'D' or header.identification[3] != '2') return NULL;

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
}*/

Package* LoadPackageWAD(FILE* file)
{
	auto start = _ftelli64(file);

	wadinfo_t header;
	MemReader reader(&header, sizeof(wadinfo_t));
	lumpinfo_t lump;

	//if (Lower(RIght(path,3)) != L"wad") return NULL;

	if (file == NULL) return NULL;

	fread(&header, sizeof(header), 1, file);

	if (header.identification[0] != 'W' or header.identification[1] != 'A' or header.identification[2] != 'D' or header.identification[3] != '2')
	{
		//fclose(file);
		return NULL;
	}

	auto wad_numlumps = header.numlumps;
	auto infotableofs = header.infotableofs;
	//auto wad_lumps = (lumpinfo_t*)(wad_base + infotableofs);

	Package* pak = new Package;
	pak->isquakewad = true;
	std::wstring wadfile;

	fseek(file, start + header.infotableofs, 0);

	PackageFile pfile;
	for (int i = 0; i < wad_numlumps; i++)
	{
		fread(&lump, sizeof(lump), 1, file);
		pak->lumps.push_back(lump);
		//wadfile = WString(std::string(lump.name));
		//if (lump.compression) continue;
		//switch (lump.type)
		//{
		//case TYP_QTEX:
		//case TYP_MIPTEX:
		//case TYP_QPIC:
			//(String(lump.type).c_str());
		//pak->fileindex[wadfile] = pak->files.size();

		pfile.type = 1;
		pfile.path = WString(lump.name);
		pfile.size = lump.size;
		pfile.offset = lump.filepos + start;
		pfile.lump = lump;
		pfile.flags = PACKAGEFILE_WADTEXTURE;
		pak->files.push_back(pfile);

		//pak->files.push_back(wadfile);
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
	//fclose(file);
	return pak;
}
 
uint64_t GetWADTextureSize(FILE* file, const lumpinfo_t& lump, int w, int h)
{
	if (lump.type == '@')
	{
		w = 256;
		h = 64;
	}

	if (lump.type == 'B')
	{
		fread(&w, 4, 1, file);
		fread(&h, 4, 1, file);

		//Really sketchy...
		if (lump.size < w * h + 8)
		{
			//context->Print(L"Error: Lump size is too small for pixel data");
			return NULL;
		}

	}

	if (lump.type == 'E')
	{
		if (lump.name == "background")
		{
			w = 320;
			h = 200;
		}
		else
		{
			w = 128;
			h = 128;
		}
	}

	if (lump.type == 'D')
	{
		char name[16] = {};
		fread(&name[0], 16, 1, file);
		fread(&w, 4, 1, file);
		fread(&h, 4, 1, file);

		// 16 bytes for why???
		short info[8];
		fread(&info[0], 2, 1, file);
		fread(&info[1], 2, 1, file);
		fread(&info[2], 2, 1, file);
		fread(&info[3], 2, 1, file);
		fread(&info[4], 2, 1, file);
		fread(&info[5], 2, 1, file);
		fread(&info[6], 2, 1, file);
		fread(&info[7], 2, 1, file);

		//Really sketchy...
		if (lump.size < w * h + 16 + 8 + 16)
		{
			//context->Print(L"Error: Lump size is too small for pixel data");
			return NULL;
		}
	}

	if (lump.size < w * h)
	{
		//context->Print(L"Error: Lump size is too small for pixel data");
		return NULL;
	}

	// Magic + Header + pixels
	auto sz = 4 + sizeof(DDS_HEADER) + (w * h * 4);
	return sz;
}

int LoadWADTexture(FILE* file, void* data, uint64_t datasize, const lumpinfo_t& lump, int w, int h)
{
	MemWriter writer(data, datasize);

	int sz = 0;
	TextureInfo texinfo;
	texinfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	
	/*
	https://six-of-one.github.io/quake-specifications/qkspec_7.htm#CWAD0
	0x40=	'@'=	Color Palette
	0x42=	'B'=	Pictures for status bar
	0x44=	'D'=	Used to be Mip Texture
	0x45=	'E'=	Console picture (flat)
	*/
	
	if (lump.type == '@')
	{
		w = 256;
		h = 64;
	}

	if (lump.type == 'B')
	{
		fread(&w, 4, 1, file);
		fread(&h, 4, 1, file);

		//Really sketchy...
		if (lump.size < w * h + 8)
		{
			//context->Print(L"Error: Lump size is too small for pixel data");
			return NULL;
		}

	}

	if (lump.type == 'E')
	{
		if (lump.name == "background")
		{
			w = 320;
			h = 200;
		}
		else
		{
			w = 128;
			h = 128;
		}
	}

	if (lump.type == 'D')
	{
		char name[16] = {};
		fread(&name[0], 16, 1, file);
		fread(&w, 4, 1, file);
		fread(&h, 4, 1, file);

		// 16 bytes for why???
		short info[8];
		fread(&info[0], 2, 1, file);
		fread(&info[1], 2, 1, file);
		fread(&info[2], 2, 1, file);
		fread(&info[3], 2, 1, file);
		fread(&info[4], 2, 1, file);
		fread(&info[5], 2, 1, file);
		fread(&info[6], 2, 1, file);
		fread(&info[7], 2, 1, file);

		//Really sketchy...
		if (lump.size < w * h + 16 + 8 + 16)
		{
			//context->Print(L"Error: Lump size is too small for pixel data");
			return NULL;
		}
	}

	sz = w * h;

	if (lump.size < w * h)
	{
		//context->Print(L"Error: Lump size is too small for pixel data");
		return NULL;
	}

	int magic = int(MAGIC_DDS);
	writer.Write(&magic);

	DDS_HEADER header(w, h);
	writer.Write(&header);

	/*context->mem = malloc(sz * 4);
	if (context->mem == NULL)
	{
		context->Print(L"Error: Failed to allocate memory");
		return NULL;
	}*/
	unsigned char index;
	int rgb;
	unsigned char c[4];
	c[0] = 255; c[1] = 0; c[2] = 0;
	memcpy(&rgb, &c[0], 3);
	unsigned char a;

	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < h; ++y)
		{
			fread(&index, 1, 1, file);
			//reader.Read(&index);
			rgb = qpallete[index];
			memcpy(&c[0], &rgb, 4);
			//memcpy(((unsigned char*)context->mem) + n * 4, &rgb, 3);
			c[3] = 255;
			if (index == 0xFF) a = 0;// transparency
			memcpy(&rgb, &c[0], 4);
			writer.Write(&rgb);
			//memcpy(((unsigned char*)context->mem) + n * 4 + 3, &a, 1);
			//n++;
		}
	}
	sz *= 4;
	
	/*texinfo.version = 201;
	texinfo.target = 2;
	texinfo.width = w;
	texinfo.height = h;
	texinfo.faces = 1;
	texinfo.mipmaps = 1;
	texinfo.depth = 1;
	texinfo.frames = 1;*/

	//context->writer.Write(&texinfo);
	//context->writer.Write(&sz);
	//context->writer.Write(&context->mem, sizeof(void*));

	//returnsize = context->writer.Size();
	//return context->writer.data();

	return 1;
}
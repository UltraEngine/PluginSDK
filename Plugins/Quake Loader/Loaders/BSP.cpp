#include "BSP.h"

const int LUMP_TEXTURES = 2;
/*#define LUMP_ENTITIES      0
#define LUMP_PLANES        1
#define LUMP_TEXTURES      2
#define LUMP_VERTICES      3
#define LUMP_VISIBILITY    4
#define LUMP_NODES         5
#define LUMP_TEXINFO       6
#define LUMP_FACES         7
#define LUMP_LIGHTING      8
#define LUMP_CLIPNODES     9
#define LUMP_LEAVES       10
#define LUMP_MARKSURFACES 11
#define LUMP_EDGES        12
#define LUMP_SURFEDGES    13
#define LUMP_MODELS       14
#define HEADER_LUMPS      15*/

Package* LoadPackageBSP(FILE* file)
{
	if (file == NULL) return NULL;
	
	auto start = ftell(file);

	uint32_t nMipTextures;
	int BSPMIPTEXOFFSET;
	int version = 0;
	fread(&version, sizeof(version), 1, file);
	if (version != 29)
	{
		return NULL;
	}
	//reader.Seek(4 + (LUMP_TEXTURES) * 8);
	fseek(file, start + 4 + LUMP_TEXTURES * 8, 0);

	Package* pak = new Package;
	pak->isquakewad = true;
	pak->isquakebsp = true;

	uint32_t lumpoffset, lumpsize;

	//reader.Read(&lumpoffset);
	//reader.Read(&lumpsize);
	fread(&lumpoffset, sizeof(lumpoffset), 1, file);
	fread(&lumpsize, sizeof(lumpsize), 1, file);

	//reader.Seek(lumpoffset);
	//reader.Read(&nMipTextures);
	fseek(file, start + lumpoffset, 0);
	fread(&nMipTextures, sizeof(nMipTextures), 1, file);

	PackageFile pfile;

	_BSPMIPTEX tex;
	for (int n = 0; n < nMipTextures; ++n)
	{
		//reader.Seek(lumpoffset + 4 + n * 4);
		//reader.Read(&BSPMIPTEXOFFSET);
		fseek(file, start + lumpoffset + 4 + n * 4, 0);
		fread(&BSPMIPTEXOFFSET, sizeof(BSPMIPTEXOFFSET), 1, file);

		if (BSPMIPTEXOFFSET == -1) continue; // this can and does happen!
		
		//reader.Seek(BSPMIPTEXOFFSET + lumpoffset);
		//reader.Read(&tex);
		fseek(file, start + BSPMIPTEXOFFSET + lumpoffset, 0);
		fread(&tex, sizeof(tex), 1, file);
		
		if (tex.nOffsets[0] == 0) continue; // texture stored in WAD, use name to find it
		lumpinfo_t lump = {};
		lump.filepos = tex.nOffsets[0];
		lump.size = tex.nWidth * tex.nHeight + 16;
		memcpy(&lump.name, tex.szName, 16);
		lump.disksize = 8 + tex.nWidth * tex.nHeight * 1;
		pak->fileindex[WString(std::string(lump.name))] = pak->lumps.size();
		pak->lumps.push_back(lump);

		BSPTexture btex = {};
		btex.width = tex.nWidth;
		btex.name = std::string(tex.szName);
		btex.height = tex.nHeight;
		btex.offset = tex.nOffsets[0] + BSPMIPTEXOFFSET + lumpoffset;
		btex.size = tex.nWidth * tex.nHeight + 8;
		//pak->textures.push_back(btex);

		pfile.offset = start + btex.offset;
		pfile.type = 1;
		pfile.lump = lump;
		pfile.size = lump.size;
		pfile.path = WString(lump.name);
		pfile.bspmiptex = tex;
		pfile.flags = PACKAGEFILE_BSPTEXTURE;

		pak->files.push_back(pfile);

		//reader.Seek(BSPMIPTEXOFFSET + lumpoffset + tex.nOffsets[0]);
		fseek(file, start + BSPMIPTEXOFFSET + lumpoffset + tex.nOffsets[0], 0);
		
		char c[4];
		//reader.Read(c, 4);
		fread(&c[0], 4, 1, file);
	}
	
	pak->file = file;
	return pak;
}

/*Package* LoadPackageBSP(const wchar_t* cpath, void* data, uint64_t sz)
{

	Package* pak = new Package;
	pak->isquakewad = true;
	pak->isquakebsp = true;
	uint32_t nMipTextures;
	int BSPMIPTEXOFFSET;
	MemReader reader(data, sz);
	int version = 0;
	reader.Read(&version);
	if (version != 29)
	{
		return NULL;
	}
	reader.Seek(4 + (LUMP_TEXTURES) * 8);

	uint32_t lumpoffset, lumpsize;

	reader.Read(&lumpoffset);
	reader.Read(&lumpsize);

	reader.Seek(lumpoffset);
	reader.Read(&nMipTextures);
	_BSPMIPTEX tex;
	for (int n = 0; n < nMipTextures; ++n)
	{
		reader.Seek(lumpoffset + 4 + n * 4);
		reader.Read(&BSPMIPTEXOFFSET);
		if (BSPMIPTEXOFFSET == -1) continue; // this can and does happen!
		reader.Seek(BSPMIPTEXOFFSET + lumpoffset);
		reader.Read(&tex);
		if (tex.nOffsets[0] == 0) continue; // texture stored in WAD, use name to find it
		lumpinfo_t lump = {};
		lump.filepos = tex.nOffsets[0];
		lump.size = tex.nWidth * tex.nHeight + 16;
		memcpy(&lump.name, tex.szName, 16);
		lump.disksize = 8 + tex.nWidth * tex.nHeight * 1;
		pak->fileindex[WString(std::string(lump.name))] = pak->lumps.size();
		pak->lumps.push_back(lump);

		BSPTexture btex = {};
		btex.width = tex.nWidth;
		btex.name = std::string(tex.szName);
		btex.height = tex.nHeight;
		btex.offset = tex.nOffsets[0] + BSPMIPTEXOFFSET + lumpoffset;
		btex.size = tex.nWidth * tex.nHeight + 8;
		pak->textures.push_back(btex);

		reader.Seek(BSPMIPTEXOFFSET + lumpoffset + tex.nOffsets[0]);
		char c[4];
		reader.Read(c, 4);
		int d = 3;
		//tex.nOffsets[0];
	}
	return pak;
}*/
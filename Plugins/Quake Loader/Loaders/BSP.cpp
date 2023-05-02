#include "BSP.h"

enum BSPLumpId
{
	LUMP_ENTITIES = 0,
	LUMP_PLANES = 1,
	LUMP_TEXTURES = 2,
	LUMP_VERTICES = 3,
	LUMP_VISIBILITY = 4,
	LUMP_NODES = 5,
	LUMP_TEXINFO = 6,
	LUMP_FACES = 7,
	LUMP_LIGHTING = 8,
	LUMP_CLIPNODES = 9,
	LUMP_LEAVES = 10,
	LUMP_MARKSURFACES = 11,
	LUMP_EDGES = 12,
	LUMP_SURFEDGES = 13,
	LUMP_MODELS = 14,
	HEADER_LUMPS = 15
};

const float QUAKE_UNIT_CONVERSION = 1.0f / 32.0f;

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

void* LoadModelBSP(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	uint32_t lumpoffset, lumpsize;
	uint32_t nMipTextures;
	int BSPMIPTEXOFFSET;
	int version = 0;

	//Read header
	auto reader = MemReader(data, size);
	reader.Read(&version);
	if (version != 29)
	{
		return NULL;
	}

	//Read vertices
	reader.Seek(4 + LUMP_VERTICES * 8);
	reader.Read(&lumpoffset);
	reader.Read(&lumpsize);
	reader.Seek(lumpoffset);
	int vertex_count = lumpsize / 12;
	if (vertex_count == 0) return NULL;
	if (vertex_count * 12 != lumpsize) return NULL;
	std::vector<GMFVec3> vertices;
	vertices.resize(vertex_count);
	GMFVec3 vertex;
	for (int v = 0; v < vertex_count; ++v)
	{
		reader.Read(&vertex, 12);
		vertices[v].x = vertex.x * QUAKE_UNIT_CONVERSION;
		vertices[v].z = vertex.y * QUAKE_UNIT_CONVERSION;
		vertices[v].y = vertex.z * QUAKE_UNIT_CONVERSION;
	}

	//Read edges
	reader.Seek(4 + LUMP_EDGES * 8);
	reader.Read(&lumpoffset);
	reader.Read(&lumpsize);
	reader.Seek(lumpoffset);
	std::vector<edge_t> edges;
	auto sz = sizeof(edge_t);
	int countedges = lumpsize / sz;
	edges.resize(countedges);
	for (int n = 0; n < countedges; ++n)
	{
		reader.Read(&edges[n]);
	}

	//Read edge indexes
	reader.Seek(4 + LUMP_SURFEDGES * 8);
	reader.Read(&lumpoffset);
	reader.Read(&lumpsize);
	reader.Seek(lumpoffset);
	std::vector<int> edgeindexes;
	edgeindexes.resize(lumpsize / 4);
	reader.Read(edgeindexes.data(), edgeindexes.size() * 4);

	//Read texture info
	reader.Seek(4 + LUMP_TEXINFO * 8);
	reader.Read(&lumpoffset);
	reader.Read(&lumpsize);
	reader.Seek(lumpoffset);
	std::vector<surface_t> texinfo;
	texinfo.resize(lumpsize / sizeof(surface_t));
	reader.Read(texinfo.data(), lumpsize);

	//Read planes
	reader.Seek(4 + LUMP_PLANES * 8);
	reader.Read(&lumpoffset);
	reader.Read(&lumpsize);
	reader.Seek(lumpoffset);
	std::vector<plane_t> planes;
	planes.resize(lumpsize / sizeof(plane_t));
	reader.Read(planes.data(), planes.size() * sizeof(plane_t));

	//Create model in memory
	auto file = new GMFFile;
	auto root = new GMFNode(file, GMF_TYPE_MODEL);
	auto lod = root->AddLOD();

	//Read textures and construct materials
	reader.Seek(4 + LUMP_TEXTURES * 8);
	reader.Read(&lumpoffset);
	reader.Read(&lumpsize);
	reader.Seek(lumpoffset);
	uint32_t texcount;
	reader.Read(&texcount);
	std::vector<miptex_t> textures;
	std::vector<int> texturedatapos(texcount);
	for (int n = 0; n < texcount; ++n)
	{
		reader.Read(&texturedatapos[n]);
	}
	textures.resize(texcount);
	for (int n = 0; n < texcount; ++n)
	{
		reader.Seek(lumpoffset + texturedatapos[n]);
		reader.Read(&textures[n]);
		int f = 2;
	}
	std::vector<GMFMaterial*> materials(texcount);
	int texindex = 0;
	for (int n = 0; n < texcount; ++n)
	{
		if (textures[n].offset1 == -1) continue;
		TextureInfo texinfo;
		texinfo.width = textures[n].width;
		texinfo.height = textures[n].height;
		texinfo.filter = 0;
		texinfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		texinfo.mipchain.resize(1);
		texinfo.mipchain[0].width = texinfo.width;
		texinfo.mipchain[0].height = texinfo.height;
		texinfo.mipchain[0].size = texinfo.width * texinfo.height * 4;
		reader.Seek(lumpoffset + texturedatapos[n] + textures[n].offset1);
		auto data = malloc(texinfo.mipchain[0].size);
		unsigned char rgba[4];
		for (int x = 0; x < texinfo.mipchain[0].width; ++x)
		{
			for (int y = 0; y < texinfo.mipchain[0].height; ++y)
			{
				unsigned char index;
				reader.Read(&index);
				auto color = qpallete[index];
				memcpy(&rgba[0], &color, 4);
				rgba[3] = 255;
				auto ptr = (const char*)data;
				ptr += uint64_t((y * texinfo.mipchain[0].width + x) * 4);
				memcpy((void*)(ptr), &rgba[0], 4);
			}
		}
		texinfo.mipchain[0].data = data;
		context->memblocks.push_back(data);
		file->AddTexture(texinfo);
		materials[n] = new GMFMaterial(file);
		materials[n]->text = "{\"material\":{"
			"\"texture0\": " + String(texindex) + "}}";
		materials[n]->data = (void*)materials[n]->text.c_str();
		materials[n]->datasize = materials[n]->text.size();
		++texindex;
	}

	//Read faces
	std::map<int, GMFMesh*> meshes;
	reader.Seek(4 + LUMP_FACES * 8);
	reader.Read(&lumpoffset);
	reader.Read(&lumpsize);
	reader.Seek(lumpoffset);
	face_t face;
	bool flipped;
	short edgeID;
	int countfaces = lumpsize / sizeof(face);
	for (int n = 0; n < countfaces; ++n)
	{
		reader.Read(&face);
		int start = face.ledge_id;

		edgeID = edgeindexes[start];
		flipped = false;
		if (edgeID < 0)
		{
			edgeID *= -1;
			flipped = true;
		}
		int a = edges[edgeID].vertex0;

		int texid = texinfo[face.texinfo_id].texture_id;

		auto mesh = meshes[texid];
		if (mesh == NULL)
		{
			mesh = new GMFMesh(file);
			lod->AddMesh(mesh);
			meshes[texid] = mesh;
			mesh->SetMaterial(materials[texid]);
		}

		float nx = planes[face.plane_id].normal.x;
		float ny = planes[face.plane_id].normal.z;
		float nz = planes[face.plane_id].normal.y;

		for (int k = 1; k < face.ledge_num; ++k)
		{
			edgeID = edgeindexes[start + k];
			flipped = false;
			if (edgeID < 0)
			{
				edgeID *= -1;
				flipped = true;
			}
			int b = edges[edgeID].vertex0;
			int c = edges[edgeID].vertex1;

			float u0, u1, u2, v0, v1, v2;

			v0 = texinfo[face.texinfo_id].vectorS.x * vertices[a].x + texinfo[face.texinfo_id].vectorS.z * vertices[a].y + texinfo[face.texinfo_id].vectorS.y * vertices[a].z + texinfo[face.texinfo_id].distS;
			u0 = texinfo[face.texinfo_id].vectorT.x * vertices[a].x + texinfo[face.texinfo_id].vectorT.z * vertices[a].y + texinfo[face.texinfo_id].vectorT.y * vertices[a].z + texinfo[face.texinfo_id].distT;

			v1 = texinfo[face.texinfo_id].vectorS.x * vertices[b].x + texinfo[face.texinfo_id].vectorS.z * vertices[b].y + texinfo[face.texinfo_id].vectorS.y * vertices[b].z + texinfo[face.texinfo_id].distS;
			u1 = texinfo[face.texinfo_id].vectorT.x * vertices[b].x + texinfo[face.texinfo_id].vectorT.z * vertices[b].y + texinfo[face.texinfo_id].vectorT.y * vertices[b].z + texinfo[face.texinfo_id].distT;

			v2 = texinfo[face.texinfo_id].vectorS.x * vertices[c].x + texinfo[face.texinfo_id].vectorS.z * vertices[c].y + texinfo[face.texinfo_id].vectorS.y * vertices[c].z + texinfo[face.texinfo_id].distS;
			u2 = texinfo[face.texinfo_id].vectorT.x * vertices[c].x + texinfo[face.texinfo_id].vectorT.z * vertices[c].y + texinfo[face.texinfo_id].vectorT.y * vertices[c].z + texinfo[face.texinfo_id].distT;

			int va = mesh->AddVertex(vertices[a].x, vertices[a].y, vertices[a].z, nx, ny, nz, u0, v0);
			int vb = mesh->AddVertex(vertices[b].x, vertices[b].y, vertices[b].z, nx, ny, nz, u1, v1);
			int vc = mesh->AddVertex(vertices[c].x, vertices[c].y, vertices[c].z, nx, ny, nz, u2, v2);
			if (flipped)
			{
				mesh->AddPolygon(vc, vb, va);
			}
			else
			{
				mesh->AddPolygon(va, vb, vc);
			}
		}
	}

	//Save model to transfer format
	int flags = 0;
	file->Save(&context->writer, flags);
	delete file;
	returnsize = context->writer.Size();
	return context->writer.data();
}
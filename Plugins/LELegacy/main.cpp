#include "DLLExports.h"
#include "VKFormat.h"
#include <algorithm>
#include <vector>
#include "s3tc.h"
#include "stb_dxt.h"

using namespace GMFSDK;
using namespace std;

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
	std::string s =
		"{"
		"\"plugin\":{"
		"\"title\":\"Leadwerks Engine File Formats\","
		"\"description\":\"Load Leadwerks Engine models, materials, and textures.\","
		"\"author\":\"Josh Klint\","
		"\"threadSafe\":true,"
		"\"loadModelExtensions\": [{\"extension\":\"mdl,gmf\",\"description\":\"Leadwerks Model\"}],"
		"\"loadTextureExtensions\": [{\"extension\":\"tex\",\"description\":\"Leadwerks Texture\"}],"
		"\"loadMaterialExtensions\": [{\"extension\":\"mat\",\"description\":\"Leadwerks Material\"}],"
		"\"loadSceneExtensions\": [{\"extension\":\"map\",\"description\":\"Leadwerks Map\"}]"
		"}"
		"}";
	if (s.length() < maxsize) maxsize = s.length();
	if (maxsize > 0) memcpy(cs, s.c_str(), maxsize);
	size_t maxsize_ = maxsize;
	return std::min(s.length(), maxsize_);
}

Context* CreateContext()
{
	return new Context;
}

void FreeContext(Context* ctx)
{
	for (auto mem : ctx->allocedmem)
	{
		free(mem);
	}
	delete ctx;
}

const int LE_TEXTURE_RGBA8 = 1;
const int LE_TEXTURE_RGB8 = 7;
const int LE_TEXTURE_RGBADXTC1 = 4;
const int LE_TEXTURE_RGBDXTC1 = 8;
const int LE_TEXTURE_RGBADXTC3 = 5;
const int LE_TEXTURE_RGBADXTC5 = 6;
const int LE_TEXTURE_RGBADXT5N = 20;
const int LE_TEXTURE_1D = 1;
const int LE_TEXTURE_2D = 2;
const int LE_TEXTURE_3D = 3;
const int LE_TEXTURE_CUBEMAP = 4;

int texcoordset;

bool LoadChunk(MemReader& reader, Chunk* chunk, GMFSDK::GMFFile* file, GMFSDK::GMFNode* parent, GMFSDK::GMFMesh* mesh)
{
	auto node = parent;
	reader.Seek(chunk->pos);
	switch (chunk->id)
	{
	case GMF_MESH:
	case GMF_NODE:
	{
		node = new GMFSDK::GMFNode(file, GMFSDK::GMF_TYPE_MODEL);
		if (parent) node->SetParent(parent);
		reader.Seek(chunk->pos);
		float mat[16];
		reader.Read(&mat[0], 64);
		node->SetPosition(mat[12], mat[13], mat[14]);
	//	node->SetColor(1.0f - mat[0], color[2], color[2], color[3]);
	}
	break;
	case GMF_BONE:
	{
		node = new GMFSDK::GMFNode(file, GMFSDK::GMF_TYPE_MODEL);
		if (parent) node->SetParent(parent);
		reader.Seek(chunk->pos);
		float mat[16];
		reader.Read(&mat[0], 64);
		int boneindex;
		reader.Read(&boneindex);
	}
	break;
	case GMF_SURFACE:
	{
		texcoordset = 0;
		mesh = new GMFSDK::GMFMesh(file, 3);
		if (node->lods.empty()) node->AddLOD();
		node->lods[0]->AddMesh(mesh);
	}
	break;
	case GMF_PROPERTIES:
	{
		int count;
		reader.Read(&count);
		for (int n = 0; n < count; ++n)
		{
			auto key = reader.ReadLine();
			auto value = reader.ReadLine();
			if (node != NULL and key == "name")
			{
				node->name = WString(value);
			}
			if (mesh != NULL and key == "material")
			{
				auto path = WString(value);
				for (int n = 0; n < file->materials.size(); ++n)
				{
					if (file->materials[n]->path == path)
					{
						mesh->material = file->materials[n]->index;
						break;
					}
				}
				if (mesh->material == 0)
				{
					auto mtl = new GMFSDK::GMFMaterial(file);
					mtl->path = WString(value);
					mesh->material = mtl->index;
				}
			}
		}
	}
	break;
	case GMF_INDICEARRAY:
	{
		{
			int indice_count, mode, type;
			reader.Read(&indice_count);
			reader.Read(&mode);
			reader.Read(&type);
			mesh->indices.resize(indice_count);
			if (type == GMF_UNSIGNED_INT)
			{
				reader.Read(mesh->indices.data(), indice_count * 4);
			}
			else if (type == GMF_UNSIGNED_SHORT)
			{
				unsigned short s;
				for (int n = 0; n < indice_count; ++n)
				{
					reader.Read(&s);
					mesh->indices[n] = s;
				}
			}
			else
			{
				return false;
			}
		}
		break;
	case GMF_VERTEXARRAY:
	{
		int vert_count, data_type, type, elements, size;
		reader.Read(&vert_count);
		reader.Read(&data_type);
		reader.Read(&type);
		reader.Read(&elements);
		size = vert_count * elements;
		switch (type)
		{
		case GMF_UNSIGNED_BYTE:
			break;
		case GMF_UNSIGNED_SHORT:
			size *= 2;
			break;
		case GMF_UNSIGNED_INT:
		case GMF_FLOAT:
			size *= 4;
			break;
		case GMF_DOUBLE:
			size *= 8;
			break;
		}
		float vector[4];
		mesh->vertices.resize(vert_count);
		switch (data_type)
		{
		case GMF_POSITION:
			if (elements != 3 or size != vert_count * 12)
			{
				return false;
			}
			for (int n = 0; n < vert_count; ++n)
			{
				reader.Read(&mesh->vertices[n], 12);
			}
			break;
		case GMF_NORMAL:
			if (elements != 3 or size != vert_count * 12)
			{
				return false;
			}
			for (int n = 0; n < vert_count; ++n)
			{
				reader.Read(&vector[0], 12);
				mesh->vertices[n].normal[0] = int(vector[0] * 127.0f);
				mesh->vertices[n].normal[1] = int(vector[1] * 127.0f);
				mesh->vertices[n].normal[2] = int(vector[2] * 127.0f);
			}
			break;
		case GMF_TEXTURE_COORD:
			texcoordset++;
			if (texcoordset == 1)
			{
				if (elements != 2 or size != vert_count * 8)
				{
					return false;
				}
				for (int n = 0; n < vert_count; ++n)
				{
					reader.Read(&vector[0], 8);
					mesh->vertices[n].texcoords[0] = floatToHalf(vector[0]);
					mesh->vertices[n].texcoords[1] = floatToHalf(vector[1]);
				}
			}
			break;
		}
	}
	break;
	}
	}
	reader.Seek(chunk->pos + chunk->size);
	for (auto subchunk : chunk->kids)
	{
		if (!LoadChunk(reader, subchunk, file, node, mesh)) return false;
	}
	if (chunk->id == GMF_SURFACE)
	{
		mesh->UpdateTangents();
	}
	return true;
}

void* LoadModel(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	if (size < 6) return NULL;
	MemReader reader(data, size);
	int flags = 0;

	int tag;
	reader.Read(&tag);
	if (tag != GMF_FILE) return NULL;
	reader.Seek(0);

	Chunk chunk;
	chunk.Read(reader);
	if (chunk.id != GMF_FILE) return NULL;
	reader.Seek(chunk.pos);
	int version;
	reader.Read(&version);
	if (version != 1 and version != 2) return NULL;
	reader.Seek(chunk.pos + chunk.size);

	GMFSDK::GMFFile file;

	auto root = new GMFSDK::GMFNode(&file, GMFSDK::GMF_TYPE_MODEL);
	for (auto chunk : chunk.kids)
	{
		if (!LoadChunk(reader, chunk, &file, NULL, NULL)) return NULL;
	}
	
	//Write file to memory
	file.Save(&context->writer, flags);
	returnsize = context->writer.Size();
	return context->writer.data();
}

void* LoadMaterial(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	if (size < 25) return NULL;

	MemReader reader(data, size);
	//if (Lower(reader.ReadString(25)) != "//leadwerks material file") return NULL;

	std::wstring path = std::wstring(cpath);
	if (!path.empty())
	{
		if (Lower(ExtractExt(path)) != L"mat") return NULL;
	}
	//auto s = Lower(reader.ReadLine());
	//if (s.find("//leadwerks material file") == -1) return NULL;

	nlohmann::json j3;
	j3["material"] = nlohmann::json::object();
	j3["material"]["textures"] = nlohmann::json::array();

	for (int n = 0; n < 16; ++n)
	{
		j3["material"]["textures"].push_back(nlohmann::json::object());
	}

	bool ok = false;
	while (reader.Pos() < reader.Size())
	{
		auto s = reader.ReadLine();
		if (Lower(s) == "//leadwerks material file") ok = true;
		auto p = Find(s, "//");
		if (p != -1) s = Right(s, s.size() - p - 1);
		p = Find(s, "=");
		if (p != -1)
		{
			auto key = Left(s, p);
			auto value = Right(s, s.size() - p - 1);
			if (key == "texture0")
			{
				ok = true;
				j3["material"]["textures"][0]["file"] = Replace(value,"\"","");
			}
			else if (key == "texture1")
			{
				ok = true;
				j3["material"]["textures"][1]["file"] = Replace(value, "\"", "");
			}
		}
	}
	if (!ok) return NULL;
	context->materialinfo = j3.dump(1, '	');
	returnsize = context->materialinfo.size();
	return (void*)context->materialinfo.c_str();
}

void* LoadTexture(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	if (size < 8) return NULL;

	MemReader reader(data, size);
	int tag;
	reader.Read(&tag,4);
	if (tag != 5784916) return nullptr; // "TEX"

	int version;
	reader.Read(&version);
	if (version != 1) return nullptr;

	TextureInfo texinfo;
	texinfo.version = 201;

	int format;
	reader.Read(&format);

	switch (format)
	{
	case LE_TEXTURE_RGB8:
		texinfo.format = VK_FORMAT_R8G8B8_UNORM;
		break;
	case LE_TEXTURE_RGBA8:
		texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case LE_TEXTURE_RGBDXTC1:
		texinfo.format = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		break;
	case LE_TEXTURE_RGBADXTC1:
		texinfo.format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		break;
	case LE_TEXTURE_RGBADXTC3:
		texinfo.format = VK_FORMAT_BC2_UNORM_BLOCK;
		break;
	case LE_TEXTURE_RGBADXT5N:
		//texinfo.format = VK_FORMAT_BC5_SNORM_BLOCK;
		texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case LE_TEXTURE_RGBADXTC5:
		texinfo.format = VK_FORMAT_BC3_UNORM_BLOCK;
		break;
	default:
		printf("Unknown texture format.");
		return nullptr;
		break;
	}

	int target;
	reader.Read(&target);

	if (target == LE_TEXTURE_1D)
	{
		texinfo.target = 1;
	}
	else if (target == LE_TEXTURE_2D)
	{
		texinfo.target = 2;
	}
	else if (target == LE_TEXTURE_3D)
	{
		texinfo.target = 3;
	}
	else if (target == LE_TEXTURE_CUBEMAP)
	{
		texinfo.target = 4;
		texinfo.faces = 6;
	}
	else
	{
		printf("Unknown texture target.");
		return nullptr;
	}

	reader.Read(&texinfo.width);
	reader.Read(&texinfo.height);
	reader.Read(&texinfo.depth);

	reader.Read(&texinfo.filter);
	reader.Read(&texinfo.clampu);
	reader.Read(&texinfo.clampv);
	reader.Read(&texinfo.clampw);

	reader.Read(&texinfo.frames);
	reader.Read(&texinfo.mipmaps);

	context->writer.Write(&texinfo);

	int mw, mh, sz;
	//for (int k = 0; k < texinfo.frames; ++k)// not ever used / supported
	//{
		for (int i = 0; i < texinfo.mipmaps; i++)
		{
			reader.Read(&mw);
			reader.Read(&mh);
			reader.Read(&sz);
			for (int f = 0; f < texinfo.faces; ++f)
			{
				void* memblock = malloc(sz);
				if (memblock == nullptr)
				{
					printf("Error: Failed to allocate memory of size %i.\n", sz);
					return nullptr;
				}
				
				reader.Read(memblock, sz);

				if (format == LE_TEXTURE_RGBADXT5N)
				{
					//Swap red and alpha channels
					int rw = max(4, mw);
					int rh = max(4, mh);
					auto memblock2 = malloc(rw*rh*4);
					BlockDecompressImageDXT5(mw, mh, (const unsigned char*)memblock, (unsigned long*)memblock2);
					free(memblock);
					memblock = memblock2;
					unsigned char rgba[4];
					int r, g, b, a;
					for (int x = 0; x < mw; ++x)
					{
						for (int y = 0; y < mh; ++y)
						{
							memcpy(&rgba[0], ((char*)memblock) + (mw * y + x) * 4, 4);
							a = rgba[3];
							rgba[3] = rgba[0];
							rgba[0] = a;
							memcpy(((char*)memblock) + (mw * y + x) * 4, &rgba[0], 4);
						}
					}
					sz = mw * mh * 4;
				}
				context->allocedmem.push_back(memblock);
				context->writer.Write(&sz);
				context->writer.Write(&memblock, sizeof(void*));
			}
		}
	//}

	returnsize = context->writer.Size();
	return context->writer.data();
}
#include "DLLExports.h"

using namespace GMFSDK;

//DLL entry point function
#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	return TRUE;
}
#else
int main(int argc, const char* argv[])
{
	return 0;
}
#endif

#ifdef _WIN32
#define FUNC_PROTOCOL __cdecl
#else
#define FUNC_PROTOCOL
#endif

void* (FUNC_PROTOCOL* FP_LOADBUFFER)(wchar_t*, void*) = NULL;
void (FUNC_PROTOCOL* FP_FREEBUFFER)(void*) = NULL;

void SetFunctionPointer(wchar_t* cname, void* ptr)
{
	auto name = std::wstring(cname);
	if (name == L"LoadBuffer")
	{
		FP_LOADBUFFER = (void* (*)(wchar_t*, void*))ptr;
	}
	else if (name == L"FreeBuffer")
	{
		FP_FREEBUFFER = (void(*)(void*))ptr;
	}
}

void* LoadBuffer(const std::wstring& path, uint64_t& sz)
{
	if (FP_LOADBUFFER == NULL) return NULL;
	return FP_LOADBUFFER((wchar_t*)path.c_str(), (void*)&sz);
}

void FreeBuffer(void* mem)
{
	if (FP_FREEBUFFER != NULL) FP_FREEBUFFER(mem);
}

Context* CreateContext()
{
	return new Context;
}

void FreeContext(Context* context)
{
	delete context;
}

//Returns all plugin information in a JSON string
int GetPluginInfo(unsigned char* cs, int maxsize)
{
	std::string s =
		"{\"plugin\":{"
		"\"title\":\"Valve Model Loader\","
		"\"description\":\"Load Source Engine model files.\","
		"\"author\":\"Josh Klint\","
		"\"threadSafe\":true,"
		"\"loadModelExtensions\":[\"mdl\"]"
		"}}";
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize, s.length()));
	return s.length();
}

//Model load function
void* LoadModel(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out)
{
	//Reader object
	MemReader reader(data, size);
	
	studiohdr_t header = {};
	if (size < sizeof(header)) return NULL;

	reader.Read(&header, sizeof(header));

	if (header.id != 1414743113) return NULL;
	//if (header.version != 44) return NULL;

	std::wstring path = std::wstring(cpath);
	uint64_t sz = 0;

	//------------------------------------------------------------------------
	// Load VVD file
	//------------------------------------------------------------------------

	const int lod = 0;
	std::vector<mstudiovertex_t> vertices;

	std::wstring vvdpath = StripExt(path) + L".vvd";
	auto buffer = LoadBuffer(vvdpath, sz);
	if (buffer == NULL) return NULL;

	vertexFileHeader_t vvdheader = {};
	MemReader vvdreader(buffer, sz);
	if (vvdreader.Size() < sizeof(vvdheader))
	{
		FreeBuffer(buffer);
		return NULL;
	}
	vvdreader.Read(&vvdheader);
	if (vvdheader.id != 1448297545)
	{
		FreeBuffer(buffer);
		return NULL;
	}
	vertexFileFixup_t lodfixup = {};
	vertexFileFixup_t fixup = {};
	if (vvdheader.numFixups != 0)
	{
		vvdreader.Seek(vvdheader.fixupTableStart);
		for (int n = 0; n < vvdheader.numFixups; ++n)
		{
			vvdreader.Read(&fixup);
			if (fixup.lod == lod) lodfixup = fixup;
		}
	}
	vvdreader.Seek(vvdheader.vertexDataStart);
	vertices.resize(vvdheader.numLODVertexes[lod]);
	for (int v = 0; v < vvdheader.numLODVertexes[lod]; ++v)
	{
		vvdreader.Read(&vertices[v]);
	}

	FreeBuffer(buffer);

	//------------------------------------------------------------------------
	// Load VTX file
	//------------------------------------------------------------------------

	std::vector<unsigned short> indices;

	std::wstring vxtpath = StripExt(path) + L".dx90.vtx";
	buffer = LoadBuffer(vxtpath, sz);
	if (buffer == NULL)
	{
		vxtpath = StripExt(path) + L".vtx";
		buffer = LoadBuffer(vxtpath, sz);
	}
	if (buffer == NULL) return NULL;

	MemReader vxtreader(buffer, sz);

	vtxHeader_t vxtheader = {};
	if (vxtreader.Size() < sizeof(vxtheader))
	{
		FreeBuffer(buffer);
		return NULL;
	}
	vxtreader.Read(&vxtheader);

	std::vector<mstudiovertex_t> meshvertices;
	for (int n = 0; n < vxtheader.numBodyParts; ++n)
	{
		auto offset = vxtheader.bodyPartOffset + n * sizeof(vtxBodyPartHeader_t);
		vxtreader.Seek(offset);
		vtxBodyPartHeader_t header = {};
		vxtreader.Read(&header);
		if (header.numModels == 0) continue;
		vxtreader.Seek(offset + header.modelOffset);
		for (int n2 = 0; n2 < header.numModels; ++n2)
		{
			auto modeloffset = offset + header.modelOffset + n2 * sizeof(vtzModelHeader_t);
			vxtreader.Seek(modeloffset);
			vtzModelHeader_t modelheader = {};
			vxtreader.Read(&modelheader);
			if (modelheader.numLODs == 0) continue;
			vxtreader.Seek(modeloffset + modelheader.lodOffset);
			for (int n3 = 0; n3 < min(1,modelheader.numLODs); n3++)
			{
				auto lodoffset = modeloffset + modelheader.lodOffset + n3 * sizeof(vtxModelLODHeader_t);
				vxtreader.Seek(lodoffset);
				vtxModelLODHeader_t lodheader = {};
				vxtreader.Read(&lodheader);
				for (int n4 = 0; n4 < lodheader.numMeshes; n4++)
				{
					auto meshoffset = lodoffset + lodheader.meshOffset + n4 * sizeof(MeshHeader_t);
					vxtreader.Seek(meshoffset);
					MeshHeader_t meshheader = {};
					vxtreader.Read(&meshheader);
					for (int n5 = 0; n5 < meshheader.numStripGroups; ++n5)
					{
						auto stripgroupoffset = meshoffset + meshheader.stripGroupHeaderOffset + n5 * sizeof(StripGroupHeader_t);
						vxtreader.Seek(stripgroupoffset);
						StripGroupHeader_t stripgroup;
						vxtreader.Read(&stripgroup);
						vxtreader.Seek(stripgroupoffset + stripgroup.indexOffset);
						auto ic = indices.size();
						indices.resize(ic + stripgroup.numIndices);
						vxtreader.Read(&indices[ic], sizeof(unsigned short) * stripgroup.numIndices);
						goto out; // let's keep it simple for testing...
					}
				}
			}
		}
	}
	
	out:
	FreeBuffer(buffer);

	//------------------------------------------------------------------------
	// Load MDL file
	//------------------------------------------------------------------------

	for (int n = 0; n < header.bodypart_count; ++n)
	{
		auto offset = header.bodypart_offset + n * 16;
		reader.Seek(offset);
		int nummodels, base, modelIndex, strpos;
		reader.Read<int>(&strpos);
		auto pos = reader.Pos();
		reader.Seek(offset + strpos);
		auto name = reader.ReadString();
		reader.Seek(pos);
		reader.Read<int>(&nummodels);
		reader.Read<int>(&base);
		reader.Read<int>(&modelIndex);
		for (int i = 0; i < nummodels; ++i)
		{
			auto offset2 = offset + modelIndex + i * 148;
			auto pos = reader.Pos();
			auto mdlname = reader.ReadString(64);
			reader.Seek(pos + 64);
			int type, numMeshes, meshIndex, numVertices, vertexIndex, tangentsIndex;
			int numattachments, attachmentsIndex, numeyeballs, eyeballindex;
			float radius;
			reader.Read<int>(&type);
			reader.Read<float>(&radius);
			reader.Read<int>(&numMeshes);
			reader.Read<int>(&meshIndex);
			reader.Read<int>(&numVertices);
			reader.Read<int>(&vertexIndex);
			reader.Read<int>(&tangentsIndex);
			reader.Read<int>(&numattachments);
			reader.Read<int>(&attachmentsIndex);
			reader.Read<int>(&numeyeballs);
			reader.Read<int>(&eyeballindex);
			for (int m = 0; m < numMeshes; ++m)
			{
				auto meshoffset = offset2 + meshIndex + m * 116;
				int material, modelIndex, numVertices, verticesOffset;
				int numflexes, flexindex, materialtype, materialparam, meshid;
				Vector center;

				reader.Seek(meshoffset);
				reader.Read<int>(&material);
				reader.Read<int>(&modelIndex);
				reader.Read<int>(&numVertices);
				reader.Read<int>(&verticesOffset);

				reader.Read<int>(&numflexes);
				reader.Read<int>(&flexindex);
				reader.Read<int>(&materialtype);
				reader.Read<int>(&materialparam);
				reader.Read<int>(&meshid);
				reader.Read(&center,12);
			}
		}
	}
	
	//------------------------------------------------------------------------
	// Construct the data structure to return
	//------------------------------------------------------------------------	
	
	auto file = new GMFFile();
	auto root = new GMFNode(file, GMF_TYPE_MODEL);
	auto glod = root->AddLOD();
	auto mesh = new GMFMesh(file, 3);
	for (int v = 0; v < vertices.size(); ++v)
	{
		mesh->AddVertex(vertices[v].m_vecPosition.x, vertices[v].m_vecPosition.y, vertices[v].m_vecPosition.z, vertices[v].m_vecNormal.x, vertices[v].m_vecNormal.y, vertices[v].m_vecNormal.z, vertices[v].m_vecTexCoord.x, vertices[v].m_vecTexCoord.y);
	}
	mesh->indices.resize(indices.size());
	for (int i = 0; i < indices.size(); ++i)
	{
		mesh->indices[i] = indices[i];
		//mesh->AddPolygon(indices[i * 3 + 0], indices[i * 3 + 1], indices[i * 3 + 2]);
	}
	glod->AddMesh(mesh);

	//------------------------------------------------------------------------
	// Write model data to transfer format in memory
	//------------------------------------------------------------------------

	if (!file->Save(&context->writer)) return NULL;
	size_out = context->writer.Size();
	return context->writer.data();
}

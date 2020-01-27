#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#include "../SDK/GMFSDK.h"
#include "MD3Structs.h"
#include "DLLExports.h"
#include <map>

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

//Returns all plugin information in a JSON string
int GetPluginInfo(unsigned char* cs, int maxsize)
{
	std::string s =
	"{"
		"\"turboPlugin\":{"
			"\"title\":\"Quake 3 Model Loader.\","
			"\"description\":\"Load Quake 3 Arena files from the MD3 file format.\","
			"\"author\":\"Josh Klint\","
			"\"url\":\"www.turboengine.com\","
			"\"extension\":\"md3\","
			"\"filter\":\"Quake 3 Model (*.md3):md3\""
		"}"
	"}";

	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
	return s.length();
}

//Constants
#define Q3A_SCALE (0.0254)
#define MD3_XYZ_SCALE (1.0 / 64.0)
#define MAX_QPATH 64
#define MD3_MAGIC 860898377
#define MD3_VERSION 15

MemWriter* writer = nullptr;

//Model load function
void* LoadModel(void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out)
{
	//Reader object
	MemReader reader(data, size);
	
	//Get the file path (used for producing the texture name)
	std::wstring path = L"";
	if (cpath != nullptr) path = std::wstring(cpath);
	
	//Read the file header
	tMd3Header header;
	reader.Read(&header);

	//Check magic number and version
	int fileid;
	memcpy(&fileid, &header.fileID[0], 4);
	if (fileid != MD3_MAGIC) return nullptr;
	if (header.version != MD3_VERSION) return nullptr;
	
	//File info object and root entity
	auto file = new GMFFile;
	auto root = new GMFNode(file, GMF_TYPE_MODEL);
	
	//Create default material - sometimes these are in JPG format, sometimes in TGA(!)
	std::map<std::wstring, GMFMaterial*> materials;
	
	char cs[MAX_QPATH];
	float mat[16];

#ifdef DEBUG
	printf("\nTags\n");
#endif
	//Read tags
	reader.Seek(header.tagStart);
	for (int n = 0; n < header.numTags; ++n)
	{		
		reader.Read(&cs[0], MAX_QPATH);
		std::string name = std::string(cs);
		
#ifdef DEBUG
		printf(name.c_str());
		printf("\n");
#endif

		reader.Read(&mat[12]);
		reader.Read(&mat[14]);
		reader.Read(&mat[13]);
		mat[12] *= Q3A_SCALE;
		mat[13] *= Q3A_SCALE;
		mat[14] *= Q3A_SCALE;
		mat[15] = 1;

		reader.Read(&mat[0]);
		reader.Read(&mat[1]);
		reader.Read(&mat[2]);
		mat[3] = 0;

		reader.Read(&mat[8]);
		reader.Read(&mat[9]);
		reader.Read(&mat[10]);
		mat[11] = 0;

		reader.Read(&mat[4]);
		reader.Read(&mat[5]);
		reader.Read(&mat[6]);
		mat[7] = 0;

		auto tag = new GMFNode(file, GMF_TYPE_PIVOT);
		tag->SetParent(root);
		tag->name = WString(name);
		tag->SetMatrix(&mat[0]);
	}

#ifdef DEBUG
	printf("\nLimbs\n");
#endif
	//Read limbs
	tMd3MeshInfo mesh;
	reader.Seek(header.tagEnd);
	for (int n = 0; n < header.numMeshes; ++n)
	{
		int meshstart = reader.Pos();
		reader.Read(&mesh);

		std::string name = std::string(mesh.strName);
		
#ifdef DEBUG
		printf("\n");
		printf(mesh.strName);
		printf("\n");
#endif

		GMFNode* limb = NULL;
		if (header.numMeshes == 1)
		{
			limb = root;
		}
		else
		{
			limb = new GMFNode(file, GMF_TYPE_MODEL);
			limb->SetParent(root);
		}
		limb->name = WString(name);
		auto lod = limb->AddLOD();

		auto surf = new GMFMesh(file, 3);

		lod->AddMesh(surf);
		
#ifdef DEBUG
		printf("\nTriangles:\n");
#endif
		//Read indices
		surf->indices.resize(mesh.numTriangles * 3);
		reader.Seek(meshstart + mesh.triStart);
		int a, b, c;
		for (int t = 0; t < mesh.numTriangles; ++t)
		{
			reader.Read(&a);
			reader.Read(&b);
			reader.Read(&c);
			
#ifdef DEBUG
			printf("%i, %i, %i\n", a, b, c);
#endif

			surf->indices[t * 3 + 0] = a;
			surf->indices[t * 3 + 1] = b;
			surf->indices[t * 3 + 2] = c;
		}

#ifdef DEBUG
		printf("\nVertices:\n");
#endif
		//Read vertex positions and normals
		surf->vertices.resize(mesh.numVertices);
		reader.Seek(meshstart + mesh.vertexStart);
		float x, y, z, nx, ny, nz;
		short cx, cy, cz, norm;
		float unpackAngle = 360.0 / 255.0;
		float longitude, latitude;

		for (int v = 0; v < mesh.numVertices; ++v)
		{			
			reader.Read(&cx);
			reader.Read(&cz);
			reader.Read(&cy);

			//Decompress MD3 vertex positions
			x = double(cx) * MD3_XYZ_SCALE * Q3A_SCALE;
			y = double(cy) * MD3_XYZ_SCALE * Q3A_SCALE;
			z = double(cz) * MD3_XYZ_SCALE * Q3A_SCALE;
			
			//Decompress MD3 normals
			reader.Read(&norm);
			
			longitude = ((float(norm) / 256.0)) * unpackAngle;
			latitude = (float(norm & 0xff)) * unpackAngle;
			nx = cos(longitude) * sin(latitude);
			ny = sin(longitude) * sin(latitude);
			nz = cos(latitude);

#ifdef DEBUG
			printf("%f, %f, %f, %f, %f, %f\n", x, y, z, nx, ny, nz);
#endif

			surf->vertices[v].position.x = x;
			surf->vertices[v].position.y = y;
			surf->vertices[v].position.z = z;
			surf->vertices[v].normal[0] = nx * 127.0f;
			surf->vertices[v].normal[1] = ny * 127.0f;
			surf->vertices[v].normal[2] = nz * 127.0f;
		}

#ifdef DEBUG
		printf("\nTexCoords:\n");
#endif
		//Read texture coordinates
		reader.Seek(meshstart + mesh.uvStart);
		float s, t;
		for (int v = 0; v < mesh.numVertices; ++v)
		{
			reader.Read(&s);
			reader.Read(&t);

#ifdef DEBUG
			printf("%f, %f\n", s, t);
#endif

			surf->vertices[v].texcoords[0] = floatToHalf(s);
			surf->vertices[v].texcoords[1] = floatToHalf(t);
		}

		//Read skins
		reader.Seek(meshstart + mesh.shadersStart);
		int index = 0;
		for (int n=0; n < mesh.numSkins; ++n)
		{
			reader.Seek(reader.Pos()+1);
			reader.Read(&cs[0], MAX_QPATH-1);
			name = std::string(cs);
			auto wname = WString(name);
			wname = L"./" + StripDir(wname);
			reader.Read(&index);
			index = 3;
			if (materials[wname] == nullptr)
			{
				materials[wname] = new GMFMaterial(file, GMF_LIGHTING_BLINN_PHONG);
				auto texture = new GMFTexture(file);
				texture->path = wname;
				materials[wname]->SetTexture(texture, GMF_TEXTURE_BASE);
			}
			surf->SetMaterial(materials[wname]);
			break;
		}

		surf->UpdateTangents();

		reader.Seek(meshstart + mesh.meshSize);
	}

	writer = new MemWriter;
	if (file->Save(writer))
	{
		size_out = writer->Size();
		return writer->data();
	}
	else
	{
		delete writer;
		return NULL;
	}
}

void Cleanup()
{
	delete writer;
	writer = nullptr;
}
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#include "../SDK/GMFSDK.h"
#include "../SDK/Utilities.h"
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
		"\"plugin\":{"
			"\"title\":\"Wavefront OBJ Model Exporter.\","
			"\"description\":\"Save models to the Wavefront OBJ file format.\","
			"\"author\":\"Josh Klint\","
			"\"saveModelExtensions\":\"obj\","
			"\"collapseSavedModels\":true,"
			"\"saveModelFilter\":\"Wavefront Object (*.obj):obj\""
		"}"
	"}";
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
	return s.length();
}

MemWriter* writer = NULL;

//Model load function
void* SaveModel(void* data, uint64_t size, wchar_t* filepath, uint64_t& returnsize, int flags)
{
	std::wstring path = std::wstring(filepath);
	delete writer;
	writer = new MemWriter;

	//Reader object
	MemReader reader(data, size);

	auto file = new GMFFile;
	file->Load(&reader);
	
	std::string s;
	GMFVec3 normal;
	bool usemtllib = false;

	//Write material library
	if (!file->materials.empty() and path != L"")
	{
		s = "newmtl default\n\n";
		int materialindex = 1;
		for (auto mtl : file->materials)
		{
			s += "newmtl material" + String(materialindex) + "\n";
			//"Kd " + String(mtl->color.r) + " " + String(mtl->color.g) + " " + String(mtl->color.b) + "\n";
			//if (mtl->transparent)
			//{
			//	"d " + String(mtl->color.a) + "\n";
			//} 
			if (mtl->textures[GMF_TEXTURE_BASE] != 0)
			{
				s += "map_Kd " + String(StripDir(file->textures[mtl->textures[GMF_TEXTURE_BASE] - 1]->path)) + "\n";
			}
			if (mtl->textures[GMF_TEXTURE_NORMAL] != 0)
			{
				s += "map_bump " + String(StripDir(file->textures[mtl->textures[GMF_TEXTURE_NORMAL] - 1]->path)) + "\n";
				//s += "bump " + String(StripDir(file->textures[mtl->textures[GMF_TEXTURE_NORMAL] - 1]->path)) + "\n";
				s += "norm " + String(StripDir(file->textures[mtl->textures[GMF_TEXTURE_NORMAL] - 1]->path)) + "\n";
			}
			s += "\n";
			materialindex++;
		}
		auto mtlfile = StripExt(path) + L".mtl";

		FILE* file = _wfopen(mtlfile.c_str(), L"wb");
		if (file != nullptr)
		{
			_fseeki64(file, 0, SEEK_SET);
			size_t l = fwrite(s.c_str(), 1, s.size(), file);
			if (fclose(file) == 0) usemtllib = true;
		}
	}

	//Write object
	s = "# Exported from ______ Engine\n";
	writer->Write(s, false);

	if (usemtllib)
	{
		s = "\nmtllib " + String(StripAll(path)) + ".mtl\n";
		writer->Write(s, false);
	}

	int sumverts = 0;

	for (auto node : file->nodes)
	{
		if (node->parent == NULL)
		{
			if (!node->lods.empty())
			{
				for (auto meshindex : node->lods[0]->meshes)
				{
					auto mesh = file->meshes[meshindex-1];
					
					if (usemtllib)
					{
						if (mesh->material != 0)
						{
							s = "\nusemtl material" + String(mesh->material) + "\n";
							writer->Write(s, false);
						}
						else
						{
							s = "\nusemtl material default\n";
							writer->Write(s, false);
						}
					}

					s = "\n# " + String(int(mesh->vertices.size())) + " vertices\n";
					writer->Write(s, false);
					
					//Vertex positions
					for (auto& vertex : mesh->vertices)
					{
						s = "v " + String(vertex.position.x) + " " + String(vertex.position.y) + " " + String(-vertex.position.z) + "\n";
						writer->Write(s, false);
					}
					s = "\n";
					writer->Write(s, false);
					
					//Vertex texcoords
					for (auto& vertex : mesh->vertices)
					{
						s = "vt " + String(halfToFloat(vertex.texcoords[0])) + " " + String(1.0f - halfToFloat(vertex.texcoords[1])) + "\n";
						writer->Write(s, false);
					}
					s = "\n";
					writer->Write(s, false);

					//Vertex normals
					for (auto& vertex : mesh->vertices)
					{
						normal = vertex.GetNormal();
						s = "vn " + String(normal.x) + " " + String(normal.y) + " " + String(-normal.z) + "\n";
						writer->Write(s, false);
					}

					//Indices
					s = "\n# " + String(int(mesh->indices.size() / mesh->polyverts)) + " faces\n";
					writer->Write(s, false);
					int v;
					for (int t = 0; t < mesh->indices.size() / mesh->polyverts; ++t)
					{
						s = "f";
						for (int n = mesh->polyverts - 1; n >= 0; --n)
						{
							v = int(mesh->indices[t * mesh->polyverts + n]) + 1 + sumverts;
							s += " " + String(v) + "/" + String(v) + "/" + String(v);
						}
						s += "\n";
						writer->Write(s, false);
					}

					sumverts += mesh->vertices.size();
				}
				break;
			}
		}
	}
	returnsize = writer->Size();
	return writer->data();
}

void Cleanup()
{
	delete writer;
	writer = nullptr;
}

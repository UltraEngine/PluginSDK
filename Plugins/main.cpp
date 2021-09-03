#include "../../Source/Libraries/PluginSDK/MemWriter.h"
#include "DLLExports.h"
#include "VKFormat.h"
#include <algorithm>
#include <vector>

using namespace GMFSDK;
using namespace std;

MemWriter* writer = nullptr;
std::vector<void*> allocedmem;

//DLL entry point function
#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		writer = nullptr;
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
		"\"title\":\"Support for legacy Leadwerks Engine formats.\","
		"\"description\":\"Load Leadwerks Engine TEX files.\","
		"\"author\":\"Josh Klint\","
		"\"url\":\"www.leadwerks.com\","
		"\"extension\": [\"tex\"],"
		"\"filter\": [\"Leadwerks 4 Texture (*.tex):tex\"]"
		"}"
		"}";
	if (s.length() < maxsize) maxsize = s.length();
	if (maxsize > 0) memcpy(cs, s.c_str(), maxsize);
	size_t maxsize_ = maxsize;
	return std::min(s.length(), maxsize_);
}

void FreeContext(void*)
{
	delete writer;
	writer = nullptr;
	for (auto levelData : allocedmem)
	{
		free(levelData);
	}
	allocedmem.clear();
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

void* LoadTexture(void* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
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
		texinfo.swizzle_red_alpha = 1;
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

	writer = new MemWriter();
	writer->Write(&texinfo);

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
				allocedmem.push_back(memblock);
				reader.Read(memblock, sz);
				writer->Write(&memblock, sizeof(void*));
			}
		}
	//}

	returnsize = writer->Size();
	return writer->data();
}


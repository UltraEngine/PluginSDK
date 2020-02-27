#include "../SDK/MemWriter.h"
#include "DLLExports.h"
#include "VTFLib/VTFLib.h"
#include "VKFormat.h"

using namespace GMFSDK;

//DLL entry point function
#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		vlInitialize();
		break;
	case DLL_PROCESS_DETACH:
		vlShutdown();
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
		"\"turboPlugin\":{"
			"\"title\":\"Valve texture Loader.\","
			"\"description\":\"Load Valve texture format image files.\","
			"\"author\":\"Josh Klint\","
			"\"url\":\"www.turboengine.com\","
			"\"extension\": [\"vtf\"],"
			"\"filter\": [\"Valve Texture Format (*.vtf):vtf\"]"
		"}"
	"}";

	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
	return s.length();
}

MemWriter* writer = nullptr;
std::vector<unsigned char> LoadData;

//Texture load function
void* LoadTexture(void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out)
{
	vlUInt img = 0;
	const int MAGIC_VTF = 4609110;

	if (size < 4) return nullptr;

	//Check file type
	int vtftag;
	memcpy(&vtftag, data, 4);
	if (vtftag != MAGIC_VTF) return nullptr;
	
	auto succ = vlCreateImage(&img);
	if (succ == false)
	{
		printf(vlGetLastError());
		return nullptr;
	}

	vlBindImage(img);

	auto bound = vlImageIsBound();

	if (vlImageLoadLump(data, size, false) == false)
	{
		printf(vlGetLastError());
		return nullptr;
	}

	GMFSDK::TextureInfo texinfo;

	texinfo.width = vlImageGetWidth();
	texinfo.height = vlImageGetHeight();
	texinfo.depth = vlImageGetDepth();
	bool convert = false;

	auto vtfformat = vlImageGetFormat();

	switch (vtfformat)
	{
	case IMAGE_FORMAT_RGBA8888:
		texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	case IMAGE_FORMAT_BGRA8888:
		texinfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		break;
	case IMAGE_FORMAT_DXT1:
		texinfo.format = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		break;
	case IMAGE_FORMAT_DXT1_ONEBITALPHA:
		texinfo.format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		break;
	case IMAGE_FORMAT_DXT3:
		texinfo.format = VK_FORMAT_BC2_UNORM_BLOCK;
		break;
	case IMAGE_FORMAT_DXT5:
		texinfo.format = VK_FORMAT_BC3_UNORM_BLOCK;
		break;
	default:
		texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		convert = true;
		break;
	}

	if (texinfo.depth > 1) texinfo.target = 3;// 3D texture
	int faces = vlImageGetFaceCount();
	if (faces != 1 && faces != 6)
	{
		vlImageDestroy();
		return nullptr;
	}
	if (faces == 6)
	{
		texinfo.target = 4;// cubemap
		if (texinfo.depth > 1)
		{
			vlImageDestroy();
			return nullptr;
		}
	}

	texinfo.mipmaps = vlImageGetMipmapCount();
	
	writer = new MemWriter;
	writer->Write(&texinfo);

	for (int f = 0; f < faces; ++f)
	{
		for (int n = 0; n < texinfo.mipmaps; ++n)
		{
			uint64_t mipsize = vlImageComputeMipmapSize(texinfo.width, texinfo.height, texinfo.depth, 0, vlImageGetFormat());
			if (convert) mipsize = texinfo.width * texinfo.height * 4;

			for (int z = 0; z < texinfo.depth; ++z)
			{
				auto pixels = vlImageGetData(0, f, z, n);
				if (convert)
				{
					auto datasize = LoadData.size();
					LoadData.resize(datasize + mipsize);
					vlImageConvertToRGBA8888(pixels, LoadData.data() + datasize, texinfo.width, texinfo.height, vlImageGetFormat());
					pixels = LoadData.data() + datasize;
				}
				//writer->Write(pixels, mipsize);
				writer->Write(&pixels, sizeof(void*));
			}

			if (texinfo.width > 1) texinfo.width /= 2;
			if (texinfo.height > 1) texinfo.height /= 2;
			if (texinfo.depth > 1) texinfo.depth /= 2;
		}
	}

	size_out = writer->Size();
	return writer->data();
}

void Cleanup()
{
	delete writer;
	writer = nullptr;
	LoadData.clear();
	vlImageDestroy();
}
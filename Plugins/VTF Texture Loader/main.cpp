#include "../../Source/Libraries/PluginSDK/MemWriter.h"
#include "../../Source/Libraries/PluginSDK/Utilities.h"
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
		"\"plugin\":{"
			"\"title\":\"Valve texture Loader\","
			"\"description\":\"Load and save Valve texture format image files.\","
			"\"author\":\"Josh Klint\","
			"\"threadSafe\":false,"
			"\"saveTextureExtensions\": [\"vtf\"],"
			"\"loadTextureExtensions\": [\"vtf\"],"
			"\"saveTextureFilter\": [\"Valve Texture Format (*.vtf):vtf\"],"
			"\"loadTextureFilter\": [\"Valve Texture Format (*.vtf):vtf\"]"
		"}"
	"}";
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
	return s.length();
}

MemWriter* writer = nullptr;
//std::vector<unsigned char> LoadData;

//Texture load function
void* LoadTexture(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out)
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
		//printf(vlGetLastError());
		return nullptr;
	}

	vlBindImage(img);

	auto bound = vlImageIsBound();

	if (vlImageLoadLump(data, size, false) == false)
	{
		//printf(vlGetLastError());
		return nullptr;
	}

	GMFSDK::TextureInfo texinfo;

	texinfo.version = 201;
	texinfo.width = vlImageGetWidth();
	texinfo.height = vlImageGetHeight();
	texinfo.depth = vlImageGetDepth();
	bool convert = false;

	auto vtfformat = vlImageGetFormat();

	switch (vtfformat)
	{
	case IMAGE_FORMAT_RGBA16161616F:
		texinfo.format = VK_FORMAT_R16G16B16_SFLOAT;
		break;
	case IMAGE_FORMAT_RGBA8888:
		texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		break;
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
		{
			std::string s = std::string("Unsupported texture format ") + GMFSDK::String(int(vtfformat));
			//printf(s.c_str());
		}
		return NULL;
		//texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		//convert = true;
		break;
	}

	if (texinfo.depth > 1) texinfo.target = 3;// 3D texture
	int faces = vlImageGetFaceCount();
	if (faces != 1 && faces != 6)
	{
		vlImageDestroy();
		return NULL;
	}
	if (faces == 6)
	{
		texinfo.target = 4;// cubemap
		if (texinfo.depth > 1)
		{
			vlImageDestroy();
			return NULL;
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
					//auto datasize = LoadData.size();
					//LoadData.resize(datasize + mipsize);
					//vlImageConvertToRGBA8888(pixels, LoadData.data() + datasize, texinfo.width, texinfo.height, vlImageGetFormat());
					//if (vlImageConvertToRGBA8888(pixels, &LoadData[datasize], texinfo.width, texinfo.height, vlImageGetFormat()) == false)
					//{
						return NULL;
					//}
					//pixels = LoadData.data() + datasize;
					//pixels = &LoadData[datasize];
				}
				//writer->Write(pixels, mipsize);
				int i = mipsize;
				writer->Write(&i,sizeof(int));
				writer->Write(&pixels, sizeof(void*));
			}
			if (texinfo.width > 1) texinfo.width /= 2;
			if (texinfo.height > 1) texinfo.height /= 2;
			if (texinfo.depth > 1) texinfo.depth /= 2;
		}
	}

	//vlImageDestroy();
	size_out = writer->Size();
	return writer->data();
}

void* SaveTexture(Context* context, wchar_t* extension, const int type, const int width, const int height, const int format, void** mipchain, int* sizechain, const int mipcount, const int layers, uint64_t& returnsize, int flags)
{
	UINT sz;
	vlUInt img;
	VTFImageFormat vtfformat;

	switch (format)
	{
	case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_DXT1;
		break;
	case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_DXT1_ONEBITALPHA;
		break;
	case VK_FORMAT_BC2_SRGB_BLOCK:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_DXT3;
		break;
	case VK_FORMAT_BC3_SRGB_BLOCK:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_DXT5;
		break;
	case VK_FORMAT_R8G8B8_SRGB:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_RGB888;
		break;
	case VK_FORMAT_B8G8R8_SRGB:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_BGR888;
		break;
	case VK_FORMAT_B8G8R8A8_SRGB:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_BGRA8888;
		break;
	case VK_FORMAT_R8G8B8A8_SRGB:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_RGBA8888;
		break;
	case VK_FORMAT_R16G16B16A16_UNORM:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_RGBA16161616;
		break;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_RGBA16161616F;
		break;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_RGBA32323232F;
		break;
	case VK_FORMAT_R32_SFLOAT:
		vtfformat = VTFImageFormat::IMAGE_FORMAT_R32F;
		break;
	default:
		return NULL;
		break;
	}

	vlCreateImage(&img);
	vlBindImage(img);

	vlImageCreate(width, height, 1, 1, layers, vtfformat, false, false, false);

	int pixmap = 0;
	for (int slice = 0; slice < layers; slice++)
	{
		for (int mip = 0; mip < mipcount; mip++)
		{
			if (type == 4)
			{
				vlImageSetData(0, 0, slice, mip, (vlByte*)mipchain[pixmap]);
			}
			else
			{
				vlImageSetData(0, slice, 0, mip, (vlByte*)mipchain[pixmap]);
			}
			++pixmap;
		}
	}

	vlImageSaveLump(NULL, 0, &sz);

	writer = new MemWriter;
	writer->Resize(sz);
	if (!vlImageSaveLump(writer->data(), writer->Size(), &sz))
	{
		vlImageDestroy();
		return NULL;
	}
	vlImageDestroy();
	return writer->data();
}

Context* CreateContext()
{
	return NULL;
}

void FreeContext(Context*)
{
	delete writer;
	writer = NULL;
	//LoadData.clear();
	vlImageDestroy();
}
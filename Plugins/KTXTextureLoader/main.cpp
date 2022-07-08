#include "DLLExports.h"
#include "VKFormat.h"

using namespace PluginSDK;

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
			"\"title\":\"KTX2 Texture Loader\","
			"\"description\":\"Load KTX2 texture format image files.\","
			"\"author\":\"Josh Klint and Khronos Group\","
			"\"threadSafe\":true,"
			"\"saveTextureExtensions\": [\"ktx2\"],"
			"\"loadTextureExtensions\": [\"ktx2\"],"
			"\"saveTextureFilter\": [\"KTX2 (*.ktx2):ktx2\"]"
			"\"loadTextureFilter\": [\"KTX2 (*.ktx2):ktx2\"]"
		"}"
	"}";
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
	return s.length();
}

Context::Context() : ktx(NULL), writer(NULL)
{}

Context::~Context()
{
	if (ktx) ktxTexture_Destroy((ktxTexture*)ktx);
	delete writer;
}

Context* CreateContext()
{
	return new Context;
}

void FreeContext(Context* context)
{
	delete context;
}

//Texture load function
void* LoadTexture(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out)
{
	ktx_uint64_t offset = 0;
	ktx_uint8_t* image = NULL;
	ktx_uint32_t level, layer, faceSlice;

	KTX_error_code result = ktxTexture2_CreateFromMemory((const ktx_uint8_t*)data, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &context->ktx);
	if (result != KTX_error_code::KTX_SUCCESS) return NULL;

	auto ktx = context->ktx;

	//Super decompression!!!!!!
	//if (ktx->supercompressionScheme == KTX_SUPERCOMPRESSION_BASIS)
	{
		if (ktxTexture2_NeedsTranscoding(ktx))
		{
			uint32_t* bdb = ktx->pDfd + 1; // Make pointer to basis descriptor block.
			//uint32_t colorModel = KHR_DFDVAL(bdb, MODEL);
			//if (colorModel == KHR_DF_MODEL_UASTC)
			//{
//				uint32_t channel = KHR_DFDVAL(bdb, CHANNEL);
				//if this then is normal map???
				//KHR_DF_CHANNEL_UASTC_RG;
			//}

			//bool isnormalmap = false;
			//unsigned int len;
			//std::string key = "KTXnormalMap";//"KTXwriterScParams";
			//void* val = NULL;
			//auto err = ktxHashList_FindValue(&ktx->kvDataHead, key.c_str(), &len, &val);
			//if (err == KTX_error_code::KTX_SUCCESS)
			//{
			//	String s = String((const char*)val);
			//	if (s == "1") isnormalmap = true;
			//}
			//if (s.Find("--normal_map") > -1) isnormalmap = true;

			//ktxHashList_Deserialize(&ktx->kvDataHead, ktx->kvDataLen, ktx->kvData);

			ktx_transcode_flags tflags = 0;
			ktx_transcode_fmt_e tfmt = KTX_TTF_BC7_RGBA;
			if (ktxTexture2_GetNumComponents(ktx) == 2) tfmt = KTX_TTF_BC5_RG;
			if (ktxTexture2_TranscodeBasis(ktx, tfmt, tflags) != KTX_SUCCESS) return NULL;
		}
	}

	PluginSDK::TextureInfo texinfo;
	texinfo.version = 201;
	texinfo.format = ktx->vkFormat;
	texinfo.width = ktx->baseWidth;
	texinfo.height = ktx->baseHeight;
	texinfo.depth = ktx->baseDepth;
	texinfo.faces = ktx->numFaces;
	texinfo.target = 2;
	if (ktx->isCubemap) texinfo.target = 4;
	if (ktx->baseDepth > 1) texinfo.target = 3;
	texinfo.mipmaps = ktx->numLevels;
	if (ktx->generateMipmaps)
	{
		texinfo.genmipmaps = true;
		texinfo.mipmaps = 1;
	}

	context->writer = new MemWriter;
	context->writer->Write(&texinfo);

	for (int n = 0; n < ktx->numLevels; ++n)
	{
		for (int face = 0; face < ktx->numFaces; ++face)
		{
			if (ktx->vtbl->GetImageOffset((ktxTexture*)ktx, n, 0, face, &offset) != ktx_error_code_e::KTX_SUCCESS) return NULL;
			auto sz = ktx->vtbl->GetImageSize((ktxTexture*)ktx, n);
			void* pixels = ktx->pData + offset;
			int i = sz;
			context->writer->Write(&i, sizeof(int));
			context->writer->Write(&pixels, sizeof(void*));
		}
	}

	//vlImageDestroy();
	size_out = context->writer->Size();
	return context->writer->data();
}

void* SaveTexture(Context* context, wchar_t* extension, const int type, const int width, const int height, const int format, void** mipchain, int* sizechain, const int mipcount, const int layers, uint64_t& returnsize, int flags)
{
	KTX_error_code result;
	ktxTextureCreateInfo createInfo = {};
	createInfo.baseWidth = width;
	createInfo.baseHeight = height;
	createInfo.baseDepth = 1;
	createInfo.numDimensions = 2;
	createInfo.numLevels = mipcount;
	createInfo.numLayers = layers;
	createInfo.numFaces = 1;
	if (type == 4)
	{
		createInfo.numFaces = 6;
		createInfo.numLayers = 1;
	}
	createInfo.isArray = KTX_FALSE;
	createInfo.generateMipmaps = KTX_FALSE;
	createInfo.vkFormat = format;

	result = ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &context->ktx);

	for (int face = 0; face < createInfo.numFaces; ++face)
	{
		for (int layer = 0; layer < createInfo.numLayers; ++layer)
		{
			for (int miplevel = 0; miplevel < mipcount; miplevel++)
			{
				context->ktx->vtbl->SetImageFromMemory((ktxTexture*)context->ktx, miplevel, layer, face, (const ktx_uint8_t*)mipchain[miplevel], sizechain[miplevel]);
			}
		}
	}

	//if ((flags & SAVE_KTX_ASTC_COMPRESSION) != 0)
	//OETFFunc decode = KHR_DF_TRANSFER_SRGB;
	//OETFFunc encode = KHR_DF_TRANSFER_Linear;
	//image->transformOETF(decode, encode);

	ktxBasisParams params = {};
	params.structSize = sizeof(struct ktxBasisParams);
	params.compressionLevel = KTX_ETC1S_DEFAULT_COMPRESSION_LEVEL;
	params.qualityLevel = 128;
	//params.uastc = true;
	switch (format)
	{
	case VK_FORMAT_BC5_UNORM_BLOCK:
	case VK_FORMAT_BC5_SNORM_BLOCK:
	case VK_FORMAT_R8G8_UNORM:
	case VK_FORMAT_R8G8_SNORM:
		params.normalMap = true;
	}
	auto err = ktxTexture2_CompressBasisEx(context->ktx, &params);
	if (KTX_SUCCESS != err) return NULL;

	ktx_uint8_t* ppDstBytes = NULL;
	ktx_size_t pSize = 0;
	if (KTX_SUCCESS != ktxTexture_WriteToMemory((ktxTexture*)context->ktx, &ppDstBytes, &pSize)) return NULL;

	context->writer->Write(ppDstBytes, pSize);
	return context->writer->data();
}
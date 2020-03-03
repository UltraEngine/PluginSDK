#include "../SDK/MemWriter.h"
#include "DLLExports.h"
#include "VKFormat.h"
#include <iostream> 
#include <algorithm> 

//Basis
#include "basis/transcoder/basisu.h"
#include "basis/transcoder/basisu_transcoder_internal.h"
#include "basis/basisu_enc.h"
#include "basis/basisu_etc.h"
#include "basis/basisu_gpu_texture.h"
#include "basis/basisu_frontend.h"
#include "basis/basisu_backend.h"
#include "basis/transcoder/basisu_global_selector_palette.h"
#include "basis/basisu_comp.h"
#include "basis/transcoder/basisu_transcoder.h"
#include "basis/basisu_ssim.h"

using namespace GMFSDK;
using namespace std;
using namespace basisu;
using namespace basist;

MemWriter* writer = nullptr;
basisu_transcoder* transcoder = nullptr;
std::vector<void*> allocedmem;

//DLL entry point function
#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		basisu_encoder_init();
		writer = nullptr;
		break;
	case DLL_PROCESS_DETACH:
		Cleanup();
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

void Cleanup()
{
	delete writer;
	writer = nullptr;
	delete transcoder;
	transcoder = nullptr;
	for (auto levelData : allocedmem)
	{
		free(levelData);
	}
	allocedmem.clear();
}

enum
{
	BASIS_HINT_FORMAT = 7343343556
};

void* LoadTexture(void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	MemReader reader(data, size);
	unsigned short tag;
	reader.Read(&tag);
	if (tag != 17011) return nullptr; // should start with "sB"

	basist::etc1_global_selector_codebook sel_codebook(basist::g_global_selector_cb_size, basist::g_global_selector_cb);
	transcoder = new basisu_transcoder(&sel_codebook);
	bool success = transcoder->start_transcoding(data, size);
	if (!success) return nullptr;

	basist::basis_texture_type basisTextureType = transcoder->get_texture_type(data, size);
	uint32_t imageCount = transcoder->get_total_images(data, size);
	
	//Declare format
	transcoder_texture_format format = transcoder_texture_format::cTFBC1_RGB;

	//Check userdata for format hint
	/*uint32_t userdata0, userdata1;
	transcoder->get_userdata(data, size, userdata0, userdata1);
	if (userdata0 == BASIS_HINT_FORMAT)
	{
		transcoder_texture_format hintedformat = transcoder_texture_format(userdata1);
		switch (hintedformat)
		{
		case transcoder_texture_format::cTFRGBA32:
		case transcoder_texture_format::cTFBC1_RGB:
		case transcoder_texture_format::cTFBC3_RGBA:
		case transcoder_texture_format::cTFBC4_R:
		case transcoder_texture_format::cTFBC5_RG:
		case transcoder_texture_format::cTFBC7_M5_RGBA:
			format = hintedformat;
			break;
		}
	}*/
	
	GMFSDK::TextureInfo texinfo;

	basist::basisu_image_info imageInfo;
	for (int n = 0; n < imageCount; ++n)
	{
		transcoder->get_image_info(data, size, imageInfo, n);
		if (n == 0)
		{
			texinfo.width = imageInfo.m_width;
			texinfo.height = imageInfo.m_height;
			texinfo.format = VK_FORMAT_UNDEFINED;
		}

		switch (format)
		{
		//Uncompressed RGBA
		case transcoder_texture_format::cTFRGBA32:
			texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		//BC1 / DXT1
		case transcoder_texture_format::cTFBC1_RGB:
			texinfo.format = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
			break;
		//BC3 / DXT5
		case transcoder_texture_format::cTFBC3_RGBA:
			texinfo.format = VK_FORMAT_BC3_UNORM_BLOCK;
			break;
		//BC4 (grayscale)
		case transcoder_texture_format::cTFBC4_R:
			texinfo.format = VK_FORMAT_BC4_UNORM_BLOCK;
			break;
		//BC% (normals)
		case transcoder_texture_format::cTFBC5_RG:
			texinfo.format = VK_FORMAT_BC5_UNORM_BLOCK;
			break;
		//BC7
		case transcoder_texture_format::cTFBC7_M5_RGBA:
			texinfo.format = VK_FORMAT_BC7_UNORM_BLOCK;
			break;
		}

		writer = new MemWriter;
		writer->Write(&texinfo);

		for (int miplevel = 0; miplevel < imageInfo.m_total_levels; miplevel++)
		{
			basist::basisu_image_level_info levelInfo;
			transcoder->get_image_level_info(data, size, levelInfo, n, miplevel);
			int sz = 0;

			switch (format)
			{
			case transcoder_texture_format::cTFRGBA32:
				sz = levelInfo.m_width * levelInfo.m_height * 4;
				break;
			case transcoder_texture_format::cTFBC1_RGB:
			case transcoder_texture_format::cTFBC4_R:
				sz = ((levelInfo.m_width + 3) / 4) * ((levelInfo.m_height + 3) / 4) * 8;
				break;
			case transcoder_texture_format::cTFBC3_RGBA:
			case transcoder_texture_format::cTFBC5_RG:
			case transcoder_texture_format::cTFBC7_M6_RGB:
			case transcoder_texture_format::cTFBC7_M5_RGBA:
				sz = ((levelInfo.m_width + 3) / 4) * ((levelInfo.m_height + 3) / 4) * 16;
				break;
			}

			void* levelData = malloc(sz);
			allocedmem.push_back(levelData);

			bool succ = transcoder->transcode_image_level(
				data,
				size,
				n,
				miplevel,
				levelData,
				levelInfo.m_total_blocks,
				format);

			if (!succ)
			{
				Cleanup();
				return nullptr;
			}

			writer->Write(&levelData, sizeof(void*));
		}
	}

	returnsize = writer->Size();
	return writer->data();
}

//Returns all plugin information in a JSON string
int GetPluginInfo(unsigned char* cs, int maxsize)
{
	std::string s =
	"{"
		"\"turboPlugin\":{"
			"\"title\":\"Basis texture support.\","
			"\"description\":\"Load and save Basis super-compressed texture files.\","
			"\"author\":\"Josh Klint\","
			"\"url\":\"www.leadwerks.com\","
			"\"extension\": [\"basis\"],"
			"\"filter\": [\"Basis Texture (*.basis):basis\"]"
		"}"
	"}";
	if (s.length() < maxsize) maxsize = s.length();
	if (maxsize > 0) memcpy(cs, s.c_str(), maxsize);
	size_t maxsize_ = maxsize;
	return std::min(s.length(),maxsize_);
}

//Texture load function
void* SavePixmap(int width, int height, int format, void* pixels, uint64_t size, wchar_t* extension, uint64_t& returnsize)
{
	return nullptr;// not working

	if (format != VK_FORMAT_R8G8B8A8_UNORM) return nullptr;
	if (wstring(extension) != L"basis") return nullptr;

	image img = image(width, height);
	char* thing = (char*)pixels;
	img.set_all(basisu::color_rgba(*thing));

	basis_compressor_params params;
	params.m_source_images.push_back(img);
	params.m_mip_gen = 0;// true;
	params.m_check_for_alpha = true;
	
	params.m_max_endpoint_clusters = 1;
	params.m_max_selector_clusters = 1;
	
	params.m_multithreading = false;// true;
	//int num_threads = std::thread::hardware_concurrency();
	//if (num_threads < 1) num_threads = 1;
	//job_pool jpool(num_threads);
	//params.m_pJob_pool = &jpool;

	//params.m_userdata0 = BASIS_HINT_FORMAT;
	//params.m_userdata1 = ;

	basist::etc1_global_selector_codebook sel_codebook(basist::g_global_selector_cb_size, basist::g_global_selector_cb);
	params.m_pSel_codebook = &sel_codebook;

	basis_compressor c;

	if (!c.init(params))
	{
		error_printf("basis_compressor::init() failed!\n");
		return nullptr;
	}

	basis_compressor::error_code ec = c.process();

	if (ec == basis_compressor::cECSuccess)
	{
		printf("Compression succeeded to file \"%s\"\n", params.m_out_filename.c_str());
	}
	else
	{
		bool exit_flag = true;

		switch (ec)
		{
		case basis_compressor::cECFailedReadingSourceImages:
		{
			error_printf("Compressor failed reading a source image!\n");
			break;
		}
		case basis_compressor::cECFailedValidating:
			error_printf("Compressor failed 2darray/cubemap/video validation checks!\n");
			break;
		case basis_compressor::cECFailedFrontEnd:
			error_printf("Compressor frontend stage failed!\n");
			break;
		case basis_compressor::cECFailedFontendExtract:
			error_printf("Compressor frontend data extraction failed!\n");
			break;
		case basis_compressor::cECFailedBackend:
			error_printf("Compressor backend stage failed!\n");
			break;
		case basis_compressor::cECFailedCreateBasisFile:
			error_printf("Compressor failed creating Basis file data!\n");
			break;
		case basis_compressor::cECFailedWritingOutput:
			error_printf("Compressor failed writing to output Basis file!\n");
			break;
		default:
			error_printf("basis_compress::process() failed!\n");
			break;
		}
	}

	return nullptr;
}


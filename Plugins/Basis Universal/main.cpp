#include "DLLExports.h"
#include "VKFormat.h"
#include <iostream> 
#include <algorithm> 

//Basis
#include "basis/transcoder/basisu.h"
#include "basis/transcoder/basisu_transcoder_internal.h"
#include "basis/encoder/basisu_enc.h"
#include "basis/encoder/basisu_etc.h"
#include "basis/encoder/basisu_gpu_texture.h"
#include "basis/encoder/basisu_frontend.h"
#include "basis/encoder/basisu_backend.h"
#include "basis/transcoder/basisu.h"
#include "basis/encoder/basisu_comp.h"
#include "basis/transcoder/basisu_transcoder.h"
#include "basis/encoder/basisu_ssim.h"

using namespace UltraEngine::PluginSDK;
using namespace std;
using namespace basisu;
using namespace basist;

MemWriter* writer = nullptr;
basisu_transcoder* transcoder = nullptr;
std::vector<void*> allocedmem;
basis_compressor* compressor = NULL;
//basist::etc1_global_selector_codebook* sel_codebook = nullptr;

//DLL entry point function
#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		basisu_encoder_init();
		//sel_codebook = new etc1_global_selector_codebook(basist::g_global_selector_cb_size, basist::g_global_selector_cb);
		writer = nullptr;
		transcoder = nullptr;
		compressor = nullptr;
		break;
	case DLL_PROCESS_DETACH:
		FreeContext(NULL);
		delete compressor;
		//delete sel_codebook;
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

void FreeContext(Context*)
{
	delete writer;
	writer = nullptr;
	delete transcoder;
	transcoder = nullptr;
	delete compressor;
	compressor = nullptr;
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

void* LoadTexture(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	MemReader reader(data, size);
	unsigned short tag;
	reader.Read(&tag);
	if (tag != 17011) return nullptr; // should start with "sB"

	//basist::etc1_global_selector_codebook sel_codebook(basist::g_global_selector_cb_size, basist::g_global_selector_cb);
	transcoder = new basisu_transcoder();
	bool success = transcoder->start_transcoding(data, size);
	if (!success) return nullptr;

	if (!transcoder->validate_header(data, size))
	{
		//FreeContext(NULL);
		return nullptr;
	}

	basist::basis_texture_type basisTextureType = transcoder->get_texture_type(data, size);
	uint32_t imageCount = transcoder->get_total_images(data, size);
	
	//Declare format
	transcoder_texture_format format = transcoder_texture_format::cTFBC7_RGBA;

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
	
	TextureInfo texinfo;
	texinfo.version = 201;

	switch (basisTextureType)
	{
	case cBASISTexType2D:
		texinfo.target = 2;
		break;
	case cBASISTexTypeVolume:
		texinfo.target = 3;
		texinfo.depth = imageCount;
		break;
	case cBASISTexTypeCubemapArray:
		texinfo.target = 4;
		texinfo.faces = 6;
		//format = transcoder_texture_format::cTFBC7_M6_RGB;
		break;
	default:
		printf("Unsupported texture type.\n");
		//FreeContext(NULL);
		return NULL;
		break;
	}
	int mipmaps = -1;
	texinfo.mipmaps = imageCount / texinfo.faces;

	basist::basisu_image_info imageInfo;

	transcoder->get_image_info(data, size, imageInfo, 0);
	texinfo.mipmaps = imageInfo.m_total_levels;

	basisu_file_info fileinfo = {};
	transcoder->get_file_info(data, size, fileinfo);

	if (imageInfo.m_alpha_flag)
	{
		switch (format)
		{
		case transcoder_texture_format::cTFBC1_RGB:
			format = transcoder_texture_format::cTFBC3_RGBA;
			break;
		case transcoder_texture_format::cTFBC7_M6_RGB:
			format = transcoder_texture_format::cTFBC7_M5_RGBA;
			break;
		}
	}

	texinfo.width = imageInfo.m_width;
	texinfo.height = imageInfo.m_height;
	texinfo.format = VK_FORMAT_UNDEFINED;

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

	//BC5 (normals)
	case transcoder_texture_format::cTFBC5_RG:
		texinfo.format = VK_FORMAT_BC5_UNORM_BLOCK;
		break;

	//BC7
	//case transcoder_texture_format::cTFBC7_M6_RGB:
	case transcoder_texture_format::cTFBC7_M5_RGBA:
		texinfo.format = VK_FORMAT_BC7_UNORM_BLOCK;
		break;
	}

	writer = new MemWriter;
	writer->Write(&texinfo);

	for (int miplevel = 0; miplevel < texinfo.mipmaps; miplevel++)
	{
		for (int k = 0; k < imageCount; ++k)
		{
			basist::basisu_image_level_info levelInfo;
			transcoder->get_image_level_info(data, size, levelInfo, k, miplevel);
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
			//case transcoder_texture_format::cTFBC7_M6_RGB:
			case transcoder_texture_format::cTFBC7_M5_RGBA:
				sz = ((levelInfo.m_width + 3) / 4) * ((levelInfo.m_height + 3) / 4) * 16;
				break;
			}

			void* levelData = malloc(sz);
			allocedmem.push_back(levelData);

			bool succ = transcoder->transcode_image_level(
				data,
				size,
				k,
				miplevel,
				levelData,
				levelInfo.m_total_blocks,
				format);

			if (!succ)
			{
				FreeContext(NULL);
				return nullptr;
			}

			writer->Write(&sz);
			writer->Write(&levelData, sizeof(void*));
		}
	}
	returnsize = writer->Size();
	return writer->data();
}

Context* CreateContext()
{
	return NULL;
}

//Returns all plugin information in a JSON string
int GetPluginInfo(unsigned char* cs, int maxsize)
{
	nlohmann::json j3;
	j3["plugin"]["title"] = "Basis Universal Texture Loader";
	j3["plugin"]["description"] = "Load Basis Universal texture format image files.";
	j3["plugin"]["author"] = "Ultra Software, Binomial LLC";
	j3["plugin"]["threadSafe"] = true;
	j3["plugin"]["loadTextureExtensions"] = nlohmann::json::array();

	nlohmann::json ext = nlohmann::json::object();

	// Load extensions
	ext["extension"] = "basis";
	ext["description"] = "Basis Universal";
	j3["plugin"]["loadTextureExtensions"].push_back(ext);

	// Save extensions
	ext["mipmaps"] = true;
	ext["formats"] = nlohmann::json::array();
	ext["formats"].push_back(VK_FORMAT_R8G8B8A8_UNORM);
	j3["plugin"]["saveTextureExtensions"].push_back(ext);

	std::string s = j3.dump(1, '	');

	if (maxsize > 0) memcpy(cs, s.c_str(), min(size_t(maxsize), s.length()));
	return s.length();
}

enum SaveFlags
{
	SAVE_DEFAULT = 0,
	SAVE_QUIET = 4,
	SAVE_BUILD_MIPMAPS = 512,
	SAVE_DUMP_INFO = 8192
};

//Texture save
void* SaveTexture(Context*, wchar_t* extension, const int type, const int width, const int height, const int format, void** mipchain, int* sizechain, const int mipcount_, const int layers, uint64_t& returnsize, int flags)
{
	int mipcount = mipcount_;
	basis_compressor_params m_comp_params = basis_compressor_params();

	if (format != VK_FORMAT_R8G8B8A8_UNORM) return nullptr;
	if (wstring(extension) != L"basis") return nullptr;

	//Copy image data
	int w = width;
	int h = height;
	mipcount = 0;
	for (int n = 0; n < mipcount_; ++n)
	{
		++mipcount;
		void* ptr = mipchain[n];
		//image img = image(w, h);
		m_comp_params.m_source_images.resize(m_comp_params.m_source_images.size() + 1);
		auto& img = m_comp_params.m_source_images[m_comp_params.m_source_images.size() - 1];
		img = image(w, h);
		memcpy(img.get_ptr(), ptr, w * h * 4);
		//m_comp_params.m_source_images.push_back(img);
		w /= 2;
		h /= 2;
		if (w < 4 and h < 4) break;
		w = max(4, w);
		h = max(4, h);
	}

	switch (type)
	{
	case 1:
		m_comp_params.m_tex_type = cBASISTexType2D;
		break;
	case 2:
		m_comp_params.m_tex_type = cBASISTexTypeVolume;
		break;
	case 3:
		m_comp_params.m_tex_type = cBASISTexTypeCubemapArray;
		break;
	default:
		return nullptr;
		break;
	}

	//Compressor parameters 
	m_comp_params.m_mip_gen = (SAVE_BUILD_MIPMAPS & flags) != 0;
	m_comp_params.m_check_for_alpha = true;
	m_comp_params.m_quality_level = 128;
	//m_comp_params.m_perceptual = true;// for normal maps
	m_comp_params.m_multithreading = true;
	int num_threads = std::thread::hardware_concurrency();
	if (num_threads < 1) num_threads = 1;
	job_pool jpool(num_threads);
	m_comp_params.m_pJob_pool = &jpool;
	m_comp_params.m_uastc = true;
	//m_comp_params.m_pSel_codebook = sel_codebook;
	m_comp_params.m_write_output_basis_files = false;
	//m_comp_params.m_out_filename = "models/barrel/test.basis";

	//Normal maps
	const bool normalmap = false;
	if (normalmap)
	{
		m_comp_params.m_swizzle[0] = 0;
		m_comp_params.m_swizzle[1] = 0;
		m_comp_params.m_swizzle[2] = 0;
		m_comp_params.m_swizzle[3] = 1;
	}

	//Create compressor
	compressor = new basis_compressor;
	if (!compressor->init(m_comp_params))
	{
		error_printf("basis_compressor::init() failed!\n");
		return nullptr;
	}

	//Process
	basis_compressor::error_code ec = compressor->process();

	//Handle errors
	if (ec != basis_compressor::cECSuccess)
	{
		printf("Error: %i", ec);
		return nullptr;
	}

	returnsize = compressor->get_output_basis_file().size();
	void* ptr = (void*)(&(compressor->get_output_basis_file())[0]);
	return ptr;
}

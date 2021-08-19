#include "../../Source/Libraries/PluginSDK/GMFSDK.h"
#include "DLLExports.h"

using namespace GMFSDK;

Context::Context() : writer(NULL) {}

Context::~Context()
{
	delete writer;
	writer = NULL;
	for (auto& bm : loadeddata)
	{
		FreeImage_Unload(bm);
	}
	loadeddata.clear();
	for (auto& mem : loadedmem)
	{
		FreeImage_CloseMemory(mem);
	}
	loadedmem.clear();
}

//DLL entry point function
#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		FreeImage_Initialise();
		break;
	case DLL_PROCESS_DETACH:
		FreeImage_DeInitialise();
		break;
	}
	return TRUE;
}
#endif

#ifdef __APPLE__
// Initializer.
__attribute__((constructor))
static void initializer(void) {
    FreeImage_Initialise();
}
 
// Finalizer.
__attribute__((destructor))
static void finalizer(void) {
    FreeImage_DeInitialise();
}
#endif

Context* CreateContext()
{
	return new Context;
}

//Returns all plugin information in a JSON string
int GetPluginInfo(unsigned char* cs, int maxsize)
{
	std::string s =
	"{"
		"\"plugin\":{"
			"\"title\":\"FreeImage texture loader.\","
			"\"description\":\"Load textures from common image file formats.\","
			"\"author\":\"Josh Klint\","
			"\"threadSafe\":true,"
			"\"url\":\"www.turboengine.com\","
			"\"extension\": [\"bmp\",\"jpg\",\"jpeg\",\"tga\",\"pcx\",\"tga\",\"gif\"],"
			"\"saveextensions\": [\"bmp\",\"jpg\",\"jpeg\",\"tga\",\"tga\"],"
			"\"filter\": [\"Portable Network Graphics (*.png):png\",\"Windows Bitmap (*bmp):bmp\"]"
		"}"
	"}";
#ifdef _WIN32
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
#else
    if (maxsize > 0) memcpy(cs, s.c_str(), fmin(maxsize,s.length() ) );	
#endif
    return s.length();
}

float Log2(const float num)
{
	if (num == 0.0f) return 0.0f; // log(0) is undefined
	return log(num) / 0.693147;
}

int Pow2(const int f)
{
	int n;
	n = Log2(f);
	return pow(2.0, n) + 0.5;
}

struct tgahdr
{
public:
	unsigned char idlen, colourmaptype, imgtype, indexlo, indexhi, lenlo, lenhi, cosize;
	unsigned short x0, y0, width, height;
	unsigned char psize, attbits;
};

//Save raw pixel data to image
void* SaveTexture(Context* context, wchar_t* extension, const int type, const int width, const int height, const int format, void** mipchain, int* sizechain, const int mipcount, const int layers, uint64_t& returnsize, int flags)
{
	FREE_IMAGE_FORMAT fiformat = FREE_IMAGE_FORMAT::FIF_UNKNOWN;
	int rmask= 0x00ff0000, gmask= 0x0000ff00, bmask= 0x000000ff, bpp=0;
	void* returndata = NULL;

	std::wstring ext = extension;

	if (ext == L"bmp") fiformat = FIF_BMP;
	if (ext == L"psd") fiformat = FIF_PSD;
	if (ext == L"png") fiformat = FIF_PNG;
	if (ext == L"tga") fiformat = FIF_TARGA;
	if (ext == L"jpg" or extension == L"jpeg") fiformat = FIF_JPEG;

	if (fiformat == FIF_UNKNOWN) return NULL;

	switch (format)
	{
	case VK_FORMAT_R8G8B8A8_UNORM:
		bpp = 4;
		break;
	case VK_FORMAT_R8G8B8_UNORM:
		bpp = 3;
		break;
	case VK_FORMAT_B8G8R8A8_UNORM:
		rmask = 0x000000ff;
		bmask = 0x00ff0000;
		bpp = 4;
		break;
	case VK_FORMAT_B8G8R8_UNORM:
		rmask = 0x000000ff;
		bmask = 0x00ff0000;
		bpp = 3;
		break;
	case VK_FORMAT_R16_UNORM:
		rmask = 0x00ff0000;
		gmask = 0x00000000;
		bmask = 0x00000000;
		bpp = 2;
		break;
	default:
		return NULL;
		break;
	}

	FIBITMAP* bitmap = FreeImage_ConvertFromRawBits((BYTE*)mipchain[0], width, height, width * bpp, bpp * 8, rmask, gmask, bmask, TRUE);
	FIMEMORY* mem = FreeImage_OpenMemory();

	//FreeImage_FlipVertical(bitmap);

	if (FreeImage_SaveToMemory(fiformat,bitmap, mem, 0) == 0)
	{
		FreeImage_Unload(bitmap);
		FreeImage_CloseMemory(mem);
		return NULL;
	}

	FreeImage_Unload(bitmap);

	BYTE* mem_buffer = NULL;
	DWORD size_in_bytes = 0;
	FreeImage_AcquireMemory(mem, &mem_buffer, &size_in_bytes);
	returnsize = size_in_bytes;

	//FreeImage_SeekMemory(mem, 0, SEEK_END);
	//returnsize = FreeImage_TellMemory(mem);
	
	context->loadedmem.push_back(mem);

	return mem_buffer;
}

//Texture load function
void* LoadTexture(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out)
{
	MemReader memreader(data, size);

	int pos;
	bool ispng = false; //ext == L"png";
	bool isbmp = false; //ext == L"png";
	bool isjpeg = false; //ext == L"jpg";
	bool istiff = false; //ext == L"tif";
	bool isgif = false; //ext == L"gif";
	bool ishdr = false; //ext == L"hdr";
	bool ispsd = false;
	bool isexr = false;
	bool istga = false;//no file header for this format
	bool isico = false;
	bool ispcx = false;

	//PNG check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 8)
	{
		unsigned int n;
		memreader.Read(&n);
		if (n == 1196314761)
		{
			memreader.Read(&n);
			if (n == 169478669) ispng = true;
		}
	}
	memreader.Seek(pos);

	//ICO check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 4)
	{
		unsigned short n;
		memreader.Read(&n);
		if (n == 0)
		{
			memreader.Read(&n);
			if (n == 1) isico = true;
		}
	}
	memreader.Seek(pos);

	//PCX check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 4)
	{
		unsigned char n;
		memreader.Read(&n);
		if (n == 10)
		{
			memreader.Read(&n);
			switch (n)
			{
			case 0:
			case 2:
			case 3:
			case 5:
				ispcx = true;
			}
		}
	}
	memreader.Seek(pos);

	//HDR check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 10)
	{
		char n[11];
		memreader.Read(&n[0],10);
		n[10] = 0;
		if (n == "#?RADIANCE") ishdr = true;
	}
	memreader.Seek(pos);

	//BMP check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 2)
	{
		char n[3];
		n[2] = 0;
		memreader.Read(&n[0],2);
		if (n == "BM") isbmp = true;
	}
	memreader.Seek(pos);

	//PSD check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 4)
	{
		char n[5];
		n[4] = 0;
		memreader.Read(&n[0], 4);
		if (n == "8BPS") ispsd = true;
	}
	memreader.Seek(pos);

	//GIF check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 3)
	{
		char n[4];
		n[3] = 0;
		memreader.Read(&n[0], 3);
		if (n == "GIF") isgif = true;
	}
	memreader.Seek(pos);

	//JPG check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 2)
	{
		unsigned short tag;
		memreader.Read(&tag);
		if (tag == 55551) isjpeg = true;
	}
	memreader.Seek(pos);

	//EXR check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 4)
	{
		unsigned int tag;
		memreader.Read(&tag);
		if (tag == 20000630) isexr = true;
	}
	memreader.Seek(pos);

	//TIF check
	pos = memreader.Pos();
	if (memreader.Size() - pos >= 4)
	{
		int tag;
		memreader.Read(&tag);
		if (tag == 2771273)
		{
			istiff = true;
		}
	}
	memreader.Seek(pos);

	//TGA check
	pos = memreader.Pos();
	tgahdr hdr;
	if (memreader.Size() - pos >= sizeof(hdr))
	{
		const int TGA_NULL = 0;
		const int TGA_MAP = 1;
		const int TGA_RGB = 2;
		const int TGA_MONO = 3;
		const int TGA_RLEMAP = 9;
		const int TGA_RLERGB = 10;
		const int TGA_RLEMONO = 11;
		const int TGA_COMPMAP = 32;
		const int TGA_COMPMAP4 = 33;
		memreader.Read(&hdr, sizeof(hdr));
		if (hdr.colourmaptype == 0)
		{
			if (hdr.imgtype == TGA_MAP or hdr.imgtype == TGA_RGB or hdr.imgtype == TGA_RLERGB)
			{
				if (hdr.psize == 15 or hdr.psize == 16 or hdr.psize == 24 or hdr.psize == 32)
				{
					if (hdr.width > 0 and hdr.width <= 163284 * 2)
					{
						if (hdr.height > 0 and hdr.height <= 163284 * 2) istga = true;
					}
				}
			}
		}
	}
	memreader.Seek(pos);

	if (ispng == false and isbmp == false and ishdr == false and isjpeg == false and istiff == false and ispsd == false and isexr == false and ispcx == false and isico == false and isgif == false and istga == false) return NULL;

	FIMEMORY* mem = FreeImage_OpenMemory((BYTE*)data, size);
	FIBITMAP* bitmap = nullptr;
	FIBITMAP* oldbitmap = nullptr;

	if (ispng)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_PNG, mem, PNG_IGNOREGAMMA);
	}
	else if (isbmp)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_BMP, mem, BMP_DEFAULT);
	}
	else if (ishdr)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_HDR, mem, HDR_DEFAULT);
	}
	else if (isjpeg)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_JPEG, mem, JPEG_DEFAULT);
	}
	else if (istiff)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_TIFF, mem, TIFF_DEFAULT);
	}
	else if (ispsd)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_PSD, mem, PSD_DEFAULT);
	}
	else if (isexr)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_EXR, mem, EXR_DEFAULT);
	}
	else if (ispcx)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_PCX, mem, PCX_DEFAULT);
	}
	else if (isico)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_ICO, mem, ICO_DEFAULT);
	}
	else if (isgif)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_GIF, mem, GIF_DEFAULT);
	}
	else if (istga)
	{
		bitmap = FreeImage_LoadFromMemory(FIF_TARGA, mem, TARGA_DEFAULT);
	}

	FreeImage_CloseMemory(mem);
	if (bitmap == nullptr) return NULL;

	GMFSDK::TextureInfo texinfo;

	//bool compressed = false;//(TEXTURE_COMPRESSED & flags) != 0;

	int bpp = FreeImage_GetBPP(bitmap);

	switch (bpp)
	{
		//case 8:
		//	format = VK_FORMAT_R8_UNORM;
			//format = TEXTURE_RED;
			//if (compressed) format = TEXTURE_COMPRESSED_RED;
		//	break;
	case 16:
		//format = VK_FORMAT_R8G8_UNORM;
		texinfo.format = VK_FORMAT_R16_UNORM;
		//format = TEXTURE_RG;
		//if (compressed) format = TEXTURE_COMPRESSED_RG;
		break;
	case 24:
		//format = TEXTURE_RGBA;
		texinfo.format = VK_FORMAT_R8G8B8_UNORM;
		if (FreeImage_GetBlueMask(bitmap) == 0x0000ff) texinfo.format = VK_FORMAT_B8G8R8_UNORM;
		//if (compressed) format = TEXTURE_COMPRESSED_RGBA;
		break;
	case 32:
		//format = TEXTURE_RGBA;
		texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		if (FreeImage_GetBlueMask(bitmap) == 0x000000ff) texinfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		//if (compressed) format = TEXTURE_COMPRESSED_RGBA;
		break;
	case 48:
		texinfo.format = VK_FORMAT_R16G16B16_UNORM;
		//format = TEXTURE_RGB16F;
		break;
	case 64:
		texinfo.format = VK_FORMAT_R16G16B16A16_UNORM;
		//format = TEXTURE_RGBA16F;
		break;
	case 96:
		texinfo.format = VK_FORMAT_R32G32B32_SFLOAT;
		//format = TEXTURE_RGB32F;
		break;
	case 128:
		texinfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		//format = TEXTURE_RGBA32F;
		break;
	default:
		texinfo.format = VK_FORMAT_R8G8B8_UNORM;
		//Vulkan does not support 24-bits
		oldbitmap = bitmap;
		bitmap = FreeImage_ConvertTo32Bits(bitmap);
		FreeImage_Unload(oldbitmap);
		texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		bpp = 32;
		//format = TEXTURE_RGB;
		//if (compressed) format = TEXTURE_COMPRESSED_RGB;
		//break;
		//printf("Error: Unsupported bit depth (%i).\n", bpp);
		//FreeImage_Unload(bitmap);
		//return NULL;
	}

	FreeImage_FlipVertical(bitmap);

	texinfo.width = FreeImage_GetWidth(bitmap);
	texinfo.height = FreeImage_GetHeight(bitmap);
	
	//Calculate mipmap count
	if (Pow2(texinfo.width) == texinfo.width and Pow2(texinfo.height) == texinfo.height)
	{
		texinfo.mipmaps = 1;
		int mw = texinfo.width; int mh = texinfo.height;
		while (mw > 1 or mh > 1)
		{
			texinfo.mipmaps++;
			mw /= 2;
			mh /= 2;
		}
	}

	//Save data in transfer format
	context->writer = new MemWriter;

	/*TextureInfo texinfo;
	texinfo.width = width;
	texinfo.height = height;
	texinfo.lods = lods;
	texinfo.format = format;
	writer->Write(&texinfo, texinfo.ssize);*/

	context->writer->Write(&texinfo);

	/*writer->Write((void*)tag.c_str(), 4);
	writer->Write(&version);
	writer->Write(&format);
	writer->Write(&target);
	writer->Write(&width);
	writer->Write(&height);
	writer->Write(&depth);
	writer->Write(&lods);*/
	
	//MipmapInfo mipinfo;

	//Write main image
	bpp = FreeImage_GetBPP(bitmap) / 8;
	//uint64_t length = bpp * width * height;
	//writer->Write(&width);
	//writer->Write(&height);
	//writer->Write(&depth);
	//writer->Write(&length);
	auto pixels = FreeImage_GetBits(bitmap);
	context->writer->Write(&pixels,sizeof(void*));
	context->loadeddata.push_back(bitmap);

	/*mipinfo.width = width;
	mipinfo.height = height;
	mipinfo.datasize = length;
	mipinfo.data = FreeImage_GetBits(bitmap);
	writer->Write(&mipinfo,mipinfo.ssize);*/

	//Build mipmaps
	if (Pow2(texinfo.width) == texinfo.width and Pow2(texinfo.height) == texinfo.height)
	{
		int level = 0;

		//Generate mipmaps offline
		while (texinfo.width > 1 or texinfo.height > 1)
		{
			level++;
			texinfo.width /= 2; texinfo.height /= 2;
#ifdef __APPLE__
            texinfo.width = fmax(1, texinfo.width); texinfo.height = fmax(1, texinfo.height);
#else
            texinfo.width = fmax(1, texinfo.width); texinfo.height = fmax(1, texinfo.height);
#endif
            auto newbitmap = FreeImage_Rescale(bitmap, texinfo.width, texinfo.height, FREE_IMAGE_FILTER::FILTER_BILINEAR);
			//FreeImage_Unload(bitmap);
			context->loadeddata.push_back(newbitmap);
			bitmap = newbitmap;
			bpp = FreeImage_GetBPP(bitmap) / 8;
			//length = bpp * texinfo.width * texinfo.height;
			//writer->Write(&width);
			//writer->Write(&height);
			//writer->Write(&depth);
			//writer->Write(&length);
			pixels = FreeImage_GetBits(bitmap);
			context->writer->Write(&pixels, sizeof(void*));
			/*mipinfo.width = width;
			mipinfo.height = height;
			mipinfo.datasize = length;
			mipinfo.data = FreeImage_GetBits(bitmap);
			writer->Write(&mipinfo,mipinfo.ssize);*/
		}
	}
	size_out = context->writer->Size();
	return context->writer->data();
}

void FreeContext(Context* context)
{
	delete context;
	/*delete writer;
	writer = nullptr;
	for (auto& bm : loadeddata)
	{
		FreeImage_Unload(bm);
	}
	loadeddata.clear();
	for (auto& mem : loadedmem)
	{
		FreeImage_CloseMemory(mem);
	}
	loadedmem.clear();*/
}

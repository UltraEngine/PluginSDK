#include "../SDK/MemWriter.h"
#include "DLLExports.h"
#include <inttypes.h>

using namespace GMFSDK;

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
			"\"title\":\"SVG Image Loader.\","
			"\"description\":\"Load SVG vector image files.\","
			"\"author\":\"Josh Klint\","
			"\"threadSafe\":false,"
			"\"url\":\"www.ultraengine.com\","
			"\"extension\": [\"svg\"],"
			"\"filter\": [\"Scalable Vector Graphics (*.svg):svg\"]"
		"}"
	"}";
	if (maxsize > 0) memcpy(cs, s.c_str(), min(maxsize,s.length() ) );
	return s.length();
}

MemWriter* writer = NULL;
MemWriter* pixels = NULL;

//Texture load function
void* LoadTexture(Context*, void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out)
{
	//Check file type
	if (size < 5) return NULL;
	if (std::string((const char*)data, 5) != "<?xml" and std::string((const char*)data, 4) != "<svg") return NULL;

	NSVGimage* image = NULL;
	NSVGrasterizer* rast = NULL;
	unsigned char* img = NULL;
	int w, h;
	image = nsvgParse((char*)data, "px", 96.0f);//dpi does not appear to do anything

	if (image == NULL)
	{
		nsvgDelete(image);
		return NULL;
	}

	float imagescale = 1.0f;

	w = (int)image->width * imagescale;
	h = (int)image->height * imagescale;

	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		nsvgDelete(image);
		return NULL;
	}

	pixels = new MemWriter;
	pixels->Resize(w * h * 4);
	if (pixels == NULL)
	{
		nsvgDeleteRasterizer(rast);
		nsvgDelete(image);
		return NULL;
	}

	nsvgRasterize(rast, image, 0, 0, imagescale, (unsigned char*)pixels->data(), w, h, w * 4);

	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);

	GMFSDK::TextureInfo texinfo;

	texinfo.width = w;
	texinfo.height = h;
	texinfo.depth = 1;
	bool convert = false;
	texinfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	texinfo.mipmaps = 1;
	
	writer = new MemWriter;
	writer->Write(&texinfo);
	void* dp = (void*)pixels->data();
	writer->Write(&dp, sizeof(void*));
	
	size_out = writer->Size();
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
	delete pixels;
	pixels = NULL;
}
#include "DLLExports.h"
#include "VKFormat.h"
#include <algorithm>
#include <vector>

using namespace GMFSDK;
using namespace std;

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
		"\"description\":\"Load Quake images from .lmp files.\","
		"\"author\":\"Josh Klint\","
		"\"title\":\"Quake Image Loader\","
		"\"loadTextureExtensions\":[\"lmp\"],"
		"\"loadTextureFilters\": [\"Quake Image Files (*.lmp):lmp\"]"
		"}"
		"}";
	if (s.length() < maxsize) maxsize = s.length();
	if (maxsize > 0) memcpy(cs, s.c_str(), maxsize);
	size_t maxsize_ = maxsize;
	return std::min(s.length(), maxsize_);
}

int qpallete[256] = { 0x000000,0x0f0f0f,0x1f1f1f,0x2f2f2f,0x3f3f3f,0x4b4b4b,0x5b5b5b,0x6b6b6b,
0x7b7b7b,0x8b8b8b,0x9b9b9b,0xababab,0xbbbbbb,0xcbcbcb,0xdbdbdb,0xebebeb,
0x0f0b07,0x170f0b,0x1f170b,0x271b0f,0x2f2313,0x372b17,0x3f2f17,0x4b371b,
0x533b1b,0x5b431f,0x634b1f,0x6b531f,0x73571f,0x7b5f23,0x836723,0x8f6f23,
0x0b0b0f,0x13131b,0x1b1b27,0x272733,0x2f2f3f,0x37374b,0x3f3f57,0x474767,
0x4f4f73,0x5b5b7f,0x63638b,0x6b6b97,0x7373a3,0x7b7baf,0x8383bb,0x8b8bcb,
0x000000,0x070700,0x0b0b00,0x131300,0x1b1b00,0x232300,0x2b2b07,0x2f2f07,
0x373707,0x3f3f07,0x474707,0x4b4b0b,0x53530b,0x5b5b0b,0x63630b,0x6b6b0f,
0x070000,0x0f0000,0x170000,0x1f0000,0x270000,0x2f0000,0x370000,0x3f0000,
0x470000,0x4f0000,0x570000,0x5f0000,0x670000,0x6f0000,0x770000,0x7f0000,
0x131300,0x1b1b00,0x232300,0x2f2b00,0x372f00,0x433700,0x4b3b07,0x574307,
0x5f4707,0x6b4b0b,0x77530f,0x835713,0x8b5b13,0x975f1b,0xa3631f,0xaf6723,
0x231307,0x2f170b,0x3b1f0f,0x4b2313,0x572b17,0x632f1f,0x733723,0x7f3b2b,
0x8f4333,0x9f4f33,0xaf632f,0xbf772f,0xcf8f2b,0xdfab27,0xefcb1f,0xfff31b,
0x0b0700,0x1b1300,0x2b230f,0x372b13,0x47331b,0x533723,0x633f2b,0x6f4733,
0x7f533f,0x8b5f47,0x9b6b53,0xa77b5f,0xb7876b,0xc3937b,0xd3a38b,0xe3b397,
0xab8ba3,0x9f7f97,0x937387,0x8b677b,0x7f5b6f,0x775363,0x6b4b57,0x5f3f4b,
0x573743,0x4b2f37,0x43272f,0x371f23,0x2b171b,0x231313,0x170b0b,0x0f0707,
0xbb739f,0xaf6b8f,0xa35f83,0x975777,0x8b4f6b,0x7f4b5f,0x734353,0x6b3b4b,
0x5f333f,0x532b37,0x47232b,0x3b1f23,0x2f171b,0x231313,0x170b0b,0x0f0707,
0xdbc3bb,0xcbb3a7,0xbfa39b,0xaf978b,0xa3877b,0x977b6f,0x876f5f,0x7b6353,
0x6b5747,0x5f4b3b,0x533f33,0x433327,0x372b1f,0x271f17,0x1b130f,0x0f0b07,
0x6f837b,0x677b6f,0x5f7367,0x576b5f,0x4f6357,0x475b4f,0x3f5347,0x374b3f,
0x2f4337,0x2b3b2f,0x233327,0x1f2b1f,0x172317,0x0f1b13,0x0b130b,0x070b07,
0xfff31b,0xefdf17,0xdbcb13,0xcbb70f,0xbba70f,0xab970b,0x9b8307,0x8b7307,
0x7b6307,0x6b5300,0x5b4700,0x4b3700,0x3b2b00,0x2b1f00,0x1b0f00,0x0b0700,
0x0000ff,0x0b0bef,0x1313df,0x1b1bcf,0x2323bf,0x2b2baf,0x2f2f9f,0x2f2f8f,
0x2f2f7f,0x2f2f6f,0x2f2f5f,0x2b2b4f,0x23233f,0x1b1b2f,0x13131f,0x0b0b0f,
0x2b0000,0x3b0000,0x4b0700,0x5f0700,0x6f0f00,0x7f1707,0x931f07,0xa3270b,
0xb7330f,0xc34b1b,0xcf632b,0xdb7f3b,0xe3974f,0xe7ab5f,0xefbf77,0xf7d38b,
0xa77b3b,0xb79b37,0xc7c337,0xe7e357,0x7fbfff,0xabe7ff,0xd7ffff,0x670000,
0x8b0000,0xb30000,0xd70000,0xff0000,0xfff393,0xfff7c7,0xffffff,0x9f5b53 };

Context* CreateContext()
{
	auto c = new Context;
	c->mem = NULL;
	c->writer = NULL;
	return c;
}

void FreeContext(void* pcontext)
{
	auto context = (Context*)pcontext;
	if (context->mem != NULL) free(context->mem);
	if (context->writer != NULL) delete context->writer;
	delete context;
}

void* LoadTexture(void* pcontext, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	auto context = (Context*)pcontext;

	if (size <= 8) return NULL;
	std::wstring path = std::wstring(cpath);
	wstring name = GMFSDK::StripDir(path);

	if (path.size() < 4) return NULL;
	{
		if (path[path.size() - 4] != '.' or path[path.size() - 3] != 'l' or path[path.size() - 2] != 'm' or path[path.size() - 1] != 'p')
		{
			return NULL;
		}
	}

	MemReader reader(data, size);
	int w, h, sz;
	
	if (name == L"palette.lmp" and size == 16 * 16 * 3)
	{
		w = 16;
		h = 16;
		sz = w * h * 3;

		context->mem = malloc(sz);
		memcpy(context->mem, data, sz);
	}
	else
	{
		if ((name == L"colormap.lmp" and size == 16385))
		{
			w = 256;
			h = 64;
		}
		else
		{
			reader.Read(&w, 4);
			reader.Read(&h, 4);

			//Really sketchy...
			if (size != w * h + 8 and size != w * h + 8 + 1)
			{
				return NULL;
			}
		}

		sz = w * h;
		context->mem = malloc(sz * 3);
		unsigned char index;
		int n = 0; int rgb;
		unsigned char c[3];
		c[0] = 255; c[1] = 0; c[2] = 0;
		memcpy(&rgb, &c[0], 3);

		for (int x = 0; x < w; ++x)
		{
			for (int y = 0; y < h; ++y)
			{
				reader.Read(&index);
				rgb = qpallete[index];
				memcpy(((unsigned char*)context->mem) + n * 3, &rgb, 3);
				n++;
			}
		}
		sz *= 3;
	}

	TextureInfo texinfo;
	texinfo.version = 201;
	texinfo.format = VK_FORMAT_B8G8R8_UNORM;
	texinfo.target = 2;
	texinfo.width = w;
	texinfo.height = h;
	texinfo.faces = 1;
	texinfo.mipmaps = 1;
	texinfo.depth = 1;
	texinfo.frames = 1;

	context->writer = new MemWriter;
	context->writer->Write(&texinfo);
	context->writer->Write(&sz);
	context->writer->Write(&context->mem, sizeof(void*));
	
	returnsize = context->writer->Size();
	return context->writer->data();
}


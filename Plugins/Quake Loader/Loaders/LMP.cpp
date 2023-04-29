//https://www.gamers.org/dEngine/quake/spec/quake-spec31.html#CSPR0
//https://github.com/id-Software/Quake/blob/bf4ac424ce754894ac8f1dae6a3981954bc9852d/QW/client/spritegn.h

#include "LMP.h"

using namespace UltraEngine::PluginSDK;
using namespace std;

void* LoadTextureLMP(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	if (size <= 8) return NULL;
	std::wstring path = std::wstring(cpath);
	wstring name = StripDir(path);

	auto ext = Lower(ExtractExt(path));
	if (ext != L"lmp") return NULL;

	if (path.size() >= 4)// return NULL;
	{
		auto ext = ExtractExt(path);
		if (not ext.empty() and ext != std::wstring(L"lmp")) return NULL;
		//if (p != std::wstring::npos)
		//{
		//	if (path[path.size() - 4] != '.' or path[path.size() - 3] != 'l' or path[path.size() - 2] != 'm' or path[path.size() - 1] != 'p')
		//	{
		//		return NULL;
		//	}
		//}
	}

	MemReader reader(data, size);
	int w, h, sz;

	TextureInfo texinfo;
	texinfo.format = VK_FORMAT_B8G8R8A8_UNORM;

	if (name == L"palette.lmp" and size == 16 * 16 * 3)
	{
		w = 16;
		h = 16;
		sz = w * h * 3;

		context->mem = malloc(sz);
		memcpy(context->mem, data, sz);

		texinfo.format = VK_FORMAT_B8G8R8_UNORM;
	}
	else
	{
		if (/*name == L"colormap.lmp" and*/ size == 16385 or size == 16384)
		{
			w = 256;
			h = 64;
		}
		else
		{
			char name[16] = {};
			reader.Read(&name[0], 16);
			reader.Read(&w, 4);
			reader.Read(&h, 4);

			//Really sketchy...
			if (size != w * h + 8 and size != w * h + 8 + 1)
			{
				//return NULL;
			}
		}

		sz = w * h;
		context->mem = malloc(sz * 4);
		unsigned char index;
		int n = 0; int rgb;
		unsigned char c[3];
		c[0] = 255; c[1] = 0; c[2] = 0;
		memcpy(&rgb, &c[0], 3);
		unsigned char a;

		for (int x = 0; x < w; ++x)
		{
			for (int y = 0; y < h; ++y)
			{
				reader.Read(&index);
				rgb = qpallete[index];
				memcpy(((unsigned char*)context->mem) + n * 4, &rgb, 3);
				a = 255;
				if (index == 0xFF) a = 0; // transparency
				memcpy(((unsigned char*)context->mem) + n * 4 + 3, &a, 1);
				n++;
			}
		}
		sz *= 4;
	}

	texinfo.format = VK_FORMAT_B8G8R8A8_UNORM;
	texinfo.width = w;
	texinfo.height = h;
	texinfo.mipmaps = 1;
	texinfo.mipchain.resize(1);
	texinfo.mipchain[0].width = texinfo.width;
	texinfo.mipchain[0].height = texinfo.height;
	texinfo.mipchain[0].size = texinfo.width * texinfo.height * 4;
	texinfo.mipchain[0].data = context->mem;

	context->writer.Write(&texinfo, texinfo.headersize);
	context->writer.Write(texinfo.mipchain.data(), sizeof(texinfo.mipchain[0]) * texinfo.mipchain.size());
	
	returnsize = context->writer.Size();
	return context->writer.data();
}
//https://www.gamers.org/dEngine/quake/spec/quake-spec31.html#CSPR0
//https://github.com/id-Software/Quake/blob/bf4ac424ce754894ac8f1dae6a3981954bc9852d/QW/client/spritegn.h

#include "SPR.h"

using namespace UltraEngine::PluginSDK;
using namespace std;

#define IDSPRITEHEADER	(('P'<<24)+('S'<<16)+('D'<<8)+'I')

void* LoadTextureSPR(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	spr_t header;
	if (size < sizeof(spr_t)) return NULL;

	MemReader r(data, size);
	r.Read(&header);

	if (header.id != IDSPRITEHEADER) return NULL;
	if (header.ver1 != 1) return NULL;
	//if (header.ver12 != 1 and header.ver12 != 2) return NULL;
	picture pic;

	for (int n = 0; n < header.nframes; ++n)
	{
		long marker, npics;
		r.Read(&marker);
		if (marker == 0)
		{
			r.Read(&pic);
				
			int sz = pic.width * pic.height; 
			context->mem = malloc(sz * 4);
			unsigned char index;
			int n = 0; int rgb;
			unsigned char c[3];
			c[0] = 255; c[1] = 0; c[2] = 0;
			memcpy(&rgb, &c[0], 3);
			unsigned char a = 255;

			for (int x = 0; x < pic.width; ++x)
			{
				for (int y = 0; y < pic.height; ++y)
				{
					r.Read(&index);
					rgb = qpallete[index];
					memcpy(((unsigned char*)context->mem) + n * 4, &rgb, 3);
					a = 255;
					if (index == 0xFF) a = 0;// transparency
					memcpy(((unsigned char*)context->mem) + n * 4 + 3, &a, 1);
					n++;
				}
			}
			sz *= 4;
			TextureInfo texinfo;
			texinfo.version = 201;
			texinfo.format = VK_FORMAT_B8G8R8A8_UNORM;
			texinfo.target = 2;
			texinfo.width = pic.width;
			texinfo.height = pic.height;
			texinfo.faces = 1;
			texinfo.mipmaps = 1;
			texinfo.depth = 1;
			texinfo.frames = 1;

			context->writer.Write(&texinfo);
			context->writer.Write(&sz);
			context->writer.Write(&context->mem, sizeof(void*));

			returnsize = context->writer.Size();
			return context->writer.data();
		}
		else
		{
			return NULL;
			r.Read(&npics);
			r.Seek(r.Pos() + npics * sizeof(float));
			for (int i = 0; i < npics; ++i)
			{
				r.Read(&pic);
			}
		}
	}

	return NULL;
}
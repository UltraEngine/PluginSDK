#include "TextureInfo.h"

namespace GMFSDK
{
    TextureInfo::TextureInfo()
    {
        tag = 843469895;// "GTF2"
        ssize = sizeof(TextureInfo);
        version = 200;
        width = 0;
        height = 1;
        depth = 1;
        format = 0;
        target = 2;
        faces = 1;
        mipmaps = 1;
        frames = 1;
        clampu = 0;
        clampv = 0;
        clampw = 0;
        filter = 0;
        swizzle_red_alpha = 0;
    }
}
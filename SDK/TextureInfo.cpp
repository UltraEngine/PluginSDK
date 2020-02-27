#include "TextureInfo.h"

namespace GMFSDK
{
    TextureInfo::TextureInfo()
    {
        auto s = "GTF2";
        tag[0] = s[0];
        tag[1] = s[1];
        tag[2] = s[2];
        tag[3] = s[3];
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
    }
}
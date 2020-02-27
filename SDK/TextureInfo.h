#pragma once

namespace GMFSDK
{
    struct TextureInfo
    {
        char tag[4];
        int version;
        int ssize;
        int width, height, depth;
        int format;
        int mipmaps;
        int faces;
        int target;
        int clampu, clampv, clampw;
        int frames;
        int filter;

        TextureInfo();
    };
}
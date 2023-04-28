#pragma once

namespace UltraEngine::PluginSDK
{
    struct TextureInfo
    {
        int tag;
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
        int genmipmaps;
        int swizzle_red_alpha;

        TextureInfo();
    };
}
#pragma once

#include <vector>

namespace UltraEngine::PluginSDK
{
    struct MipmapInfo;

    struct TextureInfo
    {
        int tag;
        int version;
        int width, height, depth;
        int format;
        int mipmaps;
        int faces;
        int target;
        int filter;
        int flags;
        std::vector<MipmapInfo> mipchain;

        inline static int headersize = 44;
        inline static int tagid = 843469895;

        TextureInfo();
    };

    struct MipmapInfo
    {
        MipmapInfo();
        int width, height, depth, size;
        void* data;
    };
}
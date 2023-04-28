#include "TextureInfo.h"

namespace UltraEngine::PluginSDK
{
    TextureInfo::TextureInfo()
    {
        tag = 843469895;// "GTF2"
        ssize = sizeof(TextureInfo);
        version = 201;
        width = 0;
        height = 0;
        depth = 1;
        format = 0;// VK_FORMAT_UNDEFINED
        target = 2;// 2D
        faces = 1;
        mipmaps = 1;
        frames = 1; 
        clampu = 0;
        clampv = 0;
        clampw = 0;
        filter = 1;// linear
        genmipmaps = 0;
    }
}
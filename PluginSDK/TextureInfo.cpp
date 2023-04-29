#include "TextureInfo.h"

namespace UltraEngine::PluginSDK
{
    TextureInfo::TextureInfo()
    {
        tag = 843469895;// "GTF2"
        version = 202;
        width = 0;
        height = 0;
        depth = 1;
        format = 0;// VK_FORMAT_UNDEFINED
        target = 1;// VK_IMAGE_TYPE_2D
        faces = 1;
        mipmaps = 1;
        filter = 1;// linear
        flags = 0;// TEXTURE_DEFAULT
    }

    MipmapInfo::MipmapInfo()
    {
        width = 0;
        height = 0;
        depth = 0;
        size = 0;
        data = nullptr;
    }
}
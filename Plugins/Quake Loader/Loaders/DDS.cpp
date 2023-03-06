#pragma once
#include "DDS.h"
#include <memory>

DDS_HEADER::DDS_HEADER(int w, int h)
{
	memset(this, 0, sizeof(DDS_HEADER));
	dwMipMapCount = 1;
	dwDepth = 1;
	dwWidth = w;
	dwHeight = h;
	ddspf.dwFlags |= DDS_RGB;
	ddspf.dwFlags |= pf_alphapix;
	ddspf.dwBBitMask = 0x000000ff;
	ddspf.dwGBitMask = 0x0000ff00;
	ddspf.dwRBitMask = 0x00ff0000;
	ddspf.dwABitMask = 0xff000000;
	dwPitchOrLinearSize = w * 4;
	ddspf.dwRGBBitCount = 32;
	dwFlags = DDSD_MIPMAPCOUNT;
	dwSize = sizeof(DDS_HEADER);
	ddspf.dwSize = sizeof(ddspf);
	dwMipMapCount = 1;
	dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
}
#pragma once
#include <stdint.h>

enum DDSBullshit
{
	MAGIC_DDS = 0x7C20534444,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DDS_RGB = 0x00000040,
	DDS_RGBA = 0x00000041,
	pf_alphapix = 0x00000001,
	DDSD_MIPMAPCOUNT = 0x20000,
	DDSCAPS_TEXTURE = 0x1000,
	DDSCAPS_MIPMAP = 0x400000,
	DDSCAPS_COMPLEX = 0x8
};

struct DDS_PIXELFORMAT
{
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwFourCC;
	uint32_t dwRGBBitCount;
	uint32_t dwRBitMask;
	uint32_t dwGBitMask;
	uint32_t dwBBitMask;
	uint32_t dwABitMask;
};

struct DDS_HEADER
{
	uint32_t           dwSize;
	uint32_t           dwFlags;
	uint32_t           dwHeight;
	uint32_t           dwWidth;
	uint32_t           dwPitchOrLinearSize;
	uint32_t           dwDepth;
	uint32_t           dwMipMapCount;
	uint32_t           dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	uint32_t           dwCaps;
	uint32_t           dwCaps2;
	uint32_t           dwCaps3;
	uint32_t           dwCaps4;
	uint32_t           dwReserved2;

	DDS_HEADER(int w, int h);
};
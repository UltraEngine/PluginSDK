#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <shobjidl_core.h>
	#include <Shlobj.h>
#endif
#include "VKFormat.h"
#include "../../PluginSDK/GMFSDK.h"
#include "../../PluginSDK/MemReader.h"
#include "../../PluginSDK/MemWriter.h"
#include "../../PluginSDK/Utilities.h"
#include <direct.h>

using namespace GMFSDK;

#ifdef _WIN32
	#define DLLExport __declspec( dllexport )
#else
	#define DLLExport
#endif

struct Context
{
	MemWriter writer;
};

//----------------------------------------------
// Quake stuff
#define MAXTEXTURENAME 16
#define MIPLEVELS 4
typedef struct _BSPMIPTEX
{
	char szName[MAXTEXTURENAME];  // Name of texture
	uint32_t nWidth, nHeight;     // Extends of the texture
	uint32_t nOffsets[MIPLEVELS]; // Offsets to texture mipmaps BSPMIPTEX;
} BSPMIPTEX;

/* Triangle info */
struct mdl_triangle_t
{
	int facesfront;  /* 0 = backface, 1 = frontface */
	int vertex[3];   /* vertex indices */
};

/* Texture coords */
struct mdl_texcoord_t
{
	int onseam;
	int s,t;
};

/* Compressed vertex */
struct mdl_vertex_t
{
	unsigned char v[3];
	unsigned char normalIndex;
};

/* Simple frame */
struct mdl_simpleframe_t
{
	struct mdl_vertex_t bboxmin; /* bouding box min */
	struct mdl_vertex_t bboxmax; /* bouding box max */
	char name[16];
	//struct mdl_vertex_t* verts;  /* vertex list of the frame */
};

struct vec3_t
{
	float x, y, z;
};

/* Group of simple frames */
struct mdl_groupframe_t
{
	struct mdl_vertex_t min;          /* min pos in all simple frames */
	struct mdl_vertex_t max;          /* max pos in all simple frames */
	float* time;                      /* time duration for each frame */
	//struct mdl_simpleframe_t* frames; /* simple frame list */
};

/* MDL header */
struct mdl_header_t
{
	int ident;            /* magic number: "IDPO" */
	int version;          /* version: 6 */

	vec3_t scale;         /* scale factor */
	vec3_t translate;     /* translation vector */
	float boundingradius;
	vec3_t eyeposition;   /* eyes' position */

	int num_skins;        /* number of textures */
	int skinwidth;        /* texture width */
	int skinheight;       /* texture height */

	int num_verts;        /* number of vertices */
	int num_tris;         /* number of triangles */
	int num_frames;       /* number of frames */

	int synctype;         /* 0 = synchron, 1 = random */
	int flags;            /* state flag */
	float size;
};

#define	CMP_NONE		0
#define	CMP_LZSS		1

#define	TYP_NONE		0
#define	TYP_LABEL		1

#define	TYP_LUMPY		64				// 64 + grab command number
#define	TYP_PALETTE		64
#define	TYP_QTEX		65
#define	TYP_QPIC		66
#define	TYP_SOUND		67
#define	TYP_MIPTEX		68

typedef struct
{
	int			width, height;
	byte		data[4];			// variably sized
} qpic_t;

typedef struct
{
	char		identification[4];		// should be WAD2 or 2DAW
	int			numlumps;
	int			infotableofs;
} wadinfo_t;

typedef struct
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[16];				// must be null terminated
} lumpinfo_t;


extern "C"
{
	DLLExport void* LoadModel(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& size_out);
	DLLExport BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved);
	DLLExport int GetPluginInfo(unsigned char* cs, int maxsize);
	DLLExport Context* CreateContext();
	DLLExport void FreeContext(Context* ctx);
}

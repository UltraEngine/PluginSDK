#pragma once

#include <stdint.h>

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

typedef struct
{
	char magic[4];     // Name of the new WAD format
	long diroffset;              // Position of WAD directory from start of file
	long dirsize;                // Number of entries * 0x40 (64 char)
} pakheader_t;

typedef struct
{
	char filename[0x38];       // Name of the file, Unix style, with extension,
	// 50 chars, padded with '\0'.
	long offset;                 // Position of the entry in PACK file
	long size;                   // Size of the entry in PACK file
} pakentry_t;

/* Triangle info */
struct mdl_triangle_t
{
	int facesfront;  /* 0 = backface, 1 = frontface */
	int vertex[3];   /* vertex indices */
};

//BSP Face
// https://six-of-one.github.io/quake-specifications/
typedef struct
{
	unsigned short plane_id;            // The plane in which the face lies
	//           must be in [0,numplanes[ 
	unsigned short side;                // 0 if in front of the plane, 1 if behind the plane
	long ledge_id;               // first edge in the List of edges
	//           must be in [0,numledges[
	unsigned short ledge_num;           // number of edges in the List of edges
	unsigned short texinfo_id;          // index of the Texture info the face is part of
	//           must be in [0,numtexinfos[ 
	unsigned char typelight;            // type of lighting, for the face
	unsigned char baselight;            // from 0xFF (dark) to 0 (bright)
	unsigned char light[2];             // two additional light models  
	long lightmap;               // Pointer inside the general light map, or -1
	// this define the start of the face light map
} face_t;

typedef struct                 // Mip Texture
{
	char   name[16];             // Name of the texture.
	uint32_t width;                // width of picture, must be a multiple of 8
	uint32_t height;               // height of picture, must be a multiple of 8
	uint32_t offset1;              // offset to u_char Pix[width   * height]
	uint32_t offset2;              // offset to u_char Pix[width/2 * height/2]
	uint32_t offset4;              // offset to u_char Pix[width/4 * height/4]
	uint32_t offset8;              // offset to u_char Pix[width/8 * height/8]
} miptex_t;

typedef struct
{
	unsigned short vertex0;             // index of the start vertex
	//  must be in [0,numvertices[
	unsigned short vertex1;             // index of the end vertex
	//  must be in [0,numvertices[
} edge_t;

struct vec3_t
{
	float x, y, z;
};

typedef struct
{
	vec3_t   vectorS;            // S vector, horizontal in texture space)
	float distS;              // horizontal offset in texture space
	vec3_t   vectorT;            // T vector, vertical in texture space
	float distT;              // vertical offset in texture space
	unsigned int   texture_id;         // Index of Mip Texture
	//           must be in [0,numtex[
	unsigned int   animated;           // 0 for ordinary textures, 1 for water 
} surface_t;

typedef struct
{
	vec3_t normal;               // Vector orthogonal to plane (Nx,Ny,Nz)
	// with Nx2+Ny2+Nz2 = 1
	float dist;               // Offset to plane, along the normal vector.
	// Distance from (0,0,0) to the plane
	long    type;                // Type of plane, depending on normal vector.
} plane_t;

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
	unsigned char		data[4];			// variably sized
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
	short		dummy;
	char		name[16];				// must be null terminated
} lumpinfo_t;

typedef struct
{
	int id;                // "IDSP"
	long ver1;                   // Version = 1
	long ver12;                  // 1 or 2 (maybe minor version number?)
	float radius;                // Radius of the largest frame
	long maxwidth;               // Width of the largest frame
	long maxheight;              // Height of the largest frame
	long nframes;                // Number of frames
	long uk0;                    // ? (always 0)
	long uk01;                   // ? (0 or 1)
} spr_t;

typedef struct
{
	long ofsx;                   // horizontal offset, in 3D space
	long ofsy;                   // vertical offset, in 3D space
	long width;                  // width of the picture
	long height;                 // height of the picture
	//char* Pixels;   // array of pixels (flat bitmap)
} picture;

extern inline int qpallete[256] = { 0x000000,0x0f0f0f,0x1f1f1f,0x2f2f2f,0x3f3f3f,0x4b4b4b,0x5b5b5b,0x6b6b6b,
0x7b7b7b,0x8b8b8b,0x9b9b9b,0xababab,0xbbbbbb,0xcbcbcb,0xdbdbdb,0xebebeb,
0x0f0b07,0x170f0b,0x1f170b,0x271b0f,0x2f2313,0x372b17,0x3f2f17,0x4b371b,
0x533b1b,0x5b431f,0x634b1f,0x6b531f,0x73571f,0x7b5f23,0x836723,0x8f6f23,
0x0b0b0f,0x13131b,0x1b1b27,0x272733,0x2f2f3f,0x37374b,0x3f3f57,0x474767,
0x4f4f73,0x5b5b7f,0x63638b,0x6b6b97,0x7373a3,0x7b7baf,0x8383bb,0x8b8bcb,
0x000000,0x070700,0x0b0b00,0x131300,0x1b1b00,0x232300,0x2b2b07,0x2f2f07,
0x373707,0x3f3f07,0x474707,0x4b4b0b,0x53530b,0x5b5b0b,0x63630b,0x6b6b0f,
0x070000,0x0f0000,0x170000,0x1f0000,0x270000,0x2f0000,0x370000,0x3f0000,
0x470000,0x4f0000,0x570000,0x5f0000,0x670000,0x6f0000,0x770000,0x7f0000,
0x131300,0x1b1b00,0x232300,0x2f2b00,0x372f00,0x433700,0x4b3b07,0x574307,
0x5f4707,0x6b4b0b,0x77530f,0x835713,0x8b5b13,0x975f1b,0xa3631f,0xaf6723,
0x231307,0x2f170b,0x3b1f0f,0x4b2313,0x572b17,0x632f1f,0x733723,0x7f3b2b,
0x8f4333,0x9f4f33,0xaf632f,0xbf772f,0xcf8f2b,0xdfab27,0xefcb1f,0xfff31b,
0x0b0700,0x1b1300,0x2b230f,0x372b13,0x47331b,0x533723,0x633f2b,0x6f4733,
0x7f533f,0x8b5f47,0x9b6b53,0xa77b5f,0xb7876b,0xc3937b,0xd3a38b,0xe3b397,
0xab8ba3,0x9f7f97,0x937387,0x8b677b,0x7f5b6f,0x775363,0x6b4b57,0x5f3f4b,
0x573743,0x4b2f37,0x43272f,0x371f23,0x2b171b,0x231313,0x170b0b,0x0f0707,
0xbb739f,0xaf6b8f,0xa35f83,0x975777,0x8b4f6b,0x7f4b5f,0x734353,0x6b3b4b,
0x5f333f,0x532b37,0x47232b,0x3b1f23,0x2f171b,0x231313,0x170b0b,0x0f0707,
0xdbc3bb,0xcbb3a7,0xbfa39b,0xaf978b,0xa3877b,0x977b6f,0x876f5f,0x7b6353,
0x6b5747,0x5f4b3b,0x533f33,0x433327,0x372b1f,0x271f17,0x1b130f,0x0f0b07,
0x6f837b,0x677b6f,0x5f7367,0x576b5f,0x4f6357,0x475b4f,0x3f5347,0x374b3f,
0x2f4337,0x2b3b2f,0x233327,0x1f2b1f,0x172317,0x0f1b13,0x0b130b,0x070b07,
0xfff31b,0xefdf17,0xdbcb13,0xcbb70f,0xbba70f,0xab970b,0x9b8307,0x8b7307,
0x7b6307,0x6b5300,0x5b4700,0x4b3700,0x3b2b00,0x2b1f00,0x1b0f00,0x0b0700,
0x0000ff,0x0b0bef,0x1313df,0x1b1bcf,0x2323bf,0x2b2baf,0x2f2f9f,0x2f2f8f,
0x2f2f7f,0x2f2f6f,0x2f2f5f,0x2b2b4f,0x23233f,0x1b1b2f,0x13131f,0x0b0b0f,
0x2b0000,0x3b0000,0x4b0700,0x5f0700,0x6f0f00,0x7f1707,0x931f07,0xa3270b,
0xb7330f,0xc34b1b,0xcf632b,0xdb7f3b,0xe3974f,0xe7ab5f,0xefbf77,0xf7d38b,
0xa77b3b,0xb79b37,0xc7c337,0xe7e357,0x7fbfff,0xabe7ff,0xd7ffff,0x670000,
0x8b0000,0xb30000,0xd70000,0xff0000,0xfff393,0xfff7c7,0xffffff,0x9f5b53 };
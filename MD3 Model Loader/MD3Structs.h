#ifndef _MD3_H
#define _MD3_H

// This file stores all of our structures and classes (besides the modular model ones in main.h)
// in order to read in and display a Quake3 character.  The file format is of type
// .MD3 and comes in many different files for each main body part section.  We convert
// these Quake3 structures to our own structures in Md3.cpp so that we are not dependant
// on their model data structures.  You can do what ever you want, but I like mine :P :)

// This holds the header information that is read in at the beginning of the file
struct tMd3Header
{
	char	fileID[4];					// This stores the file ID - Must be "IDP3"
	int		version;					// This stores the file version - Must be 15
	char	strFile[68];				// This stores the name of the file
	int		numFrames;					// This stores the number of animation frames
	int		numTags;					// This stores the tag count
	int		numMeshes;					// This stores the number of sub-objects in the mesh
	int		numMaxSkins;				// This stores the number of skins for the mesh
	int		headerSize;					// This stores the mesh header size
	int		tagStart;					// This stores the offset into the file for tags
	int		tagEnd;						// This stores the end offset into the file for tags
	int		fileSize;					// This stores the file size
};

// This structure is used to read in the mesh data for the .md3 models
struct tMd3MeshInfo
{
	char	meshID[4];					// This stores the mesh ID (We don't care)
	char	strName[68];				// This stores the mesh name (We do care)
	int		numMeshFrames;				// This stores the mesh aniamtion frame count
	int		numSkins;					// This stores the mesh skin count
	int     numVertices;				// This stores the mesh vertex count
	int		numTriangles;				// This stores the mesh face count
	int		triStart;					// This stores the starting offset for the triangles
	int		shadersStart;					// This stores the header size for the mesh
	int     uvStart;					// This stores the starting offset for the UV coordinates
	int		vertexStart;				// This stores the starting offset for the vertex indices
	int		meshSize;					// This stores the total mesh size
};

// This is our tag structure for the .MD3 file format.  These are used link other
// models to and the rotate and transate the child models of that model.
struct tMd3Tag
{
	char		strName[64];			// This stores the name of the tag (I.E. "tag_torso")
	float		vPosition[3];				// This stores the translation that should be performed
	float		rotation[3][3];			// This stores the 3x3 rotation matrix for this frame
};

// This stores the bone information (useless as far as I can see...)
struct tMd3Bone
{
	float	mins[3];					// This is the min (x, y, z) value for the bone
	float	maxs[3];					// This is the max (x, y, z) value for the bone
	float	position[3];				// This supposedly stores the bone position???
	float	scale;						// This stores the scale of the bone
	char	creator[16];				// The modeler used to create the model (I.E. "3DS Max")
};

// This stores the normals and vertex indices 
struct tMd3Triangle
{
	signed short	 vertex[3];				// The vertex for this face (scale down by 64.0f)
	unsigned char normal[2];				// This stores some crazy normal values (not sure...)
};

// This stores the indices into the vertex and texture coordinate arrays
struct tMd3Face
{
	int vertexIndices[3];
};

// This stores UV coordinates
struct tMd3TexCoord
{
	float textureCoord[2];
};

// This stores a skin name (We don't use this, just the name of the model to get the texture)
struct tMd3Skin
{
	char strName[68];
};

#endif

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// This is a lot of structures/class and code to look over, so take your time and dissect
// each one of them to see where they come into play and what is loaded into them.
// The most important is the CLoadMD3 class and it's definitions.  This is because you
// can't really do it another way since the file format is required to be read in that way.
// Of course, you don't need to use the ConvertDataStructures() function, but I do.
// The rest of the code, like the CModelMD3 class is just my quick implementation to
// get the model loaded, drawn, then freed.
// 
// Take a look at the bottom of Md3.cpp for a explanation of what all this stuff does.
// Let me know your comments and suggestion!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// The Quake3 .Md3 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written consent
// from ID Software.
//
// Quake, Quake2 and Quake3 are trademarks of ID Software.
// Lara Croft is a trademark of Eidos and should not be used for any commercial gain.
// All trademarks used are properties of their respective owners. 
//
//
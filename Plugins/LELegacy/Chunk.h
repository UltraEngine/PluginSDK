#pragma once
#include "DLLExports.h"
#include <vector>

#define GMF_BYTE 1
#define GMF_UNSIGNED_BYTE 2
#define GMF_SHORT 3
#define GMF_UNSIGNED_SHORT 4
#define GMF_HALF 5
#define GMF_INT 6
#define GMF_UNSIGNED_INT 7
#define GMF_FLOAT 8
#define GMF_DOUBLE 9
	
#define GMF_FILE 1
#define GMF_NODE 2
#define GMF_MESH 3
#define GMF_BONE 4
#define GMF_VERTEXARRAY 5
#define GMF_INDICEARRAY 6
#define GMF_PROPERTIES 7
#define GMF_ANIMATIONKEYS 8
#define GMF_AABB 9
#define GMF_SURFACE 10
#define GMF_NEWTONCOLLISIONTREE 11
	
#define GMF_POSITION 1
#define GMF_NORMAL 2
#define GMF_TEXTURE_COORD 3
#define GMF_COLOR 4
#define GMF_TANGENT 5
#define GMF_BINORMAL 6
#define GMF_BONEINDICE 7
#define GMF_BONEWEIGHT 8
	
#define GMF_POINTS 1
#define GMF_LINE_STRIP 2
#define GMF_LINE_LOOP 3
#define GMF_LINES 4
#define GMF_TRIANGLE_STRIP 5
#define GMF_TRIANGLE_FAN 6
#define GMF_TRIANGLES 7
#define GMF_QUAD_STRIP 8
#define GMF_QUADS 9
#define GMF_POLYGON 10
	
class Chunk
{
public:
		
	//Attributes
	int id;
	int pos;
	int size;
	int countkids;
	std::vector<Chunk*> kids;
	//Bank* data;
		
	//Constructor
	Chunk();
	//Chunk(const int id, Bank* data, Chunk* parent);
	virtual ~Chunk();
		
	//Procedures
	//void Write(Stream* stream);
#ifdef LEADWERKS_5
	void Read(shared_ptr<Stream> stream);
#else
	void Read(GMFSDK::MemReader& stream);
#endif
};
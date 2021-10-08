#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <math.h>
#include <cstring>
#include <string.h>
#include <wchar.h>
#ifndef _ASSERT
	#include <assert.h>
	#define _ASSERT assert
#endif
#include "MemWriter.h"
#include "MemReader.h"
#include "half/half.h"
#include "TextureInfo.h"
#ifdef _WIN32
	#include <Rpc.h>
#endif
#include <algorithm>

namespace GMFSDK
{
	//extern void SaveColliderNode(b3d::MeshCollider::Node* collider, MemWriter* writer);
	//extern b3d::MeshCollider::Node* ReadNode(b3d::MeshCollider* collider, MemReader* reader);

	enum GMFDataType
	{
		GMF_FLOAT,
		GMF_DOUBLE,
		GMF_SCHAR,
		GMF_UCHAR,
		GMF_SSHORT,
		GMF_USHORT,
		GMF_SINT,
		GMF_UINT,
		GMF_SLONG,
		GMF_ULONG
	};

	enum GMFVertexAttributeType
	{
		GMF_VERTEX_POSITION,
		GMF_VERTEX_NORMAL,
		GMF_VERTEX_TEXCOORDS,
		GMF_VERTEX_COLOR,
		GMF_VERTEX_TANGENTS,
		GMF_VERTEX_BITANGENTS,
		GMF_VERTEX_DISPLACEMENT,
		GMF_VERTEX_BONE_WEIGHTS,
		GMF_VERTEX_BONE_INDICES
	};

	struct GMFVertexAttribute
	{
		int attribtype;
		int datatype;
		int elements;
		int padding;
	};

	struct GMFVec2
	{
		float x, y;
	};

	struct GMFVec3
	{
		float x, y, z;

		GMFVec3 Cross(const GMFVec3& v);
		float Dot(const GMFVec3& v);
		GMFVec3 Normalize();
	};

	struct GMFVec4
	{
		float x, y, z, w;
	};

	struct GMFdVec3
	{
		double x, y, z;

		GMFdVec3 Cross(const GMFdVec3& v);
		double Dot(const GMFdVec3& v);
		GMFdVec3 Normalize();
	};

	struct GMFVertexFormat
	{
		std::vector<GMFVertexAttribute> attributes;
	};

	enum { GMF_MAX_TEXTURES = 16 };

	enum GMFNodeValues
	{
		GMF_NODE_POSITION = 1,
		GMF_NODE_ROTATION = 2,
		GMF_NODE_QUATERNION = 4,
		GMF_NODE_SCALE = 8,
		GMF_NODE_MATRIX = 16
	};

	enum GMFTextureSlots
	{
		GMF_TEXTURE_BASE = 0,
		GMF_TEXTURE_NORMAL = 1,
		GMF_TEXTURE_METAL_ROUGHNESS = 2,
		GMF_TEXTURE_DISPLACEMENT = 3,
		GMF_TEXTURE_EMISSION = 4,
		GMF_TEXTURE_BRDF = 5
	};

	enum GMFMaterialFlags
	{
		GMF_MATERIAL_TRANSPARENT = 1,
		GMF_MATERIAL_ALPHA_MASK = 2,
		GMF_MATERIAL_ZSORT = 4,
		GMF_MATERIAL_CULLFACE = 8,
		GMF_MATERIAL_TESSELLATION = 16,
		GMF_MATERAIL_CAST_SHADOW = 32,
		GMF_MATERIAL_TERRAIN_MAPPING_VERTICAL = 64,
		GMF_MATERIAL_TERRAIN_MAPPING_TRILINEAR = 128
	};

	enum GMFSaveFlags
	{
		GMF_SAVE_CLEANUP = 1
	};

	enum GMFLightingModel
	{
		GMF_LIGHTING_PBR = 0,
		GMF_LIGHTING_BLINN_PHONG = 1
	};

	enum GMFType
	{
		GMF_TYPE_PIVOT = 0,
		GMF_TYPE_MODEL = 1,
		GMF_TYPE_BONE = 2
	};

	class GMFFile;

	struct GMFdAABB
	{
		GMFdVec3 extents[2];
		GMFdVec3 center;
		GMFdVec3 size;
		double radius;

		GMFdAABB();

		void Update();
	};

	struct GMFAABB
	{
		GMFVec3 extents[2];
		GMFVec3 center;
		GMFVec3 size;
		float radius;

		GMFAABB();

		void Update();
	};

	struct GMFFrame
	{
		float position[3];
		float quaternion[4];
		float matrix[16];
	};

	struct GMFAnimation
	{
		std::vector<GMFFrame> frames;
	};

	struct GMFVertex
	{
		GMFVec3 position;
		short texcoords[2];
		signed char normal[3];
		signed char displacement;
		signed char tangent[4];
		unsigned char boneindices[4];
		unsigned char boneweights[4];

		GMFVertex();

		GMFVec3 GetNormal();
	};

	class GMFImage
	{
	public:
		int size[3];
		void* data;
	};

	class GMFTexture
	{
	public:
		int index;
		//int format;
		//int size[3];
		//std::vector<GMFImage*> images;
		std::wstring path;
		void* data;
		uint64_t datasize;

		GMFTexture(GMFFile* file);
		~GMFTexture();

		bool Save(MemWriter* writer, const int flags = 0);
	};

	class GMFMaterial
	{
	public:
		std::wstring path;
		std::string text;
		void* data;
		uint64_t datasize;
		int index;
		//int flags;
		//GMFLightingModel lightingmodel;
		//std::wstring texturefile[GMF_MAX_TEXTURES];
		//int textures[GMF_MAX_TEXTURES];

		GMFMaterial(GMFFile* file);
		~GMFMaterial();

		//void SetTexture(GMFTexture* texture, const int index);
		bool Save(MemWriter* writer, const int flags);
	};

	class GMFMesh
	{
	public:
		int index;
		int polyverts;
		int indicesize;
		GMFAABB bounds;
		int material;
		std::wstring materialfile;
		std::vector<GMFVertex> vertices;
		std::vector<uint32_t> indices;
		std::string uuid;
		uint32_t vertexcount;
		uint32_t indicecount;
		void* vertexdata;
		void* indicedata;
		//b3d::MeshCollider* collider;
		
		GMFMesh(GMFFile* file, const int polyverts = 3);

		bool UpdateBounds();
		bool UpdateTangents(const int texcoordset = 0);
		uint64_t AddVertex(const float x, const float y, const float z, const float nx = 0.0f, const float ny = 0.0f, const float nz = 0.0f, const float u = 0.0f, const float v = 0.0f,const float displacement = 1.0f);
		uint64_t AddPolygon(const uint32_t a, const uint32_t b);
		uint64_t AddPolygon(const uint32_t a, const uint32_t b, const uint32_t c);
		uint64_t AddPolygon(const uint32_t a, const uint32_t b, const uint32_t c, const uint32_t d);
		//void UpdateCollider();

		void SetMaterial(GMFMaterial* mtl);
		bool Save(MemWriter* writer, const int flags);
	};

	class GMFLOD
	{
	public:
		GMFFile* file;
		int meshcount;
		//std::vector<GMFMesh*> meshes;
		std::vector<int> meshes;

		GMFLOD(GMFFile* file);
		~GMFLOD();

		void AddMesh(GMFMesh* mesh);
	};

	class GMFNode : public std::enable_shared_from_this<GMFNode>
	{
	public:
		GMFFile* file;
		GMFType type;
		int index;
		int definedvalues;
		int parent;
		double position[3];
		float rotation[3];
		float quaternion[4];
		double scale[3];
		double matrix[16];
		float color[4];

		std::wstring name;
		GMFdAABB bounds;
		std::vector<GMFAnimation> animations;
		std::vector<GMFLOD*> lods;

		GMFLOD* AddLOD();

		GMFNode(GMFFile* file, GMFType type);
		~GMFNode();

		void SetColor(const float r, const float g, const float b, const float a);
		void SetParent(GMFNode* parent);
		void SetPosition(const double x, const double y, const double z);
		void SetPosition(const float x, const float y, const float z);
		void SetRotation(const float x, const float y, const float z);
		void SetQuaternion(const float x, const float y, const float z, const float w);
		void SetScale(const float x, const float y, const float z);
		void SetMatrix(const float* mat);
		void SetMatrix(const double* mat);
		bool Save(MemWriter* writer, const int flags);
	};

	class GMFFile
	{
	public:
		int version;
		std::vector<GMFTexture*> textures;
		std::vector<GMFMaterial*> materials;
		std::vector<GMFMesh*> meshes;
		std::vector<GMFNode*> nodes;

		GMFFile();
		~GMFFile();

		void Reset();
		bool Save(MemWriter* writer, const int flags = 0);
		bool Save(const std::string& path, const int flags = 0);
		bool Save(const std::wstring& path, const int flags = 0);
		bool Load(MemReader* reader, const std::wstring cachewritepath = L"", const std::wstring cachereadpath = L"", const int flags = 0);
	};
}
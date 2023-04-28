#include "PluginSDK.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

namespace UltraEngine::PluginSDK
{
	GMFAABB::GMFAABB()
	{
		extents[0].x = 0.0;
		extents[0].y = 0.0;
		extents[0].z = 0.0;
		extents[1].x = 0.0;
		extents[1].y = 0.0;
		extents[1].z = 0.0;
		size.x = 0.0;
		size.y = 0.0;
		size.z = 0.0;
		center.x = 0.0;
		center.y = 0.0;
		center.z = 0.0;
		radius = 0.0;
	}

	GMFdAABB::GMFdAABB()
	{
		extents[0].x = 0.0;
		extents[0].y = 0.0;
		extents[0].z = 0.0;
		extents[1].x = 0.0;
		extents[1].y = 0.0;
		extents[1].z = 0.0;
		size.x = 0.0;
		size.y = 0.0;
		size.z = 0.0;
		center.x = 0.0;
		center.y = 0.0;
		center.z = 0.0;
		radius = 0.0;
	}

	/*void GMFMesh::UpdateCollider()
	{
		int numtris = 0;
		switch (polyverts)
		{
		case 3:
			numtris = indices.size() / 3;
			break;
		case 4:
			numtris = indices.size() / 4 * 2;
			break;
		default:
			return;
			break;
		}
		std::vector<b3d::MeshCollider::Vertex> verts(vertices.size());
		std::vector<b3d::MeshCollider::Triangle> tris(numtris);
		for (int v = 0; v < vertices.size(); ++v)
		{
			verts[v].coords.x = vertices[v].position.x;
			verts[v].coords.y = vertices[v].position.y;
			verts[v].coords.z = vertices[v].position.z;
		}
		for (int p = 0; p < indices.size() / polyverts; ++p)
		{
			switch (polyverts)
			{
			case 3:
				tris[p].verts[0] = indices[p * 3 + 0];
				tris[p].verts[1] = indices[p * 3 + 1];
				tris[p].verts[2] = indices[p * 3 + 2];
				break;
			case 4:
				tris[p * 2 + 0].verts[0] = indices[p * 4 + 0];
				tris[p * 2 + 1].verts[1] = indices[p * 4 + 1];
				tris[p * 2 + 2].verts[2] = indices[p * 4 + 2];
				tris[p * 2 + 0].verts[0] = indices[p * 4 + 2];
				tris[p * 2 + 1].verts[1] = indices[p * 4 + 3];
				tris[p * 2 + 2].verts[2] = indices[p * 4 + 0];
				break;
			}
		}
		collider = new b3d::MeshCollider(verts, tris);
	}*/

	float Sign(const float f)
	{
		if (f < 0.0f) return -1.0f;
		return 1.0f;
	}

	void GMFdAABB::Update()
	{
		size.x = extents[1].x - extents[0].x;
		size.y = extents[1].y - extents[0].y;
		size.z = extents[1].z - extents[0].z;
		center.x = extents[0].x + size.x * 0.5;
		center.y = extents[0].y + size.y * 0.5;
		center.z = extents[0].z + size.z * 0.5;
		radius = sqrt(size.x * size.x + size.y * size.y + size.z * size.z) * 0.5;
	}

	void GMFAABB::Update()
	{
		size.x = extents[1].x - extents[0].x;
		size.y = extents[1].y - extents[0].y;
		size.z = extents[1].z - extents[0].z;
		center.x = extents[0].x + size.x * 0.5;
		center.y = extents[0].y + size.y * 0.5;
		center.z = extents[0].z + size.z * 0.5;
		radius = sqrt(size.x * size.x + size.y * size.y + size.z * size.z) * 0.5;
	}

	GMFdVec3 GMFdVec3::Cross(const GMFdVec3& v)
	{
		GMFdVec3 r;
		r.x = y * v.z - z * v.y;
		r.y = z * v.x - x * v.z;
		r.z = x * v.y - y * v.x;
		return r;
	}

	double GMFdVec3::Dot(const GMFdVec3& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	GMFdVec3 GMFdVec3::Normalize()
	{
		double m = sqrt(x * x + y * y + z * z);
		GMFdVec3 r;
		r.x = x / m;
		r.y = y / m;
		r.z = z / m;
		return r;
	}

	GMFVec3 GMFVec3::Cross(const GMFVec3& v)
	{
		GMFVec3 r;
		r.x = y * v.z - z * v.y;
		r.y = z * v.x - x * v.z;
		r.z = x * v.y - y * v.x;
		return r;
	}

	float GMFVec3::Dot(const GMFVec3& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	GMFVec3 GMFVec3::Normalize()
	{
		float m = sqrt(x * x + y * y + z * z);
		GMFVec3 r;
		r.x = x / m;
		r.y = y / m;
		r.z = z / m;
		return r;
	}

	GMFFile::GMFFile() : version(200) {}

	GMFLOD::GMFLOD(GMFFile* file)
	{
		this->file = file;
	}

	GMFVec3 GMFVertex::GetNormal()
	{
		return normal;
		/*GMFVec3 v;
		v.x = float(normal[0]) / 127.0;
		v.y = float(normal[1]) / 127.0;
		v.z = float(normal[2]) / 127.0;
		return v;*/
	}

	uint64_t GMFMesh::AddVertex(const float x, const float y, const float z, const float nx, const float ny, const float nz, const float u, const float v, const float displacement)
	{
		auto sz = vertices.size();
		vertices.resize(sz + 1);
		vertices[sz].position.x = x;
		vertices[sz].position.y = y;
		vertices[sz].position.z = z;
		vertices[sz].normal.x = nx;// *127.0f;
		vertices[sz].normal.y = ny;// *127.0f;
		vertices[sz].normal.z = nz;// *127.0f;
		vertices[sz].texcoords.x = (u);
		vertices[sz].texcoords.y = (v);
	//	vertices[sz].texcoords[2] = floatToHalf(u1);
	//	vertices[sz].texcoords[3] = floatToHalf(v1);
	//	vertices[sz].color[0] = r;
	//	vertices[sz].color[1] = g;
	//	vertices[sz].color[2] = b;
	//	vertices[sz].color[3] = a;
		//int d = displacement * 127.0f;
		//if (d < -128) d = 128;
		//if (d > 127) d = 127;
		vertices[sz].displacement = displacement;
		return sz;
	}

	uint64_t GMFMesh::AddPolygon(const uint32_t a, const uint32_t b)
	{
		_ASSERT(polyverts==2);
		_ASSERT(a != b);
		_ASSERT(a < vertices.size());
		_ASSERT(b < vertices.size());
		indices.push_back(a);
		indices.push_back(b);
		return indices.size() / polyverts - 1;
	}

	uint64_t GMFMesh::AddPolygon(const uint32_t a, const uint32_t b, const uint32_t c)
	{
		_ASSERT(polyverts == 3);
		_ASSERT(a != b);
		_ASSERT(b != c);
		_ASSERT(a != c);
		_ASSERT(a < vertices.size());
		_ASSERT(b < vertices.size());
		_ASSERT(c < vertices.size());
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
		return indices.size() / polyverts - 1;
	}

	uint64_t GMFMesh::AddPolygon(const uint32_t a, const uint32_t b, const uint32_t c, const uint32_t d)
	{
		_ASSERT(polyverts == 4);
		_ASSERT(a != b);
		_ASSERT(b != c);
		_ASSERT(a != c);
		_ASSERT(d != a);
		_ASSERT(d != b);
		_ASSERT(d != c);
		_ASSERT(a < vertices.size());
		_ASSERT(b < vertices.size());
		_ASSERT(c < vertices.size());
		_ASSERT(d < vertices.size());
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
		indices.push_back(d);
		return indices.size() / polyverts - 1;
	}

	GMFMesh::GMFMesh(GMFFile* file, const int polyverts) : polyverts(3), indicesize(4)
	{
		material = 0;
		this->polyverts = polyverts;
		file->meshes.push_back(this);
		index = file->meshes.size();
	}

	bool GMFMesh::UpdateBounds()
	{
		if (vertices.empty()) return false;
		bounds.extents[0] = vertices[0].position;
		bounds.extents[1] = vertices[0].position;
		for (uint64_t v = 1; v < vertices.size(); ++v)
		{
			bounds.extents[0].x = MIN(bounds.extents[0].x, vertices[v].position.x);
			bounds.extents[0].y = MIN(bounds.extents[0].y, vertices[v].position.y);
			bounds.extents[0].z = MIN(bounds.extents[0].z, vertices[v].position.z);
			bounds.extents[1].x = MAX(bounds.extents[1].x, vertices[v].position.x);
			bounds.extents[1].y = MAX(bounds.extents[1].y, vertices[v].position.y);
			bounds.extents[1].z = MAX(bounds.extents[1].z, vertices[v].position.z);
		}
		bounds.Update();
		return true;
	}

	bool GMFMesh::UpdateTangents(const int texcoordset)
	{
		if (polyverts != 3 and polyverts != 4) return false;
		if (texcoordset != 0 && texcoordset != 1) return false;
		std::map<int, bool> vertexupdated;
		GMFVec3 s, t;
		GMFVec3 position;
		GMFVec2 texcoords;
		float v1x, v1y, v1z, v1u, v1v;
		float v2x, v2y, v2z, v2u, v2v;
		float v3x, v3y, v3z, v3u, v3v;
		float x1, x2, y1, y2, z1, z2, s1, s2, t1, t2;
		float r;
		int a, b, c, i;
		uint64_t polycount = indices.size() / polyverts;

		for (i = 0; i < polycount; i++)
		{
			a = indices[i * polyverts + 0];
			b = indices[i * polyverts + 1];
			c = indices[i * polyverts + 2];

			//This should never happen, but skip if it does
			if (a == b || b == c || c == a) continue;

			if ((vertexupdated[a] == false) || (vertexupdated[b] == false) || (vertexupdated[c] == false))
			{
				position = vertices[a].position;
				texcoords.x = (vertices[a].texcoords.x);
				texcoords.y = (vertices[a].texcoords.y);
				v1x = position.x; v1y = position.y; v1z = position.z; v1u = texcoords.x; v1v = texcoords.y;

				position = vertices[b].position;
				texcoords.x = (vertices[b].texcoords.x);
				texcoords.y = (vertices[b].texcoords.y);
				v2x = position.x; v2y = position.y; v2z = position.z; v2u = texcoords.x; v2v = texcoords.y;

				position = vertices[c].position;
				texcoords.x = (vertices[c].texcoords.x);
				texcoords.y = (vertices[c].texcoords.y);
				v3x = position.x; v3y = position.y; v3z = position.z; v3u = texcoords.x; v3v = texcoords.y;

				x1 = v2x - v1x; x2 = v3x - v1x;
				y1 = v2y - v1y; y2 = v3y - v1y;
				z1 = v2z - v1z; z2 = v3z - v1z;
				s1 = v2u - v1u; s2 = v3u - v1u;
				t1 = v2v - v1v; t2 = v3v - v1v;

				float m = (s1 * t2 - s2 * t1);
				if (m == 0.0f) continue;
				r = 1.0f / m;
				
				s.x = (t2 * x1 - t1 * x2) * Sign(r); s.y = (t2 * y1 - t1 * y2) * Sign(r); s.z = (t2 * z1 - t1 * z2) * Sign(r);
				t.x = (s1 * x2 - s2 * x1) * Sign(r); t.y = (s1 * y2 - s2 * y1) * Sign(r); t.z = (s1 * z2 - s2 * z1) * Sign(r);

				s = s.Normalize();
				t = t.Normalize();

				float sgna = Sign(t.Dot(s.Cross(vertices[a].GetNormal())));
				float sgnb = Sign(t.Dot(s.Cross(vertices[b].GetNormal())));
				float sgnc = Sign(t.Dot(s.Cross(vertices[c].GetNormal())));

				if (vertexupdated[a] == false)
				{
					vertexupdated[a] = true;
					vertices[a].tangent.x = (s.x);
					vertices[a].tangent.y = (s.y);
					vertices[a].tangent.z = (s.z);
					//vertices[a].tangent.w = floatToHalf(sgna);
				}
				if (vertexupdated[b] == false)
				{
					vertexupdated[b] = true;
					vertices[b].tangent.x = (s.x);
					vertices[b].tangent.y = (s.y);
					vertices[b].tangent.z = (s.z);
				//	vertices[b].tangent.w = floatToHalf(sgnb);
				}
				if (vertexupdated[c] == false)
				{
					vertexupdated[c] = true;
					vertices[c].tangent.x = (s.x);
					vertices[c].tangent.y = (s.y);
					vertices[c].tangent.z = (s.z);
					//vertices[c].tangent[3] = floatToHalf(sgnc);
				}
			}
		}
		return true;
	}

	bool GMFFile::Load(MemReader* reader, const std::wstring cachewritepath, const std::wstring cachereadpath, const int flags)
	{
		Reset();

		//File ID
		char tag[5];
		tag[4] = 0;
		reader->Read(&tag[0], 4);
		if (strcmp(&tag[0], "GMF2") != 0) return false;

		//File version
		reader->Read(&version);
		switch (version)
		{
		case 200:
			break;
		default:
			return false;
			break;
		}

		std::string j3 = reader->ReadString();
		if (!j3.empty()) properties = nlohmann::json::parse(j3.data());

		uint64_t nodecount, nodepos, texcount, texpos, mtlcount, mtlpos, meshpos, meshcount;

		reader->Read(&texcount);
		reader->Read(&texpos);
		reader->Read(&mtlcount);
		reader->Read(&mtlpos);
		reader->Read(&meshcount);
		reader->Read(&meshpos);
		reader->Read(&nodecount);
		reader->Read(&nodepos);

		int n, type;
		uint64_t sz, start;

		//Read textures
		reader->Seek(texpos);
		for (n = 0; n < texcount; ++n)
		{
			start = reader->Pos();
			reader->Read(&sz);

			auto tex = new GMFTexture(this);
			tex->path = reader->ReadWString();
			reader->Read(&tex->datasize);
			if (tex->datasize != 0)
			{
				tex->data = malloc(tex->datasize);
				reader->Read(tex->data, tex->datasize);
			}
			reader->Seek(start + sz);
		}

		//Read materials
		reader->Seek(mtlpos);
		for (n = 0; n < mtlcount; ++n)
		{
			start = reader->Pos();
			reader->Read(&sz);

			auto mtl = new GMFMaterial(this);
			mtl->path = reader->ReadWString();
			reader->Read(&mtl->datasize);
			if (mtl->datasize != 0)
			{
				mtl->data = malloc(mtl->datasize);
				reader->Read(mtl->data, mtl->datasize);
			}

			//int tex_count;
			//reader->Read(&tex_count);

			//_ASSERT(tex_count == 16);
			//for (int i = 0; i < tex_count; ++i)
			//{
			//	reader->Read(&mtl->textures[i]);
			//}

			reader->Seek(start + sz);
		}

		//Read meshes
		reader->Seek(meshpos);
		for (n = 0; n < meshcount; ++n)
		{
			start = reader->Pos();
			reader->Read(&sz);

			auto mesh = new GMFMesh(this);

			//mesh->uuid = reader->ReadString(36);
			reader->Read(&mesh->polyverts);
			//reader->Read(&mesh->indicesize);//must be 4
			reader->Read(&mesh->material);

			//_ASSERT(mesh->indicesize == 4);
			_ASSERT(mesh->polyverts == 3 || mesh->polyverts == 4);

			//Mesh bounds
			reader->Read(&mesh->bounds, sizeof(mesh->bounds));
			
			reader->Read(&mesh->vertexcount);
			
			int indicesize = 4;
			
			//Read vertices
			mesh->vertexdata = (char*)reader->data() + reader->Pos();			
			mesh->vertices.resize(mesh->vertexcount);
			if (mesh->vertexcount) memcpy(mesh->vertices.data(), mesh->vertexdata, sizeof(mesh->vertices[0]) * mesh->vertices.size());
			
			reader->Seek(reader->Pos() + mesh->vertexcount * sizeof(GMFVertex));

			reader->Read(&mesh->indicecount);

			//Read indices
			mesh->indicedata = (char*)reader->data() + reader->Pos();
			mesh->indices.resize(mesh->indicecount);
			if (mesh->indicecount) memcpy(mesh->indices.data(), mesh->indicedata, sizeof(mesh->indices[0]) * mesh->indices.size());

			reader->Seek(reader->Pos() + mesh->indicecount * indicesize);

			//Read pick structure
			//mesh->collider = new b3d::MeshCollider();
			uint32_t verts;
			reader->Read(&verts);
			/*if (verts > 0)
			{
				mesh->collider->vertices.resize(verts);
				_ASSERT(sizeof(b3d::MeshCollider::Vertex) == 12);
				reader->Read(mesh->collider->vertices.data(), verts * sizeof(b3d::MeshCollider::Vertex));
				uint32_t tris;
				reader->Read(&tris);
				if (tris > 0)
				{
					mesh->collider->triangles.resize(tris);
					reader->Read(mesh->collider->triangles.data(), tris * sizeof(b3d::MeshCollider::Triangle));

					mesh->collider->tree = ReadNode(mesh->collider, reader);
				}
			}*/

			reader->Seek(start + sz);
		}

		//Read nodes
		reader->Seek(nodepos);
		for (n = 0; n < nodecount; ++n)
		{
			start = reader->Pos();
			reader->Read(&sz);

			reader->Read(&type);
			auto node = new GMFNode(this, GMFType(type));
			reader->Read(&node->parent);
			node->name = reader->ReadWString();
			reader->Read(&node->definedvalues);
			reader->Read(&node->position[0], sizeof(node->position[0]) * 3);
			reader->Read(&node->rotation[0], sizeof(node->rotation[0]) * 3);
			reader->Read(&node->quaternion[0], sizeof(node->quaternion[0]) * 4);
			reader->Read(&node->scale[0], sizeof(node->scale[0]) * 3);
			reader->Read(&node->matrix[0], sizeof(node->matrix[0]) * 16);
			reader->Read(&node->color[0], sizeof(node->color[0]) * 4);

			if (node->type == GMF_TYPE_MODEL)
			{
				int lodcount, meshcount;
				reader->Read(&lodcount);
				node->lods.resize(lodcount);
				for (int k = 0; k < lodcount; ++k)
				{
					node->lods[k] = new GMFLOD(this);
					reader->Read(&meshcount);
					node->lods[k]->meshes.resize(meshcount);
					for (int m = 0; m < meshcount; ++m)
					{
						reader->Read(&node->lods[k]->meshes[m]);
					}
				}
			}
			reader->Seek(start + sz);
		}
		return true;
	}

	/*b3d::MeshCollider::Node* ReadNode(b3d::MeshCollider* collider, MemReader* reader)
	{
		uint32_t count;
		b3d::MeshCollider::Node* node = new b3d::MeshCollider::Node;
		_ASSERT(sizeof(node->box) == 24);
		reader->Read(&node->box,sizeof(node->box));
		reader->Read(&count);
		if (count > 0)
		{
			node->triangles.resize(count);
			reader->Read(node->triangles.data(), sizeof(node->triangles[0]) * count);
			collider->leaves.push_back(node);
		}
		else
		{
			node->left = ReadNode(collider, reader);
			node->right = ReadNode(collider, reader);
		}
		return node;
	}*/

	bool GMFFile::Save(const std::string& path, const int flags)
	{
		MemWriter writer;
		if (!Save(&writer, flags)) return false;
		FILE* f = NULL;
#ifdef _WIN32
		if (fopen_s(&f, path.c_str(), "w+b") != 0) return false;
#else
		f = fopen(path.c_str(),"w+b");
#endif
		if (f == NULL) return false;
		size_t l = fwrite(writer.data(), 1, writer.Size(), f);
		if (l != writer.Size()) return false;
		fclose(f);
		return true;
	}

	bool GMFFile::Save(const std::wstring& path, const int flags)
	{
		MemWriter writer;
		if (!Save(&writer, flags)) return false;
		FILE* f = NULL;
#ifdef _WIN32
		if (_wfopen_s(&f, path.c_str(), L"w+b") != 0) return false;
#else
		std::vector<char> s;
		auto sz = wcstombs(NULL,path.c_str(),path.size());
		if (sz == -1) return false;
		s.resize(sz);
		sz = wcstombs(s.data(),path.c_str(),path.size());
		if (sz == -1) return false;
		auto utf8 = std::string(s.data(),s.size());
		f = fopen(utf8.c_str(), "w+b");
#endif
		if (f == NULL) return false;
		size_t l = fwrite(writer.data(), 1, writer.Size(), f);
		if (l != writer.Size()) return false;
		fclose(f);
		return true;
	}

	bool GMFFile::Save(MemWriter* writer, const int flags)
	{
		std::string s = "GMF2";
		writer->Write((void*)s.c_str(), 4);
		writer->Write(&version);

		writer->Write(properties.dump('	'));

		uint64_t tex_count = textures.size();
		writer->Write(&tex_count);
		uint64_t tex_pos = writer->Pos();
		writer->Seek(writer->Pos() + sizeof(uint64_t));

		uint64_t mtl_count = materials.size();
		writer->Write(&mtl_count);
		uint64_t mtl_pos = writer->Pos();
		writer->Seek(writer->Pos() + sizeof(uint64_t));

		uint64_t mesh_count = meshes.size();
		writer->Write(&mesh_count);
		uint64_t mesh_pos = writer->Pos();
		writer->Seek(writer->Pos() + sizeof(uint64_t));

		uint64_t node_count = nodes.size();
		writer->Write(&node_count);
		uint64_t node_pos = writer->Pos();
		writer->Seek(writer->Pos() + sizeof(uint64_t));

		//Write textures
		uint64_t tex_start = writer->Pos();
		for (auto tex : textures)
		{
			if (!tex->Save(writer, flags)) return false;
		}

		//Write materials
		uint64_t mtl_start = writer->Pos();
		for (auto mtl : materials)
		{
			if (!mtl->Save(writer, flags)) return false;
		}

		//Write meshes
		uint64_t mesh_start = writer->Pos();
		for (auto mesh : meshes)
		{
			if (!mesh->Save(writer, flags)) return false;
		}

		//Write nodes
		uint64_t node_start = writer->Pos();
		for (auto node : nodes)
		{
			if (!node->Save(writer, flags)) return false;
		}

		writer->Seek(tex_pos);
		writer->Write(&tex_start);
		writer->Seek(mtl_pos);
		writer->Write(&mtl_start);
		writer->Seek(mesh_pos);
		writer->Write(&mesh_start);
		writer->Seek(node_pos);
		writer->Write(&node_start);

		return true;
	}
	
	bool GMFMesh::Save(MemWriter* writer, const int flags)
	{
		uint64_t start = writer->Pos();
		writer->Seek(start + sizeof(uint64_t));

		writer->Write(&polyverts);
		writer->Write(&material);
		uint32_t vertex_count = vertices.size();
		uint32_t indice_count = indices.size();

		UpdateBounds();
		writer->Write(&bounds, sizeof(bounds));

		writer->Write(&vertex_count);
		
		_ASSERT(sizeof(bounds) == 13 * sizeof(float));
		
		if (!vertices.empty())
		{
			writer->Write(vertices.data(), vertices.size() * sizeof(GMFVertex));
		}

		writer->Write(&indice_count);

		if (!indices.empty())
		{
			writer->Write(indices.data(), indices.size() * sizeof(indices[0]));
		}

		/*UpdateCollider();
		if (collider)
		{
			if (collider->tree == NULL || collider->vertices.empty()==true || collider->triangles.empty()==true)
			{
				delete collider;
				collider = NULL;
			}
		}
		if (collider)
		{
			uint32_t sz = collider->vertices.size();
			writer->Write(&sz);
			_ASSERT(sizeof(b3d::MeshCollider::Vertex) == 12);
			writer->Write(collider->vertices.data(), collider->vertices.size() * sizeof(b3d::MeshCollider::Vertex));
			sz = collider->triangles.size();
			writer->Write(&sz);
			writer->Write(collider->triangles.data(), collider->triangles.size() * sizeof(b3d::MeshCollider::Triangle));
			SaveColliderNode(collider->tree, writer);
		}
		else
		{*/
			int n = 0;
			writer->Write(&n);
			writer->Write(&n);
		//}

		if ((GMF_SAVE_CLEANUP & flags) != 0)
		{
			vertices.clear();
			vertices.shrink_to_fit();
			indices.clear();
			indices.shrink_to_fit();
		}

		uint64_t end = writer->Pos();
		uint64_t sz = end - start;
		writer->Seek(start);
		writer->Write(&sz);
		writer->Seek(end);

		return true;
	}

	/*void SaveColliderNode(b3d::MeshCollider::Node* node, MemWriter* writer)
	{
		_ASSERT(node != NULL);
		writer->Write(&node->box.a.x);
		writer->Write(&node->box.a.y);
		writer->Write(&node->box.a.z);
		writer->Write(&node->box.b.x);
		writer->Write(&node->box.b.y);
		writer->Write(&node->box.b.z);
		uint32_t sz = node->triangles.size();
		writer->Write(&sz);
		if (node->triangles.empty())
		{
			SaveColliderNode(node->left, writer);
			SaveColliderNode(node->right, writer);
		}
		else
		{
			writer->Write(node->triangles.data(), node->triangles.size() * sizeof(node->triangles[0]));
			//for (int t = 0; t < node->triangles.size(); ++t)
			//{
			//	writer->Write(&node->triangles[t * 3 + 0]);
			//	writer->Write(&node->triangles[t * 3 + 1]);
			//	writer->Write(&node->triangles[t * 3 + 2]);
			//}
		}
	}*/

	bool GMFMaterial::Save(MemWriter* writer, const int flags)
	{
		uint64_t start = writer->Pos();
		writer->Seek(start + sizeof(uint64_t));

		writer->Write(path);
		writer->Write(&datasize);
		if (data != NULL)
		{
			writer->Write(data, datasize);
		}

		//int i = GMF_MAX_TEXTURES;
		//writer->Write(&i);
		//writer->Write(&textures[0], GMF_MAX_TEXTURES * sizeof(textures[0]));

		//float f=1;

		//Flags
		//writer->Write(&flags);

		//Base color
		//writer->Write(&f);
		//writer->Write(&f);
		//writer->Write(&f);
		//writer->Write(&f);

		//Emission
		//writer->Write(&f);
		//writer->Write(&f);
		//writer->Write(&f);

		//Metal / roughness
		//writer->Write(&f);
		//writer->Write(&f);

		//Normal scale
		/*writer->Write(&f);

		//Displacement
		writer->Write(&f);

		//Occlussion strength
		writer->Write(&f);

		//Mapping scale
		writer->Write(&f);
		writer->Write(&f);

		//Texture scroll
		writer->Write(&f);
		writer->Write(&f);
		writer->Write(&f);*/

		uint64_t end = writer->Pos();
		uint64_t sz = end - start;
		writer->Seek(start);
		writer->Write(&sz);
		writer->Seek(end);

		return true;
	}

	bool GMFTexture::Save(MemWriter* writer, const int flags)
	{
		uint64_t start = writer->Pos();
		writer->Seek(start + sizeof(uint64_t));

		writer->Write(path);
		writer->Write(&datasize);

		if (data != NULL)
		{
			writer->Write(data, datasize);
		}

		uint64_t end = writer->Pos();
		uint64_t sz = end - start;
		writer->Seek(start);
		writer->Write(&sz);
		writer->Seek(end);

		return true;
	}

	bool GMFNode::Save(MemWriter* writer, const int flags)
	{
		uint64_t start = writer->Pos();
		writer->Seek(start + sizeof(start));

		writer->Write(&type);
		writer->Write(&parent);
		writer->Write(name);

		writer->Write(&definedvalues);
		writer->Write(&position[0], 3 * sizeof(position[0]));
		writer->Write(&rotation[0], 3 * sizeof(rotation[0]));
		writer->Write(&quaternion[0], 4 * sizeof(quaternion[0]));
		writer->Write(&scale[0], 3 * sizeof(scale[0]));
		writer->Write(&matrix[0], 16 * sizeof(matrix[0]));
		writer->Write(&color[0], 4 * sizeof(color[0]));

		int lod_count = lods.size();
		if (type != GMF_TYPE_MODEL) lod_count = 0;
		writer->Write(&lod_count);

		if (type == GMF_TYPE_MODEL)
		{
			for (auto lod : lods)
			{
				int mesh_count = lod->meshes.size();
				writer->Write(&mesh_count);
				for (auto meshindex : lod->meshes)
				{
					writer->Write(&meshindex);
				}
			}
		}

		uint64_t end = writer->Pos();
		uint64_t sz = end - start;
		writer->Seek(start);
		writer->Write(&sz);
		writer->Seek(end);

		return true;
	}

	/*
	void GMFFile::SetRoot(GMFNode* node)
	{
		if (node == NULL)
		{
			root = 0;
		}
		else
		{
			root = node->index;
		}
	}*/

	GMFTexture::GMFTexture(GMFFile* file)
	{
		data = NULL;
		datasize = 0;
		file->textures.push_back(this);
		index = file->textures.size();
	}
	
	GMFTexture::~GMFTexture()
	{
		/*for (auto image : images)
		{
			delete image;
		}*/
		if (data != NULL) free(data);
		//images.clear();
	}

	void GMFNode::SetColor(const float r, const float g, const float b, const float a)
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;
	}

	GMFNode::GMFNode(GMFFile* file, GMFType type) : file(NULL)
	{
		this->file = file;

		definedvalues = 0;

		color[0] = 1;
		color[1] = 1;
		color[2] = 1;
		color[3] = 1;

		position[0] = 0;
		position[1] = 0;
		position[2] = 0;
		
		rotation[0] = 0;
		rotation[1] = 0;
		rotation[2] = 0;

		scale[0] = 1;
		scale[1] = 1;
		scale[2] = 1;

		quaternion[0] = 0;
		quaternion[1] = 0;
		quaternion[2] = 0;
		quaternion[3] = 1;
		
		matrix[0] = 1;
		matrix[1] = 0;
		matrix[2] = 0;
		matrix[3] = 0;
		
		matrix[4] = 0;
		matrix[5] = 1;
		matrix[6] = 0;
		matrix[7] = 0;

		matrix[8] = 0;
		matrix[9] = 0;
		matrix[10] = 1;
		matrix[11] = 0;

		matrix[12] = 0;
		matrix[13] = 0;
		matrix[14] = 0;
		matrix[15] = 1;

		parent = 0;
		this->type = type;
		file->nodes.push_back(this);
		index = file->nodes.size();
	}

	GMFMaterial::GMFMaterial(GMFFile* file) : index(-1)
	{
		datasize = 0;
		data = NULL;
		//this->lightingmodel = lightingmodel;
		//flags = 0;
		file->materials.push_back(this);
		index = file->materials.size();
		//for (int n = 0; n < GMF_MAX_TEXTURES; ++n)
		//{
		//	textures[n] = 0;
		//}
	}

	GMFMaterial::~GMFMaterial()
	{
		if (data) free(data);
	}

	/*void GMFMaterial::SetTexture(GMFTexture* texture, const int index)
	{
		if (texture == NULL)
		{
			this->textures[index] = 0;
		}
		else
		{
			this->textures[index] = texture->index;
		}
	}*/

	GMFVertex::GMFVertex() : tangent{0,0,0}, position { 0.0f, 0.0f, 0.0f }, normal{ 0,0,0 }, /*color{ 255,255,255,255 },*/ boneindices{ 0,0,0,0 }, boneweights{ 0,0,0,0 }, displacement(127)
	{
		texcoords.x = 0.0f;
		texcoords.y = 0.0f;
	}

	GMFNode::~GMFNode()
	{
		for (auto lod : lods)
		{
			delete lod;
		}
		lods.clear();
	}

	GMFFile::~GMFFile()
	{
		Reset();
	}

	void GMFFile::Reset()
	{
		for (auto node : nodes)
		{
			delete node;
		}
		for (auto mesh : meshes)
		{
			delete mesh;
		}
		for (auto mtl : materials)
		{
			delete mtl;
		}
		for (auto tex : textures)
		{
			delete tex;
		}
		nodes.clear();
		meshes.clear();
		materials.clear();
		textures.clear();
	}

	void GMFLOD::AddMesh(GMFMesh* mesh)
	{
		meshes.push_back(mesh->index);
	}

	GMFLOD::~GMFLOD()
	{
		meshes.clear();
	}

	GMFLOD* GMFNode::AddLOD()
	{
		if (type != GMF_TYPE_MODEL) return nullptr;
		auto lod = new GMFLOD(file);
		lods.push_back(lod);
		return lod;
	}

	void GMFNode::SetParent(GMFNode* parent)
	{
		_ASSERT(parent != this);
		if (parent)
		{
			this->parent = parent->index;
		}
		else
		{
			this->parent = 0;
		}
	}

	void GMFMesh::SetMaterial(GMFMaterial* mtl)
	{
		if (mtl == NULL)
		{
			material = 0;
		}
		else
		{
			material = mtl->index;
		}
	}

	void GMFNode::SetPosition(const double x, const double y, const double z)
	{
		position[0] = x;
		position[1] = y;
		position[2] = z;
		definedvalues |= GMF_NODE_POSITION;
	}

	void GMFNode::SetPosition(const float x, const float y, const float z)
	{
		position[0] = x;
		position[1] = y;
		position[2] = z;
		definedvalues |= GMF_NODE_POSITION;
	}

	void GMFNode::SetRotation(const float x, const float y, const float z)
	{
		rotation[0] = x;
		rotation[1] = y;
		rotation[2] = z;
		definedvalues |= GMF_NODE_ROTATION;
	}

	void GMFNode::SetScale(const float x, const float y, const float z)
	{
		scale[0] = x;
		scale[1] = y;
		scale[2] = z;
		definedvalues |= GMF_NODE_SCALE;
	}

	void GMFNode::SetQuaternion(const float x, const float y, const float z, const float w)
	{
		quaternion[0] = x;
		quaternion[1] = y;
		quaternion[2] = z;
		quaternion[3] = w;
		definedvalues |= GMF_NODE_QUATERNION;
	}

	void GMFNode::SetMatrix(const float* mat)
	{
		for (int n = 0; n < 16; ++n)
		{
			matrix[n] = mat[n];
		}
		//memcpy(&matrix[0], mat, sizeof(float) * 16);
		definedvalues |= GMF_NODE_MATRIX;
	}

	void GMFNode::SetMatrix(const double* mat)
	{
		memcpy(&matrix[0], mat, sizeof(mat[0]) * 16);
		definedvalues |= GMF_NODE_MATRIX;
	}
}
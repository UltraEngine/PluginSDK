#include "MDL.h"

using namespace UltraEngine::PluginSDK;
using namespace std;

const float anorms[255 * 3] =
{ -0.525731f, 0.000000f, 0.850651f ,
 -0.442863f,  0.238856f,  0.864188f ,
 -0.295242f,  0.000000f,  0.955423f ,
 -0.309017f,  0.500000f,  0.809017f ,
 -0.162460f,  0.262866f,  0.951056f ,
 0.000000f,  0.000000f,  1.000000f ,
 0.000000f,  0.850651f,  0.525731f ,
 -0.147621f,  0.716567f,  0.681718f ,
 0.147621f,  0.716567f,  0.681718f ,
 0.000000f,  0.525731f,  0.850651f ,
 0.309017f,  0.500000f,  0.809017f ,
 0.525731f,  0.000000f,  0.850651f ,
 0.295242f,  0.000000f,  0.955423f ,
 0.442863f,  0.238856f,  0.864188f ,
 0.162460f,  0.262866f,  0.951056f ,
 -0.681718f,  0.147621f,  0.716567f ,
 -0.809017f,  0.309017f,  0.500000f ,
 -0.587785f,  0.425325f,  0.688191f ,
 -0.850651f,  0.525731f,  0.000000f ,
 -0.864188f,  0.442863f,  0.238856f ,
 -0.716567f,  0.681718f,  0.147621f ,
 -0.688191f,  0.587785f,  0.425325f ,
 -0.500000f,  0.809017f,  0.309017f ,
 -0.238856f,  0.864188f,  0.442863f ,
 -0.425325f,  0.688191f,  0.587785f ,
 -0.716567f,  0.681718f, -0.147621f ,
 -0.500000f,  0.809017f, -0.309017f ,
 -0.525731f,  0.850651f,  0.000000f ,
 0.000000f,  0.850651f, -0.525731f ,
 -0.238856f,  0.864188f, -0.442863f ,
 0.000000f,  0.955423f, -0.295242f ,
 -0.262866f,  0.951056f, -0.162460f ,
 0.000000f,  1.000000f,  0.000000f ,
 0.000000f,  0.955423f,  0.295242f ,
 -0.262866f,  0.951056f,  0.162460f ,
 0.238856f,  0.864188f,  0.442863f ,
 0.262866f,  0.951056f,  0.162460f ,
 0.500000f,  0.809017f,  0.309017f ,
 0.238856f,  0.864188f, -0.442863f ,
 0.262866f,  0.951056f, -0.162460f ,
 0.500000f,  0.809017f, -0.309017f ,
 0.850651f,  0.525731f,  0.000000f ,
 0.716567f,  0.681718f,  0.147621f ,
 0.716567f,  0.681718f, -0.147621f ,
 0.525731f,  0.850651f,  0.000000f ,
 0.425325f,  0.688191f,  0.587785f ,
 0.864188f,  0.442863f,  0.238856f ,
 0.688191f,  0.587785f,  0.425325f ,
 0.809017f,  0.309017f,  0.500000f ,
 0.681718f,  0.147621f,  0.716567f ,
 0.587785f,  0.425325f,  0.688191f ,
 0.955423f,  0.295242f,  0.000000f ,
 1.000000f,  0.000000f,  0.000000f ,
 0.951056f,  0.162460f,  0.262866f ,
 0.850651f, -0.525731f,  0.000000f ,
 0.955423f, -0.295242f,  0.000000f ,
 0.864188f, -0.442863f,  0.238856f ,
 0.951056f, -0.162460f,  0.262866f ,
 0.809017f, -0.309017f,  0.500000f ,
 0.681718f, -0.147621f,  0.716567f ,
 0.850651f,  0.000000f,  0.525731f ,
 0.864188f,  0.442863f, -0.238856f ,
 0.809017f,  0.309017f, -0.500000f ,
 0.951056f,  0.162460f, -0.262866f ,
 0.525731f,  0.000000f, -0.850651f ,
 0.681718f,  0.147621f, -0.716567f ,
 0.681718f, -0.147621f, -0.716567f ,
 0.850651f,  0.000000f, -0.525731f ,
 0.809017f, -0.309017f, -0.500000f ,
 0.864188f, -0.442863f, -0.238856f ,
 0.951056f, -0.162460f, -0.262866f ,
 0.147621f,  0.716567f, -0.681718f ,
 0.309017f,  0.500000f, -0.809017f ,
 0.425325f,  0.688191f, -0.587785f ,
 0.442863f,  0.238856f, -0.864188f ,
 0.587785f,  0.425325f, -0.688191f ,
 0.688191f,  0.587785f, -0.425325f ,
 -0.147621f,  0.716567f, -0.681718f ,
 -0.309017f,  0.500000f, -0.809017f ,
 0.000000f,  0.525731f, -0.850651f ,
 -0.525731f,  0.000000f, -0.850651f ,
 -0.442863f,  0.238856f, -0.864188f ,
 -0.295242f,  0.000000f, -0.955423f ,
 -0.162460f,  0.262866f, -0.951056f ,
 0.000000f,  0.000000f, -1.000000f ,
 0.295242f,  0.000000f, -0.955423f ,
 0.162460f,  0.262866f, -0.951056f ,
 -0.442863f, -0.238856f, -0.864188f ,
 -0.309017f, -0.500000f, -0.809017f ,
 -0.162460f, -0.262866f, -0.951056f ,
 0.000000f, -0.850651f, -0.525731f ,
 -0.147621f, -0.716567f, -0.681718f ,
 0.147621f, -0.716567f, -0.681718f ,
 0.000000f, -0.525731f, -0.850651f ,
 0.309017f, -0.500000f, -0.809017f ,
 0.442863f, -0.238856f, -0.864188f ,
 0.162460f, -0.262866f, -0.951056f ,
 0.238856f, -0.864188f, -0.442863f ,
 0.500000f, -0.809017f, -0.309017f ,
 0.425325f, -0.688191f, -0.587785f ,
 0.716567f, -0.681718f, -0.147621f ,
 0.688191f, -0.587785f, -0.425325f ,
 0.587785f, -0.425325f, -0.688191f ,
 0.000000f, -0.955423f, -0.295242f ,
 0.000000f, -1.000000f,  0.000000f ,
 0.262866f, -0.951056f, -0.162460f ,
 0.000000f, -0.850651f,  0.525731f ,
 0.000000f, -0.955423f,  0.295242f ,
 0.238856f, -0.864188f,  0.442863f ,
 0.262866f, -0.951056f,  0.162460f ,
 0.500000f, -0.809017f,  0.309017f ,
 0.716567f, -0.681718f,  0.147621f ,
 0.525731f, -0.850651f,  0.000000f ,
 -0.238856f, -0.864188f, -0.442863f ,
 -0.500000f, -0.809017f, -0.309017f ,
 -0.262866f, -0.951056f, -0.162460f ,
 -0.850651f, -0.525731f,  0.000000f ,
 -0.716567f, -0.681718f, -0.147621f ,
 -0.716567f, -0.681718f,  0.147621f ,
 -0.525731f, -0.850651f,  0.000000f ,
 -0.500000f, -0.809017f,  0.309017f ,
 -0.238856f, -0.864188f,  0.442863f ,
 -0.262866f, -0.951056f,  0.162460f ,
 -0.864188f, -0.442863f,  0.238856f ,
 -0.809017f, -0.309017f,  0.500000f ,
 -0.688191f, -0.587785f,  0.425325f ,
 -0.681718f, -0.147621f,  0.716567f ,
 -0.442863f, -0.238856f,  0.864188f ,
 -0.587785f, -0.425325f,  0.688191f ,
 -0.309017f, -0.500000f,  0.809017f ,
 -0.147621f, -0.716567f,  0.681718f ,
 -0.425325f, -0.688191f,  0.587785f ,
 -0.162460f, -0.262866f,  0.951056f ,
 0.442863f, -0.238856f,  0.864188f ,
 0.162460f, -0.262866f,  0.951056f ,
 0.309017f, -0.500000f,  0.809017f ,
 0.147621f, -0.716567f,  0.681718f ,
 0.000000f, -0.525731f,  0.850651f ,
 0.425325f, -0.688191f,  0.587785f ,
 0.587785f, -0.425325f,  0.688191f ,
 0.688191f, -0.587785f,  0.425325f ,
 -0.955423f,  0.295242f,  0.000000f ,
 -0.951056f,  0.162460f,  0.262866f ,
 -1.000000f,  0.000000f,  0.000000f ,
 -0.850651f,  0.000000f,  0.525731f ,
 -0.955423f, -0.295242f,  0.000000f ,
 -0.951056f, -0.162460f,  0.262866f ,
 -0.864188f,  0.442863f, -0.238856f ,
 -0.951056f,  0.162460f, -0.262866f ,
 -0.809017f,  0.309017f, -0.500000f ,
 -0.864188f, -0.442863f, -0.238856f ,
 -0.951056f, -0.162460f, -0.262866f ,
 -0.809017f, -0.309017f, -0.500000f ,
 -0.681718f,  0.147621f, -0.716567f ,
 -0.681718f, -0.147621f, -0.716567f ,
 -0.850651f,  0.000000f, -0.525731f ,
 -0.688191f,  0.587785f, -0.425325f ,
 -0.587785f,  0.425325f, -0.688191f ,
 -0.425325f,  0.688191f, -0.587785f ,
 -0.425325f, -0.688191f, -0.587785f ,
 -0.587785f, -0.425325f, -0.688191f ,
 -0.688191f, -0.587785f, -0.425325f };

void* LoadModelMDL(Context* context, void* data, uint64_t size, wchar_t* cpath, uint64_t& returnsize)
{
	MemReader reader(data, size);
	mdl_header_t header;
	if (sizeof(mdl_header_t) > size) return NULL;
	reader.Read(&header);
	if (header.ident != 1330660425) return NULL;
	if (header.version != 6) return NULL;

	std::vector<std::vector<unsigned char> > skins;
	skins.reserve(header.num_skins);

	std::vector<unsigned char> pixels(header.skinwidth * header.skinheight);

	/* Read texture data */
	for (int i = 0; i < header.num_skins; ++i)
	{
		int group = 0;
		reader.Read(&group);
		if (group == 0)
		{
			//reader.Seek(reader.Pos() + header.skinwidth * header.skinheight);
			reader.Read(pixels.data(), pixels.size());
			skins.push_back(pixels);
		}
		else
		{
			int nb; float time;
			reader.Read(&nb);
			for (int n = 0; n < nb; ++n)
			{
				reader.Read(&time);
			}
			for (int n = 0; n < nb; ++n)
			{
				//reader.Seek(reader.Pos() + header.skinwidth * header.skinheight);
				reader.Read(pixels.data(), pixels.size());
				if (n == 0) skins.push_back(pixels);
			}
		}
	}

	std::vector<mdl_vertex_t> vertices(header.num_verts);
	std::vector<mdl_texcoord_t> texcoords(header.num_verts);
	std::vector<mdl_triangle_t> triangles(header.num_tris);

	reader.Read(texcoords.data(), sizeof(mdl_texcoord_t) * header.num_verts);
	reader.Read(triangles.data(), sizeof(mdl_triangle_t) * header.num_tris);

	for (int i = 0; i < header.num_frames; ++i)
	{
		int type;
		reader.Read(&type);
		if (type == 0)
		{
			mdl_simpleframe_t frame = {};
			reader.Read(&frame);
			//auto offset = uint64_t(frame.verts);
			//reader.Seek(offset);
			reader.Read(vertices.data(), sizeof(mdl_vertex_t) * header.num_verts);
		}
		else
		{
			mdl_groupframe_t gframe = {};
			reader.Read(&gframe);
			mdl_simpleframe_t frame = {};
			reader.Read(&frame);
			reader.Read(vertices.data(), sizeof(mdl_vertex_t) * header.num_verts);
		}
		break;
	}

	auto file = new GMFFile;
	auto root = new GMFNode(file, GMF_TYPE_MODEL);
	auto mesh = new GMFMesh(file);
	auto lod = root->AddLOD();
	lod->AddMesh(mesh);

	mesh->vertices.resize(header.num_verts);
	mesh->indices.resize(header.num_tris * 3);

	for (int n = 0; n < skins.size(); ++n)
	{
		//auto tex = new GMFTexture(file);
		//tex->path = L"skin" + WString(String(n));
		//tex->datasize = header.skinwidth * header.skinheight + 8;
		//tex->data = malloc(tex->datasize);
		//memcpy(tex->data, &header.skinwidth, 4);
		//memcpy((char*)tex->data + 4, &header.skinheight, 4);
		//memcpy((char*)tex->data + 8, skins[n].data(), header.skinwidth * header.skinheight);

		TextureInfo texinfo;
		texinfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		texinfo.width = header.skinwidth;
		texinfo.height = header.skinheight;
		texinfo.mipmaps = 1;
		texinfo.mipchain.resize(1);

		texinfo.mipchain[0].width = header.skinwidth;
		texinfo.mipchain[0].height = header.skinheight;
		texinfo.mipchain[0].size = header.skinwidth * header.skinheight * 4;
		auto data = malloc(header.skinwidth * header.skinheight * 4);;
		texinfo.mipchain[0].data = data;
		context->memblocks.push_back(data);
		char rgba[4];
		for (int k = 0; k < pixels.size(); ++k)
		{
			int color = qpallete[pixels[k]];
			memcpy(&rgba[0], &color, 4);
			rgba[3] = 255;
			void* dst = (void*)(((const char*)data) + uint64_t(k * 4));
			memcpy(dst, &rgba[0], 4);
		}

		//memcpy((char*)data, skins[n].data(), header.skinwidth * header.skinheight * 4);

		file->AddTexture(texinfo);
	}

	auto mtl = new GMFMaterial(file);
	mtl->text = "{\"material\":{"
					"\"texture0\": 0"
				"}}";
	mtl->path = L"material0.json";
	mtl->data = (void*)mtl->text.c_str();
	mtl->datasize = mtl->text.size();

	std::map<int, int> flippedverts;
	for (int t = 0; t < header.num_tris; ++t)
	{
		if (triangles[t].facesfront == 0)
		{
			for (int n = 0; n < 3; ++n)
			{
				int i = triangles[t].vertex[n];
				if (texcoords[i].onseam != 0)
				{
					if (flippedverts.find(i) == flippedverts.end())
					{
						vertices.push_back(vertices[i]);
						texcoords.push_back(texcoords[i]);
						texcoords[texcoords.size() - 1].s += header.skinwidth / 2;
						flippedverts[i] = vertices.size() - 1;
					}
					triangles[t].vertex[n] = flippedverts[i];
				}
			}
		}
		mesh->indices[t * 3 + 0] = triangles[t].vertex[2];
		mesh->indices[t * 3 + 1] = triangles[t].vertex[1];
		mesh->indices[t * 3 + 2] = triangles[t].vertex[0];
	}

	mesh->vertices.resize(vertices.size());
	for (int v = 0; v < vertices.size(); ++v)
	{
		mesh->vertices[v].position.x = float(vertices[v].v[1]) / 127.0f * header.scale.y + header.translate.y;
		mesh->vertices[v].position.y = float(vertices[v].v[2]) / 127.0f * header.scale.z + header.translate.z;
		mesh->vertices[v].position.z = float(vertices[v].v[0]) / 127.0f * header.scale.x + header.translate.x;
		mesh->vertices[v].texcoords.x = ((float(texcoords[v].s) + 0.5f) / float(header.skinwidth));
		mesh->vertices[v].texcoords.y = ((float(texcoords[v].t) + 0.5f) / float(header.skinheight));
		mesh->vertices[v].normal.x = anorms[vertices[v].normalIndex * 3 + 1];
		mesh->vertices[v].normal.y = anorms[vertices[v].normalIndex * 3 + 2];
		mesh->vertices[v].normal.z = anorms[vertices[v].normalIndex * 3 + 0];
	}

	mesh->SetMaterial(mtl);

	int flags = 0;
	file->Save(&context->writer,flags);
	delete file;
	returnsize = context->writer.Size();
	return context->writer.data();
}
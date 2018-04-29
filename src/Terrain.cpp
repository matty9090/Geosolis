#include "Terrain.hpp"
#include "SimplexNoise.hpp"

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

Terrain::Terrain() {
	mMesh = new irr::scene::SMesh();

	SMeshBuffer *buf = new SMeshBuffer();
	mMesh->addMeshBuffer(buf);

	s32 width = 128, height = 128;
	u32 numIndices = 6 * (width - 1) * (height - 1);

	buf->Vertices.set_used(width * height);
	buf->Indices.set_used(numIndices);

	SimplexNoise noise(1.2f, 1.0f, 2.2f, 0.4f);

	u32 i = 0;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			S3DVertex &v = buf->Vertices[i++];
			v.Pos.set(x - (width / 2), noise.fractal(9, x / 50.0f, y / 50.0f) * 5.0f, y - (height / 2));
			v.Color.set(0x003300);
			v.TCoords.set(((f32)x / (f32)width) * 10.0f, ((f32)y / (f32)height) * 10.0f);
		}
	}

	i = 0;

	for (int y = 0; y < height - 1; ++y) {
		for (int x = 0; x < width - 1; ++x) {
			buf->Indices[i++] = (y    ) * width + x;
			buf->Indices[i++] = (y    ) * width + x + 1;
			buf->Indices[i++] = (y + 1) * width + x;
			buf->Indices[i++] = (y + 1) * width + x;
			buf->Indices[i++] = (y    ) * width + x + 1;
			buf->Indices[i++] = (y + 1) * width + x + 1;
		}
	}

	for (i = 0; i < numIndices - 2; ++i) {
		vector3df p1 = buf->Vertices[buf->Indices[i]].Pos;
		vector3df p2 = buf->Vertices[buf->Indices[i + 1]].Pos;
		vector3df p3 = buf->Vertices[buf->Indices[i + 2]].Pos;

		vector3df u = p2 - p1, v = p3 - p1;
		vector3df n = v.crossProduct(u).normalize();

		buf->Vertices[buf->Indices[i]].Normal = n;
	}
}

Terrain::~Terrain() {
	mMesh->drop();
}
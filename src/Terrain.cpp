#include "Terrain.hpp"
#include <iostream>

using namespace irr;
using namespace irr::scene;
using namespace irr::video;

Terrain::Terrain() {
	mMesh = new irr::scene::SMesh();

	std::cout << mMesh->getMeshBufferCount() << "\n";

	SMeshBuffer *buf = new SMeshBuffer();
	mMesh->addMeshBuffer(buf);

	int width = 64, height = 64;

	buf->Vertices.set_used(width * height);
	buf->Indices.set_used(6 * (width - 1) * height);

	u32 i = 0;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			S3DVertex &v = buf->Vertices[i++];
			v.Pos.set(x - 32, 0.0f, y - 32);
			v.Color.set(0x0000ff);
			v.Normal.set(0.0f, 1.0f, 0.0f);
			v.TCoords.set((f32)x / (f32)width, (f32)x / (f32)height);
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
}

Terrain::~Terrain() {
	mMesh->drop();
}
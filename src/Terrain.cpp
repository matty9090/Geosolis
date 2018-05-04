#include "Terrain.hpp"
#include "SimplexNoise.hpp"
#include "TerrainNode.hpp"

#include <iostream>

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

Terrain::Terrain(irr::scene::ISceneManager *scene, irr::video::IVideoDriver *driver, irr::scene::ICameraSceneNode *cam)
	: mScene(scene), mDriver(driver), mCamera(cam), mBounds(-0.5f, -0.5f, 0.5f, 0.5f)
{
	for (auto &face : mFaces)
		face = new TerrainNode(nullptr, this, mBounds);

	mFaces[Top   ]->setRotation(vector3df(   0.0f, 0.0f,   0.0f));
	mFaces[Bottom]->setRotation(vector3df( 180.0f, 0.0f,   0.0f));
	mFaces[Right ]->setRotation(vector3df(   0.0f, 0.0f,  90.0f));
	mFaces[Left  ]->setRotation(vector3df(   0.0f, 0.0f, -90.0f));
	mFaces[Front ]->setRotation(vector3df( -90.0f, 0.0f,   0.0f));
	mFaces[Back  ]->setRotation(vector3df(  90.0f, 0.0f,   0.0f));

	mFaces[Top   ]->setFaceNeighbours(mFaces[Back ], mFaces[Right], mFaces[Front ], mFaces[Left ]);
	mFaces[Bottom]->setFaceNeighbours(mFaces[Front], mFaces[Right], mFaces[Back  ], mFaces[Left ]);
	mFaces[Right ]->setFaceNeighbours(mFaces[Top  ], mFaces[Back ], mFaces[Bottom], mFaces[Front]);
	mFaces[Left  ]->setFaceNeighbours(mFaces[Top  ], mFaces[Front], mFaces[Bottom], mFaces[Back ]);
	mFaces[Front ]->setFaceNeighbours(mFaces[Top  ], mFaces[Right], mFaces[Bottom], mFaces[Left ]);
	mFaces[Back  ]->setFaceNeighbours(mFaces[Top  ], mFaces[Left ], mFaces[Bottom], mFaces[Right]);

	for (auto &face : mFaces)
		face->init();
	
	mNoise.SetFrequency(1.0f);
	mNoise.SetPersistence(0.5f);
	mNoise.SetLacunarity(2.0f);

	utils::NoiseMapBuilderSphere builder;
	builder.SetSourceModule(mNoise);
	builder.SetDestNoiseMap(mHeightmap);
	builder.SetDestSize(512, 256);
	builder.SetBounds(-90.0, 90.0, -180.0, 180.0);
	builder.Build();
}

Terrain::~Terrain() {
	for (auto &face : mFaces) {
		face->cleanup();
		delete face;
	}
}

void Terrain::update() {
	for (auto &face : mFaces)
		face->update();
}

float Terrain::getHeight(float x, float y) const {
	float h = (float)mNoise.GetValue((double)x * 10.0, (double)y * 10.0, 0.0) * 2.0f;
	return h;
}

vector3df Terrain::project(vector3df p) const {
	float x2 = p.X * p.X, y2 = p.Y * p.Y, z2 = p.Z * p.Z;

	return vector3df(p.X * sqrtf(1.0f - y2 * 0.5f - z2 * 0.5f + (y2 * z2) * 0.33333f),
					 p.Y * sqrtf(1.0f - z2 * 0.5f - x2 * 0.5f + (z2 * x2) * 0.33333f),
					 p.Z * sqrtf(1.0f - x2 * 0.5f - y2 * 0.5f + (x2 * y2) * 0.33333f));
}

void Terrain::setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value) const {
	for (auto &face : mFaces)
		face->getSceneNode()->setMaterialFlag(flag, value);
}

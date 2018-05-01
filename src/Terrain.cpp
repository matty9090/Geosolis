#include "Terrain.hpp"
#include "SimplexNoise.hpp"
#include "TerrainNode.hpp"

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

Terrain::Terrain(irr::scene::ISceneManager *scene, irr::video::IVideoDriver *driver, irr::scene::ICameraSceneNode *cam)
	: mScene(scene), mDriver(driver), mCamera(cam)
{
	mRoot = new TerrainNode(nullptr, this, rectf(-100.0f, -100.0f, 100.0f, 100.0f));
	mRoot->init();

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
	mRoot->cleanup();

	delete mRoot;
}

void Terrain::update() {
	mRoot->update();
}

float Terrain::getHeight(float x, float y) {
	return (float)mNoise.GetValue((double)x / 50.0, (double)y / 50.0, 0.0) * 9.0f;
}

TerrainNode *Terrain::getTerrainNode() const {
	return mRoot;
}

irr::scene::ISceneNode * Terrain::getSceneNode() const {
	return mRoot->getSceneNode();
}

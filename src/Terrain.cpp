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
}

Terrain::~Terrain() {
	mRoot->cleanup();

	delete mRoot;
}

void Terrain::update() {
	mRoot->update();
}

TerrainNode *Terrain::getTerrainNode() const {
	return mRoot;
}

irr::scene::ISceneNode * Terrain::getSceneNode() const {
	return mRoot->getSceneNode();
}

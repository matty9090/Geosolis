#include "Terrain.hpp"
#include "SimplexNoise.hpp"
#include "TerrainNode.hpp"

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

Terrain::Terrain(irr::scene::ISceneManager *scene, irr::video::IVideoDriver *driver) : mScene(scene), mDriver(driver) {
	mRoot = new TerrainNode(nullptr, this, rectf(-0.5f, -0.5f, 0.5f, 0.5f));
}

Terrain::~Terrain() {
	mRoot->cleanup();

	delete mRoot;
}

TerrainNode *Terrain::getTerrainNode() const {
	return mRoot;
}

irr::scene::ISceneNode * Terrain::getSceneNode() const {
	return mRoot->getSceneNode();
}

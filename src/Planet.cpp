#include "Planet.hpp"

Planet::Planet(irr::IrrlichtDevice *device, irr::f64 mass, irr::f64 radius)
	: mDevice(device),
	mMass(mass),
	mRadius(radius)
{
	mTerrain = new Terrain(device, (irr::f32)(mRadius / WORLD_SCALE));
}

Planet::~Planet() {
	delete mTerrain;
}

void Planet::update(float dt) {
	mTerrain->update();
}

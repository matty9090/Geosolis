#include "Planet.hpp"

using namespace irr;
using namespace core;

Planet::Planet(irr::IrrlichtDevice *device, irr::f64 mass, irr::f64 radius)
	: mDevice(device),
	mMass(mass),
	mRadius(radius)
{
	mSceneNode = device->getSceneManager()->addEmptySceneNode();
	mTerrain = new Terrain(device, mSceneNode, (irr::f32)(mRadius / WORLD_SCALE));
	//mAtmosphere = device->getSceneManager()->addSphereSceneNode(;
}

Planet::~Planet() {
	delete mTerrain;
}

void Planet::update(f32 dt) {
	vector3d<f64> force;

	for (auto &body : mInfluences)
		force += gravity(body);

	mVelocity += dt * (force / mMass);
	
	rotate(vector3d<f64>(0.0, dt, 0.0));
	move(mVelocity * dt);
	mTerrain->update();
}

void Planet::move(irr::core::vector3d<irr::f64> m) {
	mPosition += m;
	mSceneNode->setPosition(vector3df((f32)mPosition.X, (f32)mPosition.Y, (f32)mPosition.Z));
	mTerrain->setPosition(vector3df((f32)mPosition.X, (f32)mPosition.Y, (f32)mPosition.Z));
}

void Planet::rotate(irr::core::vector3d<irr::f64> r) {
	mRotation += r;
	mSceneNode->setRotation(vector3df((f32)mRotation.X, (f32)mRotation.Y, (f32)mRotation.Z));
	mTerrain->setRotation(vector3df((f32)mRotation.X, (f32)mRotation.Y, (f32)mRotation.Z));
}

void Planet::setPosition(irr::core::vector3df pos) {
	mPosition = vector3d<f64>(pos.X, pos.Y, pos.Z);
	mSceneNode->setPosition(pos);
	mTerrain->setPosition(vector3df((f32)mPosition.X, (f32)mPosition.Y, (f32)mPosition.Z));
}

void Planet::setVelocity(irr::core::vector3df vel) {
	mVelocity = vector3d<f64>(vel.X, vel.Y, vel.Z);
}

void Planet::setRotation(irr::core::vector3df rot) {
	mRotation = vector3d<f64>((f32)rot.X, (f32)rot.Y, (f32)rot.Z);
	mSceneNode->setRotation(rot);
	mTerrain->setRotation(rot);
}

irr::core::vector3d<f64> Planet::gravity(Planet *p) {
	double dx = p->getPosition().X - mPosition.X;
	double dy = p->getPosition().Y - mPosition.Y;
	double dz = p->getPosition().Z - mPosition.Z;

	double r = sqrt((dx * dx) + (dy * dy) + (dz * dz)) * WORLD_SCALE;
	double force = (G * mMass * p->getMass()) / (r * r);

	return vector3d<f64>(force * dx / r, force * dy / r, force * dz / r);
}

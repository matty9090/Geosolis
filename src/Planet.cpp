#include "Planet.hpp"

using namespace irr;
using namespace core;

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

void Planet::update(f32 dt) {
	vector3d<f64> force;

	for (auto &body : mInfluences)
		force += gravity(body);

	mVelocity += dt * (force / mMass);
	mPosition += mVelocity * dt;

	mTerrain->rotate(vector3df(0.0f, dt, 0.0f));
	mTerrain->setPosition(vector3df((f32)mPosition.X, (f32)mPosition.Y, (f32)mPosition.Z));
	mTerrain->update();
}

void Planet::setPosition(irr::core::vector3df pos) {
	mPosition = vector3d<f64>(pos.X, pos.Y, pos.Z);
	mTerrain->setPosition(pos);
}

void Planet::setVelocity(irr::core::vector3df vel) {
	mVelocity = vector3d<f64>(vel.X, vel.Y, vel.Z);
}

irr::core::vector3d<f64> Planet::gravity(Planet *p) {
	double dx = p->getPosition().X - mPosition.X;
	double dy = p->getPosition().Y - mPosition.Y;
	double dz = p->getPosition().Z - mPosition.Z;

	double r = sqrt((dx * dx) + (dy * dy) + (dz * dz)) * WORLD_SCALE;
	double force = (G * mMass * p->getMass()) / (r * r);

	return vector3d<f64>(force * dx / r, force * dy / r, force * dz / r);
}

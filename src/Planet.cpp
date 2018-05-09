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
	vector3df force;

	for (auto &body : mInfluences)
		force += gravity(body);

	mVelocity += 1000.0f * dt * (force / mMass);
	mPosition += 1000.0f * mVelocity * dt;

	mTerrain->rotate(vector3df(0.0f, 10.0f * dt, 0.0f));
	mTerrain->setPosition(mPosition);
	mTerrain->update();
}

void Planet::setPosition(irr::core::vector3df pos) {
	mPosition = pos;
	mTerrain->setPosition(pos);
}

void Planet::setVelocity(irr::core::vector3df vel) {
	mVelocity = vel;
}

irr::core::vector3df Planet::gravity(Planet *p) {
	float dx = p->getPosition().X - mPosition.X;
	float dy = p->getPosition().Y - mPosition.Y;
	float dz = p->getPosition().Z - mPosition.Z;

	float r = sqrt((dx * dx) + (dy * dy) + (dz * dz)) * WORLD_SCALE;
	float force = (G * mMass * p->getMass()) / (r * r);

	return vector3df(force * dx / r, force * dy / r, force * dz / r);
}

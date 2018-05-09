#include "Planet.hpp"

using namespace irr;
using namespace core;
using namespace video;

Planet::Planet(irr::IrrlichtDevice *device, irr::f64 mass, irr::f64 radius)
	: mDevice(device),
	mMass(mass),
	mRadius(radius)
{
	mSceneNode = device->getSceneManager()->addEmptySceneNode();
	mTerrain = new Terrain(device, mSceneNode, (irr::f32)(mRadius / WORLD_SCALE));
	mAtmosphere = device->getSceneManager()->addSphereSceneNode((irr::f32)(mRadius / WORLD_SCALE) * 1.08f, 32, mSceneNode);

	IGPUProgrammingServices *gpu = device->getVideoDriver()->getGPUProgrammingServices();
	AtmosphereShader *atmosphereShader = new AtmosphereShader();

	s32 material = gpu->addHighLevelShaderMaterialFromFiles("shaders/AtmosphereVS.fx", "main", EVST_VS_3_0, "shaders/AtmospherePS.fx", "main", EPST_PS_3_0, atmosphereShader, EMT_TRANSPARENT_ALPHA_CHANNEL);
	atmosphereShader->drop();

	mAtmosphere->setMaterialType((E_MATERIAL_TYPE)material);
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

void AtmosphereShader::OnSetConstants(IMaterialRendererServices *services, s32 userData) {
	IVideoDriver *driver = services->getVideoDriver();

	matrix4 worldViewProj;
	worldViewProj = driver->getTransform(video::ETS_PROJECTION);
	worldViewProj *= driver->getTransform(video::ETS_VIEW);
	worldViewProj *= driver->getTransform(video::ETS_WORLD);

	vector3df lightPos = vector3df(-10000.0f, 0.0f, -1000.0f);
	SColorf col(1.0f, 1.0f, 1.0f, 0.0f);
	SColorf ambient(0.008f, 0.008f, 0.008f, 0.0f);

	matrix4 world = driver->getTransform(video::ETS_WORLD);

	services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
	services->setVertexShaderConstant("mWorld", world.pointer(), 16);
	/*services->setPixelShaderConstant("mWorld", world.pointer(), 16);
	services->setPixelShaderConstant("mAmbient", reinterpret_cast<f32*>(&ambient), 4);
	services->setPixelShaderConstant("mLightPos", reinterpret_cast<f32*>(&lightPos), 3);
	services->setPixelShaderConstant("mLightColour", reinterpret_cast<f32*>(&col), 4);*/
}

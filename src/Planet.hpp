#include <irr/irrlicht.h>
#include "Terrain.hpp"

#define WORLD_SCALE 32350

class Planet {
	public:
		Planet(irr::IrrlichtDevice *device, irr::f64 mass, irr::f64 radius);
		~Planet();

		void update(float dt);

	private:
		irr::f64 mMass;
		irr::f64 mRadius;

		irr::core::vector2d<irr::f64> mVelocity;
		irr::core::vector2d<irr::f64> mPosition;

		Terrain *mTerrain;

		irr::IrrlichtDevice *mDevice;
};
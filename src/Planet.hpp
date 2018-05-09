#include <irr/irrlicht.h>
#include <vector>

#include "Terrain.hpp"

#define WORLD_SCALE 32350
#define G 6.674e-11

class Planet {
	public:
		Planet(irr::IrrlichtDevice *device, irr::f64 mass, irr::f64 radius);
		~Planet();

		void update(irr::f32 dt);
		void addInflucence(Planet *p) { mInfluences.push_back(p); }
		void setPosition(irr::core::vector3df pos);
		void setVelocity(irr::core::vector3df vel);

		Terrain					*getTerrain()	const { return mTerrain;	}
		irr::f64				 getMass()		const { return mMass;		}
		irr::f64				 getRadius()	const { return mRadius;		}
		irr::core::vector3df	 getPosition()	const { return mPosition;	}

	private:
		irr::f64 mMass;
		irr::f64 mRadius;

		irr::core::vector3df mVelocity;
		irr::core::vector3df mPosition;

		Terrain *mTerrain;

		irr::IrrlichtDevice *mDevice;
		std::vector<Planet*> mInfluences;

		irr::core::vector3df gravity(Planet *p);
};
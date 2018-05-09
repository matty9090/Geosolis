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

		void move(irr::core::vector3d<irr::f64> m);
		void rotate(irr::core::vector3d<irr::f64> r);

		void setVelocity(irr::core::vector3df vel);
		void setPosition(irr::core::vector3df pos);
		void setRotation(irr::core::vector3df rot);

		Terrain							*getTerrain()	const { return mTerrain;	}
		irr::f64						 getMass()		const { return mMass;		}
		irr::f64						 getRadius()	const { return mRadius;		}
		irr::core::vector3d<irr::f64>	 getVelocity()	const { return mVelocity; }
		irr::core::vector3d<irr::f64>	 getPosition()	const { return mPosition;	}
		irr::core::vector3d<irr::f64>	 getRotation()	const { return mRotation;	}

	private:
		irr::f64 mMass;
		irr::f64 mRadius;

		irr::core::vector3d<irr::f64> mVelocity;
		irr::core::vector3d<irr::f64> mPosition;
		irr::core::vector3d<irr::f64> mRotation;

		Terrain *mTerrain;

		irr::scene::ISceneNode *mSceneNode;
		irr::scene::ISceneNode *mAtmosphere;
		irr::IrrlichtDevice *mDevice;
		std::vector<Planet*> mInfluences;

		irr::core::vector3d<irr::f64> gravity(Planet *p);
};
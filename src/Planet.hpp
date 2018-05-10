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
		irr::f32						 getCamHeight()	const;
		irr::f64						 getMass()		const { return mMass;		}
		irr::f64						 getRadius()	const { return mRadius;		}
		irr::f64						 getAtmRadius()	const { return mAtmRadius;	}
		irr::core::vector3d<irr::f64>	 getVelocity()	const { return mVelocity;	}
		irr::core::vector3d<irr::f64>	 getPosition()	const { return mPosition;	}
		irr::core::vector3df			 getLightDir()	const;
		irr::core::vector3df			 getLocalCam()	const;

	private:
		irr::f64 mMass;
		irr::f64 mRadius, mAtmRadius;

		irr::core::vector3d<irr::f64> mVelocity;
		irr::core::vector3d<irr::f64> mPosition;
		irr::core::vector3d<irr::f64> mRotation;

		Terrain *mTerrain;

		irr::scene::ISceneNode *mSceneNode;
		irr::scene::IMeshSceneNode *mAtmosphere;
		irr::IrrlichtDevice *mDevice;
		std::vector<Planet*> mInfluences;

		irr::core::vector3d<irr::f64> gravity(Planet *p);
};

class AtmosphereShader : public irr::video::IShaderConstantSetCallBack {
	public:
		AtmosphereShader(Planet *p) : mPlanet(p) {}

		virtual void OnSetConstants(irr::video::IMaterialRendererServices *services, irr::s32 userData);

	private:
		Planet *mPlanet;
};
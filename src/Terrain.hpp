#include <irr/irrlicht.h>
#include <noise/noise.h>
#include <array>

#include "noiseutils.h"

class TerrainNode;

#define GRID_SIZE 25

class Terrain {
	public:
		Terrain(irr::IrrlichtDevice *device, irr::f32 radius);
		~Terrain();
		
		enum EFace { Top, Bottom, Right, Left, Front, Back };

		void							 rotate(irr::core::vector3df rot);
		void							 setPosition(irr::core::vector3df pos);
		irr::core::vector3df			 getPosition()						const { return mPosition; }
		irr::core::vector3df			 getRotation()						const { return mRotation; }
		void							 update();
		irr::f32						 getRadius()						const { return mRadius; }
		irr::f32						 getHeight(float x, float y)		const;
		void							 setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value) const;
		irr::s32						 getMaterialType()					const { return mMaterialType; }
		irr::core::vector3df			 project(irr::core::vector3df p)	const;
		irr::scene::ISceneManager		*getSceneManager()					const { return mScene; }
		irr::video::IVideoDriver		*getVideoDriver()					const { return mDriver; }
		irr::scene::ICameraSceneNode	*getCamera()						const { return mScene->getActiveCamera(); }
		
	private:
		irr::f32 mRadius;
		irr::s32 mMaterialType;
		irr::core::vector3df mPosition, mRotation;
		irr::core::rectf mBounds;
		std::array<TerrainNode*, 6> mFaces;
		utils::NoiseMap mHeightmap;
		noise::module::Perlin mNoise;

		irr::video::IVideoDriver  *mDriver;
		irr::scene::ISceneManager *mScene;
		irr::video::IGPUProgrammingServices *mGPU;

		void generateHeightmap();
};

class TerrainShader : public irr::video::IShaderConstantSetCallBack {
	public:
		TerrainShader(Terrain *terrain) : mTerrain(terrain) {}

		virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

	private:
		Terrain *mTerrain;
};
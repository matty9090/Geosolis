#include <irr/irrlicht.h>
#include <noise/noise.h>
#include <array>

#include "noiseutils.h"

class TerrainNode;

#define GRID_SIZE 17

class Terrain {
	public:
		Terrain(irr::scene::ISceneManager *scene, irr::video::IVideoDriver *driver, irr::scene::ICameraSceneNode *cam);
		~Terrain();
		
		enum EFace { Top, Bottom, Right, Left, Front, Back };

		void							 update();
		float							 getHeight(float x, float y)		const;
		void							 setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value) const;
		irr::core::vector3df			 project(irr::core::vector3df p)	const;
		irr::scene::ISceneManager		*getSceneManager()					const { return mScene; }
		irr::video::IVideoDriver		*getVideoDriver()					const { return mDriver; }
		irr::scene::ICameraSceneNode	*getCamera()						const { return mCamera; }
		
	private:
		irr::core::rectf mBounds;
		std::array<TerrainNode*, 6> mFaces;
		utils::NoiseMap mHeightmap;
		noise::module::Perlin mNoise;

		irr::video::IVideoDriver  *mDriver;
		irr::scene::ISceneManager *mScene;
		irr::scene::ICameraSceneNode *mCamera;
};
#include <irr/irrlicht.h>
#include <noise/noise.h>

#include "noiseutils.h"

class TerrainNode;

#define GRID_SIZE 17

class Terrain {
	public:
		Terrain(irr::scene::ISceneManager *scene, irr::video::IVideoDriver *driver, irr::scene::ICameraSceneNode *cam);
		~Terrain();
		
		void update();
		float getHeight(float x, float y);

		TerrainNode						*getTerrainNode()	const;
		irr::scene::ISceneNode			*getSceneNode()		const;
		irr::scene::ISceneManager		*getSceneManager()	const { return mScene; }
		irr::video::IVideoDriver		*getVideoDriver()	const { return mDriver; }
		irr::scene::ICameraSceneNode	*getCamera()		const { return mCamera; }

	private:
		TerrainNode *mRoot;
		utils::NoiseMap mHeightmap;
		noise::module::Perlin mNoise;

		irr::video::IVideoDriver  *mDriver;
		irr::scene::ISceneManager *mScene;
		irr::scene::ICameraSceneNode *mCamera;
};
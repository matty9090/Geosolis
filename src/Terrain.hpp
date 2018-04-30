#include <irr/irrlicht.h>

class TerrainNode;

#define GRID_SIZE 33

class Terrain {
	public:
		Terrain(irr::scene::ISceneManager *scene, irr::video::IVideoDriver *driver, irr::scene::ICameraSceneNode *cam);
		~Terrain();
		
		void update();

		TerrainNode						*getTerrainNode()	const;
		irr::scene::ISceneNode			*getSceneNode()		const;
		irr::scene::ISceneManager		*getSceneManager()	const { return mScene; }
		irr::video::IVideoDriver		*getVideoDriver()	const { return mDriver; }
		irr::scene::ICameraSceneNode	*getCamera()		const { return mCamera; }

	private:
		TerrainNode *mRoot;

		irr::video::IVideoDriver  *mDriver;
		irr::scene::ISceneManager *mScene;
		irr::scene::ICameraSceneNode *mCamera;
};
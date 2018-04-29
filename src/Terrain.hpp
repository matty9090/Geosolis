#include <irr/irrlicht.h>

class TerrainNode;

class Terrain {
	public:
		Terrain(irr::scene::ISceneManager *scene, irr::video::IVideoDriver *driver);
		~Terrain();

		TerrainNode					*getTerrainNode()	const;
		irr::scene::ISceneNode		*getSceneNode()		const;
		irr::scene::ISceneManager	*getSceneManager()	const { return mScene; }
		irr::video::IVideoDriver	*getVideoDriver()	const { return mDriver; }

	private:
		TerrainNode *mRoot;

		irr::video::IVideoDriver  *mDriver;
		irr::scene::ISceneManager *mScene;
};
#include <irr/irrlicht.h>

#include "TerrainNode.hpp"

class Terrain {
	public:
		Terrain();
		~Terrain();

		irr::scene::SMesh *getMesh() const { return mMesh; }

		class Face {
			irr::scene::SMesh mesh;
		};

	private:
		irr::scene::SMesh *mMesh;

		TerrainNode *mRoot;
};
#include <irr/irrlicht.h>

class Terrain {
	public:
		Terrain();
		~Terrain();

		irr::scene::SMesh *getMesh() const { return mMesh; }

	private:
		irr::scene::SMesh *mMesh;
};
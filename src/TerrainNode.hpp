#include <irr/irrlicht.h>
#include <array>

class Terrain;

class TerrainNode {
	public:
		TerrainNode(TerrainNode *parent, Terrain *terrain, irr::core::rectf bounds);
		~TerrainNode();

		bool isLeaf() const { return mChildren[0] == nullptr; }
		void setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value);
		void cleanup();

		irr::scene::ISceneNode *getSceneNode() const { return mSceneNode; }

	private:
		irr::scene::SMesh *mMesh;
		irr::scene::ISceneNode *mSceneNode;

		Terrain *mTerrain;
		TerrainNode *mParent;

		std::array<TerrainNode*, 4> mChildren;

		void createMesh();
};
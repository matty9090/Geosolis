#include <irr/irrlicht.h>
#include <array>

class Terrain;

class TerrainNode {
	public:
		TerrainNode(TerrainNode *parent, Terrain *terrain, irr::core::rectf bounds);
		~TerrainNode();

		bool isLeaf() const { return mChildren[0] == nullptr; }
		void setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value);
		void update();
		void split();
		void merge();
		void cleanup();

		irr::u32				getDepth()	   const { return mDepth; }
		irr::scene::ISceneNode *getSceneNode() const { return mSceneNode; }

	private:
		irr::u32 mDepth;

		irr::core::vector3df mCentre;
		irr::core::rectf mBounds;

		irr::scene::SMesh *mMesh;
		irr::scene::ISceneNode *mSceneNode;

		irr::u32 mNumVertices, mNumIndices;

		Terrain *mTerrain;
		TerrainNode *mParent;

		std::array<TerrainNode*, 4> mChildren;

		void cleanData();
		void createMesh();
		void createPlane(irr::scene::SMeshBuffer *buf);
		void calculateNormals(irr::scene::SMeshBuffer *buf, bool smooth = true);
};
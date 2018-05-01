#include <irr/irrlicht.h>
#include <array>
#include <vector>

class Terrain;

class TerrainNode {
	public:
		TerrainNode(TerrainNode *parent, Terrain *terrain, irr::core::rectf bounds);
		~TerrainNode();

		enum EDir  { North, East, South, West };
		enum EQuad { NE, NW, SE, SW };

		void init() { cleanData(); createMesh(); }
		bool isLeaf() const { return mChildren[0] == nullptr; }
		void setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value);
		void update();
		void split();
		void merge();
		void rebuildEdge(int dir);
		void cleanup();
		void removeMarkers();

		irr::f32					 getHeight(int edge, int index) const;
		irr::u32					 getDepth()						const { return mDepth; }
		irr::scene::ISceneNode		*getSceneNode()					const { return mSceneNode; }
		std::array<TerrainNode*, 4>	 getGENeighbours()				const;
		TerrainNode					*getChild(int quad)				const { return mChildren[quad]; };
		TerrainNode					*getGENeighbourDir(int dir)		const;

	private:
		irr::u32 mDepth;

		irr::core::rectf mBounds;
		irr::core::vector3df mCentre;

		irr::scene::SMesh *mMesh;
		irr::scene::SMeshBuffer *mBuffer;
		irr::scene::ISceneNode *mSceneNode;

		std::array<TerrainNode*, 4> mChildren;
		std::array<std::vector<irr::u32>, 4> mEdges;

		std::vector<irr::core::vector3df> mHeights;
		std::vector<irr::scene::ISceneNode*> mMarkers;

		irr::u32 mNumVertices, mNumIndices;

		Terrain *mTerrain;
		TerrainNode *mParent;

		void cleanData();
		void createMesh();
		void createPlane(irr::scene::SMeshBuffer *buf);
		void calculateNormals(irr::scene::SMeshBuffer *buf, bool smooth = true);
};
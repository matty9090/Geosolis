#include <irr/irrlicht.h>
#include <array>
#include <vector>

class Terrain;

class TerrainNode {
	public:
		TerrainNode(TerrainNode *parent, Terrain *terrain, irr::core::rectf bounds, irr::scene::ISceneNode *node = nullptr);
		~TerrainNode();

		enum EDir  { North, East, South, West };
		enum EQuad { NE, NW, SE, SW };

		void init();
		bool isVisible() { return mVisible; }
		bool isLeaf() const { return mChildren[0] == nullptr; }
		void update();
		void split();
		void merge();
		void rebuildEdge(int dir);
		void cleanup();
		void notifyNeighbours();

		void setSphereBounds(irr::core::rectf sphereBounds);
		void setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value);
		void setRotation(irr::core::vector3df rotation) { mRotation = rotation; }
		void setFaceNeighbours(TerrainNode *north, TerrainNode *east, TerrainNode *south, TerrainNode *west);

		irr::f32					 getHeight(int edge, int index) const;
		irr::f32					 getScale()						const { return 1.0f / (1 << mDepth); }
		irr::u32					 getDepth()						const { return mDepth; }
		irr::core::vector3df		 getRotation()					const { return mParent ? mParent->getRotation() : mRotation; }
		irr::scene::ISceneNode		*getSceneNode()					const { return mSceneNode; }
		TerrainNode					*getChild(int quad)				const { return mChildren[quad]; };

		std::array<TerrainNode*, 4>					 getGENeighbours()								 	 const;
		std::array<std::vector<TerrainNode*>, 4>	 getSENeighbours()									 const;
		TerrainNode									*getGENeighbourDir(int dir)							 const;
		std::vector<TerrainNode*>					 getSENeighboursDir(TerrainNode *neighbour, int dir) const;

		static bool Wireframe;
		static bool Normals;
		static irr::u64 Triangles;

	private:
		bool mVisible;

		irr::u32 mDepth;
		irr::f32 mDiameter;

		irr::core::rectf mBounds, mSphereBounds;
		irr::core::vector3df mCentre;

		irr::scene::SMesh *mMesh;
		irr::scene::SMeshBuffer *mBuffer;
		irr::scene::ISceneNode *mSceneNode;

		std::array<TerrainNode*, 4> mChildren;
		std::array<TerrainNode*, 4> mTopNeighbours;
		std::array<std::vector<irr::u32>, 4> mEdges;

		std::vector<irr::core::vector3df> mHeights;
		std::vector<irr::scene::ISceneNode*> mMarkers;

		irr::core::vector3df mRotation;
		irr::u32 mNumVertices, mNumIndices;

		Terrain *mTerrain;
		TerrainNode *mParent;
		irr::scene::ISceneNode *mParentSceneNode;

		irr::video::ITexture *mHeightTex;

		void cleanData();
		void createMesh();
		void createHeightMap();
		void createPlane(irr::scene::SMeshBuffer *buf);

		inline irr::core::vector3df calculateNormal(irr::f32 x, irr::f32 y, irr::f32 stepX, irr::f32 stepY);
		inline void fixDetailV(irr::video::S3DVertex &v, int x, float yy, std::array<irr::s32, 4U> &details, float stepX, float stepY, float &height, irr::core::vector3df &normal, int dir, irr::core::matrix4 &rot);
		inline void fixDetailH(irr::video::S3DVertex &v, int y, float xx, std::array<irr::s32, 4U> &details, float stepX, float stepY, float &height, irr::core::vector3df &normal, int dir, irr::core::matrix4 &rot);
};
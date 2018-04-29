#include <irr/irrlicht.h>
#include <array>

class TerrainNode {
	public:
		TerrainNode(TerrainNode *parent, irr::core::rectf bounds);
		~TerrainNode();

		bool isLeaf() { return mChildren[0] == nullptr; }
		void cleanup();

	private:
		TerrainNode *mParent;
		std::array<TerrainNode*, 4> mChildren;
};
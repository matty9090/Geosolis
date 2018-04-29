#include "TerrainNode.hpp"

TerrainNode::TerrainNode(TerrainNode *parent, irr::core::rectf bounds) : mParent(parent) {
	
}

void TerrainNode::cleanup() {
	if (!isLeaf()) {
		for (auto child : mChildren) {
			child->cleanup();
			delete child;
		}
	}
}

TerrainNode::~TerrainNode() {

}
#include "TerrainNode.hpp"
#include "Terrain.hpp"

#include <iostream>
#include <queue>

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

TerrainNode::TerrainNode(TerrainNode *parent, Terrain *terrain, irr::core::rectf bounds)
  : mParent(parent),
	mTerrain(terrain),
	mBounds(bounds),
	mDepth(parent ? parent->getDepth() + 1 : 1)
{
	
}

void TerrainNode::setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value) {
	mSceneNode->setMaterialFlag(flag, value);

	if (!isLeaf()) {
		for (auto &child : mChildren)
			child->setMaterialFlag(flag, value);
	}
}

void TerrainNode::update() {
	float distance	= mCentre.getDistanceFrom(mTerrain->getCamera()->getPosition());

	bool divide	= (11.0f - 4.6f * log10(distance + 2.0f) > (f32)mDepth);

	if (!divide)
		merge();

	if (isLeaf() && divide && mDepth < 8) {
		split();
	} else if (!isLeaf()) {
		for (auto child : mChildren)
			child->update();
	}
}

void TerrainNode::split() {
	if (isLeaf()) {
		cleanData();

		rectf b = mBounds;

		float w = b.getWidth()  / 2.0f;
		float h = b.getHeight() / 2.0f;

		float x1 = b.UpperLeftCorner.X , y1 = b.UpperLeftCorner.Y;
		float x2 = b.LowerRightCorner.X, y2 = b.LowerRightCorner.Y;

		mChildren[NW] = new TerrainNode(this, mTerrain, rectf(x1    , y1    , x2 - w, y2 - h));
		mChildren[NE] = new TerrainNode(this, mTerrain, rectf(x1 + w, y1    , x2    , y2 - h));
		mChildren[SE] = new TerrainNode(this, mTerrain, rectf(x1 + w, y1 + h, x2    , y2    ));
		mChildren[SW] = new TerrainNode(this, mTerrain, rectf(x1    , y1 + h, x2 - w, y2    ));
		
		for (auto &child : mChildren)
			child->init();

		notifyNeighbours();
	} else {
		for (auto &child : mChildren)
			child->split();
	}
}

void TerrainNode::merge() {
	if (isLeaf())
		return;

	if (mChildren[0]->isLeaf()) {
		for (auto &child : mChildren) {
			child->cleanup();
			delete child;
			child = nullptr;
		}

		removeMarkers();
		notifyNeighbours();

		createMesh();
	} else {
		for (auto &child : mChildren)
			child->merge();
	}
}

void TerrainNode::rebuildEdge(int dir) {
	init();
}

void TerrainNode::cleanup() {
	cleanData();

	if (!isLeaf()) {
		for (auto &child : mChildren) {
			child->cleanup();
			delete child;
			child = nullptr;
		}
	}
}

void TerrainNode::notifyNeighbours() {
	auto neighbours = getSENeighbours();

	for (auto &node : neighbours[North]) if (node) node->rebuildEdge(South);
	for (auto &node : neighbours[East ]) if (node) node->rebuildEdge(West );
	for (auto &node : neighbours[South]) if (node) node->rebuildEdge(North);
	for (auto &node : neighbours[West ]) if (node) node->rebuildEdge(East );
}

void TerrainNode::removeMarkers() {
	for (auto &marker : mMarkers)
		mSceneNode->removeChild(marker);

	mMarkers.clear();
}

irr::f32 TerrainNode::getHeight(int edge, int index) const {
	return mHeights[mEdges[edge][index]].Y;
}

std::array<TerrainNode*, 4> TerrainNode::getGENeighbours() const {
	std::array<TerrainNode*, 4> neighbours;

	neighbours[North] = getGENeighbourDir(North);
	neighbours[East]  = getGENeighbourDir(East);
	neighbours[South] = getGENeighbourDir(South);
	neighbours[West]  = getGENeighbourDir(West);

	return neighbours;
}

std::array<std::vector<TerrainNode*>, 4> TerrainNode::getSENeighbours() const {
	std::array<TerrainNode*, 4> neighbours;
	std::array<std::vector<TerrainNode*>, 4> neighboursSE;

	neighbours[North] = getGENeighbourDir(North);
	neighbours[East]  = getGENeighbourDir(East);
	neighbours[South] = getGENeighbourDir(South);
	neighbours[West]  = getGENeighbourDir(West);

	neighboursSE[North] = getSENeighboursDir(neighbours[North], North);
	neighboursSE[East ] = getSENeighboursDir(neighbours[East ], East);
	neighboursSE[South] = getSENeighboursDir(neighbours[South], South);
	neighboursSE[West ] = getSENeighboursDir(neighbours[West ], West);

	return neighboursSE;
}

TerrainNode	*TerrainNode::getGENeighbourDir(int dir) const {
	switch(dir) {
		case North: {
			if (!mParent) return nullptr;
			if (mParent->getChild(SW) == this) return mParent->getChild(NW);			
			if (mParent->getChild(SE) == this) return mParent->getChild(NE);

			TerrainNode *node = mParent->getGENeighbourDir(dir);

			if (!node || node->isLeaf())
				return node;

			return (mParent->getChild(NW) == this) ? node->getChild(SW) : node->getChild(SE);

			break;
		}

		case South: {
			if (!mParent) return nullptr;
			if (mParent->getChild(NW) == this) return mParent->getChild(SW);
			if (mParent->getChild(NE) == this) return mParent->getChild(SE);

			TerrainNode *node = mParent->getGENeighbourDir(dir);

			if (!node || node->isLeaf())
				return node;

			return (mParent->getChild(SW) == this) ? node->getChild(NW) : node->getChild(NE);

			break;
		}

		case East: {
			if (!mParent) return nullptr;
			if (mParent->getChild(NW) == this) return mParent->getChild(NE);
			if (mParent->getChild(SW) == this) return mParent->getChild(SE);

			TerrainNode *node = mParent->getGENeighbourDir(dir);

			if (!node || node->isLeaf())
				return node;

			return (mParent->getChild(NE) == this) ? node->getChild(NW) : node->getChild(SW);

			break;
		}

		case West: {
			if (!mParent) return nullptr;
			if (mParent->getChild(NE) == this) return mParent->getChild(NW);
			if (mParent->getChild(SE) == this) return mParent->getChild(SW);

			TerrainNode *node = mParent->getGENeighbourDir(dir);

			if (!node || node->isLeaf())
				return node;

			return (mParent->getChild(NW) == this) ? node->getChild(NE) : node->getChild(SE);

			break;
		}
	}

	return nullptr;
}

std::vector<TerrainNode*> TerrainNode::getSENeighboursDir(TerrainNode *neighbour, int dir) const {
	std::vector<TerrainNode*> neighbours;
	std::queue<TerrainNode*> nodes;

	if (neighbour)
		nodes.push(neighbour);

	switch (dir) {
		case North: {
			while (nodes.size() > 0) {
				if (nodes.front()->isLeaf())
					neighbours.push_back(nodes.front());
				else {
					nodes.push(nodes.front()->getChild(SW));
					nodes.push(nodes.front()->getChild(SE));
				}

				nodes.pop();
			}

			break;
		}

		case South: {
			while (nodes.size() > 0) {
				if (nodes.front()->isLeaf())
					neighbours.push_back(nodes.front());
				else {
					nodes.push(nodes.front()->getChild(NW));
					nodes.push(nodes.front()->getChild(NE));
				}

				nodes.pop();
			}

			break;
		}

		case East: {
			while (nodes.size() > 0) {
				if (nodes.front()->isLeaf())
					neighbours.push_back(nodes.front());
				else {
					nodes.push(nodes.front()->getChild(NW)); 
					nodes.push(nodes.front()->getChild(SW));
				}

				nodes.pop();
			}

			break;
		}

		case West: {
			while (nodes.size() > 0) {
				if (nodes.front()->isLeaf())
					neighbours.push_back(nodes.front());
				else {
					nodes.push(nodes.front()->getChild(NE));
					nodes.push(nodes.front()->getChild(SE));
				}

				nodes.pop();
			}

			break;
		}
	}

	return neighbours;
}

void TerrainNode::cleanData() {
	removeMarkers();

	if (mSceneNode && mMesh) {
		mMesh->clear();
		mMesh->drop();
		mMesh = nullptr;
	}
}

void TerrainNode::createMesh() {
	mMesh = new irr::scene::SMesh();

	mBuffer = new SMeshBuffer();
	mMesh->addMeshBuffer(mBuffer);
	mBuffer->drop();

	ISceneNode *parent = (mParent) ? mParent->getSceneNode() : nullptr;
	mSceneNode = mTerrain->getSceneManager()->addMeshSceneNode(mMesh, parent);
	mSceneNode->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
	mSceneNode->setAutomaticCulling(EAC_OFF);
	mSceneNode->setMaterialTexture(0, mTerrain->getVideoDriver()->getTexture("tex/grass.png"));
	mSceneNode->setMaterialFlag(EMF_WIREFRAME, false);

	mNumVertices = GRID_SIZE * GRID_SIZE;
	mNumIndices	 = 6 * (GRID_SIZE - 1) * (GRID_SIZE - 1);

	mBuffer->Vertices.set_used(mNumVertices);
	mBuffer->Indices.set_used(mNumIndices);

	createPlane(mBuffer);

	mCentre = mBuffer->Vertices[mNumVertices / 2].Pos;

	mBuffer->setHardwareMappingHint(EHM_STATIC);
}

void TerrainNode::createPlane(irr::scene::SMeshBuffer * buf) {
	SimplexNoise noise(1.2f, 1.0f, 2.2f, 0.4f);

	u32 i = 0;

	float stepX = mBounds.getWidth() / (GRID_SIZE - 1);
	float stepY = mBounds.getHeight() / (GRID_SIZE - 1);

	auto neighbours = getGENeighbours();
	
	std::array<s32, 4> details;
	details[North] = neighbours[North] ? ((int)pow(2, mDepth - neighbours[North]->getDepth())) : 0;
	details[South] = neighbours[South] ? ((int)pow(2, mDepth - neighbours[South]->getDepth())) : 0;
	details[East ] = neighbours[East ] ? ((int)pow(2, mDepth - neighbours[East ]->getDepth())) : 0;
	details[West ] = neighbours[West ] ? ((int)pow(2, mDepth - neighbours[West ]->getDepth())) : 0;

	/*mEdges[North].resize(GRID_SIZE);
	mEdges[West ].resize(GRID_SIZE);
	mEdges[South].resize(GRID_SIZE);
	mEdges[East ].resize(GRID_SIZE);*/

	//mHeights.resize(mNumVertices);

	for (int y = 0; y < GRID_SIZE; ++y) {
		for (int x = 0; x < GRID_SIZE; ++x) {
			float xx = mBounds.UpperLeftCorner.X + x * stepX;
			float yy = mBounds.UpperLeftCorner.Y + y * stepY;			
			float height;
			vector3df normal;

			if (details[North] > 1 && y == 0 && x > 0 && x < GRID_SIZE - 1 && (x % details[North] != 0))
				fixDetailV(noise, x, yy, details, stepX, stepY, height, normal, North);
			else if(details[South] > 1 && y == GRID_SIZE - 1 && x > 0 && x < GRID_SIZE - 1 && (x % details[South] != 0))
				fixDetailV(noise, x, yy, details, stepX, stepY, height, normal, South);
			else if (details[West] > 1 && x == 0 && y > 0 && y < GRID_SIZE - 1 && (y % details[West] != 0))
				fixDetailH(noise, y, xx, details, stepX, stepY, height, normal, West);
			else if (details[East] > 1 && x == GRID_SIZE - 1 && y > 0 && y < GRID_SIZE - 1 && (y % details[East] != 0))
				fixDetailH(noise, y, xx, details, stepX, stepY, height, normal, East);
			else {
				normal = calculateNormal(noise, xx, yy, stepX, stepY);
				height = noise.fractal(8, (f32)xx / 50.0f, (f32)yy / 50.0f) * 5.0f;
			}

			S3DVertex &v = buf->Vertices[i++];
			v.Pos.set(xx, height, yy);
			v.Color.set(0x00aa00);
			v.Normal = normal;
			v.TCoords.set(x * stepX, y * stepY);

			//mHeights[i - 1] = v.Pos;

			/*if (y == 0) mEdges[North][x] = i - 1;
			if (x == 0) mEdges[West ][y] = i - 1;
			if (y == GRID_SIZE - 1) mEdges[South][x] = i - 1;
			if (x == GRID_SIZE - 1) mEdges[East ][y] = i - 1;*/
		}
	}

	i = 0;

	for (int y = 0; y < GRID_SIZE - 1; ++y) {
		for (int x = 0; x < GRID_SIZE - 1; ++x) {
			buf->Indices[i++] = (y)* GRID_SIZE + x;
			buf->Indices[i++] = (y)* GRID_SIZE + x + 1;
			buf->Indices[i++] = (y + 1) * GRID_SIZE + x;
			buf->Indices[i++] = (y + 1) * GRID_SIZE + x;
			buf->Indices[i++] = (y)* GRID_SIZE + x + 1;
			buf->Indices[i++] = (y + 1) * GRID_SIZE + x + 1;
		}
	}
}

void TerrainNode::fixDetailV(SimplexNoise &noise, int x, float yy, std::array<irr::s32, 4U> &details, float stepX, float stepY, float &height, irr::core::vector3df &normal, int dir) {
	f32 r = (f32)(x % details[dir]);
	f32 p = r / (f32)details[dir];
	f32 p0 = -r, p1 = (f32)details[dir] - r;

	float xx  = mBounds.UpperLeftCorner.X + x * stepX;
	float xx0 = mBounds.UpperLeftCorner.X + (x + p0) * stepX;
	float xx1 = mBounds.UpperLeftCorner.X + (x + p1) * stepX;

	float h0 = noise.fractal(8, (f32)xx0 / 50.0f, (f32)yy / 50.0f) * 5.0f;
	float h1 = noise.fractal(8, (f32)xx1 / 50.0f, (f32)yy / 50.0f) * 5.0f;

	height = h0 + p * (h1 - h0);

	//vector3df n0 = calculateNormal(noise, xx0, yy, stepX, stepY);
	//vector3df n1 = calculateNormal(noise, xx1, yy, stepX, stepY);

	//normal = n0 + p * (n1 - n0);
}

void TerrainNode::fixDetailH(SimplexNoise &noise, int y, float xx, std::array<irr::s32, 4U> &details, float stepX, float stepY, float &height, irr::core::vector3df &normal, int dir) {
	f32 r = (f32)(y % details[dir]);
	f32 p = r / (f32)details[dir];
	f32 p0 = -r, p1 = (f32)details[dir] - r;

	float yy  = mBounds.UpperLeftCorner.Y + y * stepY;
	float yy0 = mBounds.UpperLeftCorner.Y + (y + p0) * stepY;
	float yy1 = mBounds.UpperLeftCorner.Y + (y + p1) * stepY;

	float h0 = noise.fractal(8, (f32)xx / 50.0f, (f32)yy0 / 50.0f) * 5.0f;
	float h1 = noise.fractal(8, (f32)xx / 50.0f, (f32)yy1 / 50.0f) * 5.0f;

	height = h0 + p * (h1 - h0);

	vector3df n0 = calculateNormal(noise, xx, yy0, stepX, stepY);
	vector3df n1 = calculateNormal(noise, xx, yy1, stepX, stepY);

	normal = n0 + p * (n1 - n0);
}

vector3df TerrainNode::calculateNormal(SimplexNoise &noise, irr::f32 x, irr::f32 y, irr::f32 stepX, irr::f32 stepY) {
	std::array<f32, 9> s;
	vector3df n;
	u32 i = 0;

	for (int yy = -1; yy <= 1; yy++)
		for (int xx = -1; xx <= 1; xx++)
			s[i++] = noise.fractal(8, (x - (f32)xx * stepX) / 50.0f, (y - (f32)yy * stepY) / 50.0f);

	f32 scale = 0.02f;

	n.X = scale * -(s[2] - s[0] + 2 * (s[5] - s[3]) + s[8] - s[6]);
	n.Z = scale * -(s[6] - s[0] + 2 * (s[7] - s[1]) + s[8] - s[2]);
	n.Y = 1.0;

	return n.normalize();
}

TerrainNode::~TerrainNode() {

}
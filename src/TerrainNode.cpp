#include "TerrainNode.hpp"
#include "SimplexNoise.hpp"
#include "Terrain.hpp"

#include <iostream>

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

	//std::cout << distance << ": " << 7.0f - 0.07f * distance << "\n";

	bool divide	= (3.0f - 0.03f * distance > (f32)mDepth);

	if (!divide)
		merge();

	if (isLeaf() && divide && mDepth < 4) {
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

		auto neighbours = getGENeighbours();

		if (neighbours[North])
			neighbours[North]->rebuildEdge(South);

		mChildren[NE] = new TerrainNode(this, mTerrain, rectf(x1    , y1    , x2 - w, y2 - h));
		mChildren[NW] = new TerrainNode(this, mTerrain, rectf(x1 + w, y1    , x2    , y2 - h));
		mChildren[SE] = new TerrainNode(this, mTerrain, rectf(x1 + w, y1 + h, x2    , y2    ));
		mChildren[SW] = new TerrainNode(this, mTerrain, rectf(x1    , y1 + h, x2 - w, y2    ));

		for (auto &child : mChildren)
			child->init();
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

		//auto neighbours = getGENeighbours();

		//if (neighbours[North])
		//	neighbours[North]->rebuildEdge(South);

		createMesh();
	} else {
		for (auto &child : mChildren)
			child->merge();
	}
}

void TerrainNode::rebuildEdge(int dir) {
	removeMarkers();

	//cleanData();
	//init();
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
	}

	return nullptr;
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
	calculateNormals(mBuffer);

	mCentre = mBuffer->Vertices[mNumVertices / 2].Pos;

	mBuffer->setHardwareMappingHint(EHM_STATIC);
}

void TerrainNode::createPlane(irr::scene::SMeshBuffer * buf) {
	SimplexNoise noise(1.2f, 2.0f, 2.2f, 0.4f);

	u32 i = 0;

	float stepX = mBounds.getWidth() / (GRID_SIZE - 1);
	float stepY = mBounds.getHeight() / (GRID_SIZE - 1);

	auto neighbours = getGENeighbours();
	
	std::array<s32, 4> details;
	details[North] = neighbours[North] ? ((int)pow(2, mDepth - neighbours[North]->getDepth())) : 0;

	mEdges[North].resize(GRID_SIZE);
	mEdges[West ].resize(GRID_SIZE);
	mEdges[South].resize(GRID_SIZE);
	mEdges[East ].resize(GRID_SIZE);

	mHeights.resize(mNumVertices);

	for (int y = 0; y < GRID_SIZE; ++y) {
		for (int x = 0; x < GRID_SIZE; ++x) {
			float xx = mBounds.UpperLeftCorner.X + x * stepX;
			float yy = mBounds.UpperLeftCorner.Y + y * stepY;			
			float height;

			//mTerrain->getSceneManager()->addSphereSceneNode(0.14f, 16, getSceneNode(), -1, vector3df(xx, height, yy))->setMaterialTexture(0, mTerrain->getVideoDriver()->getTexture("tex/blue.png"));

			if (details[North] > 1 && y == 0 && x > 0 && x < GRID_SIZE - 1 && ((x - 1) % details[North] == 0)) {
				/*height  = mParent->getHeight(North, (x - 1) / details[North]);
				height += mParent->getHeight(North, (x + 1) / details[North]);
				height /= 2;*/

				float xx0 = mBounds.UpperLeftCorner.X + (x - 1) * stepX;
				float xx1 = mBounds.UpperLeftCorner.X + (x + 1) * stepX;

				height = noise.fractal(8, (f32)xx0 / 50.0f, (f32)yy / 50.0f) * 5.0f;
				height += noise.fractal(8, (f32)xx1 / 50.0f, (f32)yy / 50.0f) * 5.0f;
				height /= 2;

				/*ISceneNode *node = mTerrain->getSceneManager()->addSphereSceneNode(0.28f / mDepth, 8, mSceneNode, -1, vector3df(xx, height, yy));
				node->setMaterialTexture(0, mTerrain->getVideoDriver()->getTexture("tex/blue.png"));
				mMarkers.push_back(node);*/
			} else
				height = noise.fractal(8, (f32)xx / 50.0f, (f32)yy / 50.0f) * 5.0f;

			S3DVertex &v = buf->Vertices[i++];
			v.Pos.set(xx, height, yy);
			v.Color.set(0x00aa00);
			v.Normal.set(0.0f, 0.0f, 0.0f);
			v.TCoords.set(x * stepX, y * stepY);

			mHeights[i - 1] = v.Pos;

			if (y == 0) mEdges[North][x] = i - 1;
			if (x == 0) mEdges[West ][y] = i - 1;
			if (y == GRID_SIZE - 1) mEdges[South][x] = i - 1;
			if (x == GRID_SIZE - 1) mEdges[East ][y] = i - 1;
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

void TerrainNode::calculateNormals(irr::scene::SMeshBuffer *buf, bool smooth) {
	if (smooth) {
		auto neighbours = getGENeighbours();

		for (u32 i = 0; i < mNumIndices; i += 3) {
			vector3df p0 = buf->Vertices[buf->Indices[i + 0]].Pos;
			vector3df p1 = buf->Vertices[buf->Indices[i + 1]].Pos;
			vector3df p2 = buf->Vertices[buf->Indices[i + 2]].Pos;

			vector3df e1 = p1 - p0, e2 = p2 - p0;
			vector3df n = e2.crossProduct(e1).normalize();

			buf->Vertices[buf->Indices[i + 0]].Normal += n;
			buf->Vertices[buf->Indices[i + 1]].Normal += n;
			buf->Vertices[buf->Indices[i + 2]].Normal += n;
		}

		for (u32 i = 0; i < mNumVertices; ++i) {
			vector3df &normal = buf->Vertices[i].Normal;
			normal = normal.normalize();
		}
	} else {
		for (u32 i = 0; i < mNumIndices - 2; i++) {
			vector3df p0 = buf->Vertices[buf->Indices[i + 0]].Pos;
			vector3df p1 = buf->Vertices[buf->Indices[i + 1]].Pos;
			vector3df p2 = buf->Vertices[buf->Indices[i + 2]].Pos;

			vector3df e1 = p1 - p0, e2 = p2 - p0;
			vector3df n = e2.crossProduct(e1).normalize();

			buf->Vertices[buf->Indices[i]].Normal = n;
		}
	}
}

TerrainNode::~TerrainNode() {

}
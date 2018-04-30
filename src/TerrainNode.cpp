#include "TerrainNode.hpp"
#include "SimplexNoise.hpp"
#include "Terrain.hpp"

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

TerrainNode::TerrainNode(TerrainNode *parent, Terrain *terrain, irr::core::rectf bounds)
	: mParent(parent), mTerrain(terrain), mBounds(bounds)
{
	mDepth = (parent) ? parent->getDepth() + 1 : 1;

	createMesh();
}

void TerrainNode::setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value) {
	mSceneNode->setMaterialFlag(flag, value);

	if (!isLeaf()) {
		for (auto &child : mChildren)
			child->setMaterialFlag(flag, value);
	}
}

void TerrainNode::update() {
	/*float distance	= mCentre.getDistanceFrom(mTerrain->getCamera()->getPosition());
	bool divide		= (distance < (float)mDepth / 5.5f);

	if (!divide)
		merge();

	if (isLeaf() && divide && mDepth < 10) {
		split();
	} else if (!isLeaf()) {
		for (auto child : mChildren)
			child->update();
	}*/
}

void TerrainNode::split() {
	if (isLeaf()) {
		rectf b = mBounds;

		float w = b.getWidth()  / 2.0f;
		float h = b.getHeight() / 2.0f;

		float x1 = b.UpperLeftCorner.X , y1 = b.UpperLeftCorner.Y;
		float x2 = b.LowerRightCorner.X, y2 = b.LowerRightCorner.Y;

		mChildren[0] = new TerrainNode(this, mTerrain, rectf(x1    , y1    , x2 - w, y2 - h));
		mChildren[1] = new TerrainNode(this, mTerrain, rectf(x1 + w, y1    , x2    , y2 - h));
		mChildren[2] = new TerrainNode(this, mTerrain, rectf(x1 + w, y1 + h, x2    , y2    ));
		mChildren[3] = new TerrainNode(this, mTerrain, rectf(x1    , y1 + h, x2 - w, y2    ));

		cleanData();
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

		createMesh();
	} else {
		for (auto &child : mChildren)
			child->merge();
	}
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

void TerrainNode::cleanData() {
	if (mSceneNode && mMesh) {
		mMesh->clear();
		mMesh->drop();
		mMesh = nullptr;
	}
}

void TerrainNode::createMesh() {
	mMesh = new irr::scene::SMesh();

	SMeshBuffer *buf = new SMeshBuffer();
	mMesh->addMeshBuffer(buf);
	buf->drop();

	mNumVertices = GRID_SIZE * GRID_SIZE;
	mNumIndices	 = 6 * (GRID_SIZE - 1) * (GRID_SIZE - 1);

	buf->Vertices.set_used(mNumVertices);
	buf->Indices.set_used(mNumIndices);

	createPlane(buf);
	calculateNormals(buf);

	mCentre = buf->Vertices[((GRID_SIZE - 1) * (GRID_SIZE - 1)) / 2].Pos;

	buf->setHardwareMappingHint(EHM_STATIC);
	
	ISceneNode *parent = (mParent) ? mParent->getSceneNode() : nullptr;
	mSceneNode = mTerrain->getSceneManager()->addMeshSceneNode(mMesh, parent);
	mSceneNode->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
	mSceneNode->setAutomaticCulling(EAC_OFF);
	mSceneNode->setMaterialTexture(0, mTerrain->getVideoDriver()->getTexture("tex/grass.png"));
}

void TerrainNode::createPlane(irr::scene::SMeshBuffer * buf) {
	SimplexNoise noise(1.2f, 2.0f, 2.2f, 0.4f);

	u32 i = 0;

	float stepX = mBounds.getWidth() / (GRID_SIZE - 1);
	float stepY = mBounds.getHeight() / (GRID_SIZE - 1);

	for (int y = 0; y < GRID_SIZE; ++y) {
		for (int x = 0; x < GRID_SIZE; ++x) {
			float xx = mBounds.UpperLeftCorner.X + x * stepX;
			float yy = mBounds.UpperLeftCorner.Y + y * stepY;

			S3DVertex &v = buf->Vertices[i++];
			v.Pos.set(xx, noise.fractal(8, (f32)xx / 50.0f, (f32)yy / 50.0f) * 5.0f, yy);
			v.Color.set(0x00aa00);
			v.Normal.set(0.0f, 0.0f, 0.0f);
			v.TCoords.set(x * stepX, y * stepY);
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
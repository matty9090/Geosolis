#include "Terrain.hpp"
#include "TerrainNode.hpp"

#include <iostream>

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

Terrain::Terrain(irr::IrrlichtDevice *device, irr::scene::ISceneNode *node, irr::f32 radius)
	: mScene(device->getSceneManager()),
	mDriver(device->getVideoDriver()),
	mBounds(-0.5f, -0.5f, 0.5f, 0.5f),
	mRadius(radius)
{
	//generateHeightmap();
	
	mGPU = mDriver->getGPUProgrammingServices();
	mDriver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	TerrainShader *terrainShader = new TerrainShader(this);

	mMaterialType = mGPU->addHighLevelShaderMaterialFromFiles("shaders/VertexShader.fx", "main", EVST_VS_3_0, "shaders/PixelShader.fx", "main", EPST_PS_3_0, terrainShader, EMT_SOLID);
	terrainShader->drop();

	for (auto &face : mFaces)
		face = new TerrainNode(nullptr, this, mBounds, node);

	mFaces[Top   ]->setRotation(vector3df(   0.0f, 0.0f,   0.0f));
	mFaces[Bottom]->setRotation(vector3df( 180.0f, 0.0f,   0.0f));
	mFaces[Right ]->setRotation(vector3df(   0.0f, 0.0f,  90.0f));
	mFaces[Left  ]->setRotation(vector3df(   0.0f, 0.0f, -90.0f));
	mFaces[Front ]->setRotation(vector3df( -90.0f, 0.0f,   0.0f));
	mFaces[Back  ]->setRotation(vector3df(  90.0f, 0.0f,   0.0f));

	mFaces[Top   ]->setFaceNeighbours(mFaces[Back ], mFaces[Right], mFaces[Front ], mFaces[Left ]);
	mFaces[Bottom]->setFaceNeighbours(mFaces[Front], mFaces[Right], mFaces[Back  ], mFaces[Left ]);
	mFaces[Right ]->setFaceNeighbours(mFaces[Top  ], mFaces[Back ], mFaces[Bottom], mFaces[Front]);
	mFaces[Left  ]->setFaceNeighbours(mFaces[Top  ], mFaces[Front], mFaces[Bottom], mFaces[Back ]);
	mFaces[Front ]->setFaceNeighbours(mFaces[Top  ], mFaces[Right], mFaces[Bottom], mFaces[Left ]);
	mFaces[Back  ]->setFaceNeighbours(mFaces[Top  ], mFaces[Left ], mFaces[Bottom], mFaces[Right]);

	for (auto &face : mFaces)
		face->init();
}

Terrain::~Terrain() {
	for (auto &face : mFaces) {
		face->cleanup();
		delete face;
	}
}

void Terrain::setRotation(vector3df rot) {
	mRotation = rot;
}

void Terrain::setPosition(vector3df pos) {
	mPosition = pos;
}

void Terrain::update() {
	for (auto &face : mFaces)
		face->update();
}

float Terrain::getHeight(float x, float y) const {
	float h = (float)mNoise.GetValue((double)x * 10.0, (double)y * 10.0, 0.0) * 2.0f;
	return h;
}

irr::scene::ISceneNode *Terrain::getSceneNode() const {
	return mFaces[0]->getSceneNode();
}

vector3df Terrain::project(vector3df p) const {
	float x2 = p.X * p.X, y2 = p.Y * p.Y, z2 = p.Z * p.Z;

	return vector3df(p.X * sqrtf(1.0f - y2 * 0.5f - z2 * 0.5f + (y2 * z2) * 0.33333f),
					 p.Y * sqrtf(1.0f - z2 * 0.5f - x2 * 0.5f + (z2 * x2) * 0.33333f),
					 p.Z * sqrtf(1.0f - x2 * 0.5f - y2 * 0.5f + (x2 * y2) * 0.33333f));
}

void Terrain::generateHeightmap() {
	mNoise.SetOctaveCount(14);
	mNoise.SetFrequency(1.0f);
	mNoise.SetPersistence(0.5f);
	mNoise.SetLacunarity(2.0f);

	utils::NoiseMapBuilderSphere builder;
	builder.SetSourceModule(mNoise);
	builder.SetDestNoiseMap(mHeightmap);
	builder.SetDestSize(512, 256);
	builder.SetBounds(-90.0, 90.0, -180.0, 180.0);
	builder.Build();

	utils::RendererImage renderer;
	utils::Image image;
	utils::NoiseMapBuilderSphere heightMapBuilder;

	heightMapBuilder.SetSourceModule(mNoise);
	heightMapBuilder.SetDestNoiseMap(mHeightmap);
	heightMapBuilder.SetDestSize(4096, 2048);
	heightMapBuilder.SetBounds(-90.0, 90.0, -180.0, 180.0);
	heightMapBuilder.Build();

	renderer.SetSourceNoiseMap(mHeightmap);
	renderer.SetDestImage(image);
	renderer.ClearGradient();
	renderer.AddGradientPoint(-1.0000, utils::Color(0, 0, 128, 255)); // deeps
	renderer.AddGradientPoint(-0.2500, utils::Color(0, 0, 255, 255)); // shallow
	renderer.AddGradientPoint(0.0000, utils::Color(0, 128, 255, 255)); // shore
	renderer.AddGradientPoint(0.0625, utils::Color(240, 240, 64, 255)); // sand
	renderer.AddGradientPoint(0.1250, utils::Color(32, 160, 0, 255)); // grass
	renderer.AddGradientPoint(0.3750, utils::Color(32, 120, 0, 255)); // dirt
	renderer.AddGradientPoint(0.7500, utils::Color(32, 100, 0, 255)); // rock
	renderer.AddGradientPoint(1.0000, utils::Color(32, 70, 0, 255)); // snow
	renderer.EnableLight();
	renderer.SetLightContrast(3.0);
	renderer.SetLightBrightness(2.0);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename("tex/heightmap.bmp");
	writer.WriteDestFile();
}

void Terrain::setMaterialFlag(irr::video::E_MATERIAL_FLAG flag, bool value) const {
	for (auto &face : mFaces)
		face->getSceneNode()->setMaterialFlag(flag, value);
}

void TerrainShader::OnSetConstants(IMaterialRendererServices *services, s32 userData) {
	IVideoDriver *driver = services->getVideoDriver();

	matrix4 worldViewProj;
	worldViewProj = driver->getTransform(video::ETS_PROJECTION);
	worldViewProj *= driver->getTransform(video::ETS_VIEW);
	worldViewProj *= driver->getTransform(video::ETS_WORLD);

	vector3df lightPos = vector3df(-10000.0f, 0.0f, -1000.0f);
	SColorf col(1.0f, 1.0f, 1.0f, 0.0f);
	SColorf ambient(0.008f, 0.008f, 0.008f, 0.0f);

	matrix4 world = driver->getTransform(video::ETS_WORLD);
	
	services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
	services->setVertexShaderConstant("mWorld", world.pointer(), 16);
	services->setPixelShaderConstant("mWorld", world.pointer(), 16);
	services->setPixelShaderConstant("mAmbient", reinterpret_cast<f32*>(&ambient), 4);
	services->setPixelShaderConstant("mLightPos", reinterpret_cast<f32*>(&lightPos), 3);
	services->setPixelShaderConstant("mLightColour", reinterpret_cast<f32*>(&col), 4);
}

#include "App.hpp"

#include <iostream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

App::App(std::wstring title) : dt(0), mTitle(title.c_str()), mWireframe(true) {
	mEventReciever = new EventReciever();
	mDevice = createDevice(EDT_DIRECT3D9, dimension2d<u32>(1280, 960), 32, false, false, false, mEventReciever);

	if (!mDevice)
		exit(1);

	mDevice->setWindowCaption(mTitle.c_str());

	mDriver = mDevice->getVideoDriver();
	mScene  = mDevice->getSceneManager();
	mGui    = mDevice->getGUIEnvironment();

	ICameraSceneNode *cam = mScene->addCameraSceneNodeFPS(nullptr, 80.0f, 0.02f);
	cam->setPosition(vector3df(0.0f, 20.0f, 0.0f));
	cam->setRotation(vector3df(3.141f, 0.0f, 0.0f));

	mScene->addLightSceneNode(nullptr, vector3df(20.0f, 20.0f, 0.0f));

	mTerrain = new Terrain();

	mTerrainNode = mScene->addMeshSceneNode(mTerrain->getMesh());
	mTerrainNode->setMaterialFlag(EMF_WIREFRAME, mWireframe);
	mTerrainNode->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
	mTerrainNode->setAutomaticCulling(EAC_OFF);
	mTerrainNode->setMaterialTexture(0, mDriver->getTexture("tex/grass.png"));
}

App::~App() {
	mDevice->drop();
	delete mTerrain;
}

int App::run() {
	int fps = 0;
	int lfps = 0;

	while (mDevice->run()) {
		handleEvents();
		
		mDriver->beginScene(true, true, SColor(0x000000));
		
		mScene->drawAll();
		mGui->drawAll();
		mDriver->endScene();

		dt = mDevice->getTimer()->getTime();
		fps = mDriver->getFPS();

		if (fps != lfps) {
			stringw tmp = mTitle + " [";
			tmp += fps;
			tmp += " FPS]";

			mDevice->setWindowCaption(tmp.c_str());
			lfps = fps;
		}
	}

	return 0;
}

void App::handleEvents() {
	if (mEventReciever->KeyHit(KEY_ESCAPE))
		mDevice->closeDevice();

	if (mEventReciever->KeyHit(KEY_F3)) {
		mWireframe = !mWireframe;
		mTerrainNode->setMaterialFlag(EMF_WIREFRAME, mWireframe);
	}
}
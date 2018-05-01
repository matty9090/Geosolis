#include "App.hpp"
#include "TerrainNode.hpp"

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

	mCamera = mScene->addCameraSceneNodeFPS(nullptr, 80.0f, 0.04f);
	//mCamera = mScene->addCameraSceneNode();
	mCamera->setPosition(vector3df(0.0f, 100.0f, 0.0f));
	mCamera->setRotation(vector3df(3.141f, 0.0f, 0.0f));

	mScene->addLightSceneNode(nullptr, vector3df(0.0f, 20.0f, 0.0f));

	mTerrain = new Terrain(mScene, mDriver, mCamera);
	mTerrain->getTerrainNode()->setMaterialFlag(EMF_WIREFRAME, mWireframe);
}

App::~App() {
	mDevice->drop();

	delete mTerrain;
	delete mEventReciever;
}

int App::run() {
	int fps = 0;
	int lfps = 0;

	while (mDevice->run()) {
		handleEvents();

		mTerrain->update();
		
		mDriver->beginScene(true, true, SColor(0x000000));
		
		mScene->drawAll();
		mGui->drawAll();
		mDriver->endScene();

		dt  = mDevice->getTimer()->getTime();
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
		mTerrain->getTerrainNode()->setMaterialFlag(EMF_WIREFRAME, mWireframe);
	}

	if (mEventReciever->KeyHit(KEY_F1)) mTerrain->getTerrainNode()->split();
	if (mEventReciever->KeyHit(KEY_F2)) mTerrain->getTerrainNode()->merge();

	if (mEventReciever->KeyHit(KEY_KEY_Q)) {
		bool enabled = mCamera->isInputReceiverEnabled();
		mCamera->setInputReceiverEnabled(!enabled);
		mDevice->getCursorControl()->setVisible(enabled);
	}
}
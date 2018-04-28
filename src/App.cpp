#include "App.hpp"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

App::App(std::wstring title) : dt(0), mTitle(title.c_str()) {
	mEventReciever = new EventReciever();
	mDevice = createDevice(EDT_DIRECT3D9, dimension2d<u32>(1280, 960), 32, false, false, false, mEventReciever);

	if (!mDevice)
		exit(1);

	mDevice->setWindowCaption(mTitle.c_str());

	mDriver = mDevice->getVideoDriver();
	mScene  = mDevice->getSceneManager();
	mGui    = mDevice->getGUIEnvironment();

	//mScene->addCameraSceneNode(nullptr, vector3df(0.0f, 20.0f, 0.0f), vector3df(0.0f, 0.0f, 0.0f));
	mScene->addCameraSceneNodeFPS(nullptr, 80.0f, 0.02f);

	mTerrain = new Terrain();

	ISceneNode *node = mScene->addMeshSceneNode(mTerrain->getMesh());
	node->setMaterialFlag(EMF_LIGHTING, false);
	node->setMaterialFlag(EMF_WIREFRAME, true);
	node->setMaterialFlag(EMF_BACK_FACE_CULLING, false);
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
	if (mEventReciever->IsKeyDown(KEY_ESCAPE))
		mDevice->closeDevice();
}
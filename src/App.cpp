#include "App.hpp"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

App::App(std::wstring title) {
	mDevice = createDevice(EDT_DIRECT3D9, dimension2d<u32>(1280, 960));

	if (!mDevice)
		exit(1);

	mDevice->setWindowCaption(title.c_str());

	mDriver = mDevice->getVideoDriver();
	mScene  = mDevice->getSceneManager();
	mGui    = mDevice->getGUIEnvironment();
}

int App::run() {
	while (mDevice->run()) {
		mDriver->beginScene(true, true, SColor(255, 0, 0, 0));
		mScene->drawAll();
		mGui->drawAll();
		mDriver->endScene();
	}

	return 0;
}

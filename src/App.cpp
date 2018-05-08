#include "App.hpp"
#include "TerrainNode.hpp"

#include <iostream>
#include <sstream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

App::App(std::wstring title) : dt(0), mTitle(title.c_str()), mWireframe(false) {
	mEventReciever = new EventReciever();
	mDevice = createDevice(EDT_DIRECT3D9, dimension2d<u32>(1280, 960), 32, false, false, false, mEventReciever);

	if (!mDevice)
		exit(1);

	mDevice->setWindowCaption(mTitle.c_str());

	mDriver = mDevice->getVideoDriver();
	mScene  = mDevice->getSceneManager();
	mGui    = mDevice->getGUIEnvironment();

	mCamera = mScene->addCameraSceneNodeFPS(nullptr, 80.0f, 0.04f);
	mCamera->setNearValue(0.01f);
	mCamera->setPosition(vector3df(0.0f, 0.0f, -300.0f));
	mCamera->setRotation(vector3df(0.0f, 0.0f, 0.0f));

	mScene->addLightSceneNode(nullptr, vector3df(-500.0f, 500.0f, -500.0f), SColor(255, 255, 255, 255), 2000);

	mTerrain = new Terrain(mScene, mDriver, mCamera);
	TerrainNode::Wireframe = mWireframe;
	
	IGUISkin *skin = mGui->getSkin();
	IGUIFont *font = mGui->getFont("fonts/Consolas/Consolas.xml");

	skin->setFont(font);
	skin->setColor(EGDC_BUTTON_TEXT, SColor(255, 255, 255, 255));

	IGUIStaticText *txtCam		= mGui->addStaticText(L"Camera: (0, 0, 0)", recti(10, 10, 600, 40));
	IGUIStaticText *txtTri		= mGui->addStaticText(L"Triangles: 0", recti(10, 40, 600, 70));
	
	mHUD["Camera"]		= txtCam;
	mHUD["Triangles"]	= txtTri;
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

		update();
		updateGUI();
		
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

	if (mEventReciever->KeyHit(KEY_F2))
		TerrainNode::Normals = !TerrainNode::Normals;

	if (mEventReciever->KeyHit(KEY_F3)) {
		mWireframe = !mWireframe;
		TerrainNode::Wireframe = mWireframe;
		mTerrain->setMaterialFlag(EMF_WIREFRAME, mWireframe);
	}

	if (mEventReciever->KeyHit(KEY_KEY_Q)) {
		bool enabled = mCamera->isInputReceiverEnabled();
		mCamera->setInputReceiverEnabled(!enabled);
		mDevice->getCursorControl()->setVisible(enabled);
	}
}

void App::update() {
	float speed	= log((mCamera->getPosition().getDistanceFrom(vector3df()) + 4.0f) / 200.0f) / 5.0f;
	ISceneNodeAnimatorCameraFPS *anim = (ISceneNodeAnimatorCameraFPS*)*mCamera->getAnimators().begin();
	anim->setMoveSpeed(speed);
}

void App::updateGUI() {
	mHUD["Camera"]->setText(toMultiByte("Camera: " + vecToString(mCamera->getPosition())).c_str());
	mHUD["Triangles"]->setText(toMultiByte("Triangles: " + toString(TerrainNode::Triangles, 10)).c_str());
}

template<class T> inline std::string App::vecToString(irr::core::vector3d<T> v) {
	std::ostringstream ss;
	ss.precision(4);
	ss << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
	
	return ss.str();
}


template <class T> std::string App::toString(T v, size_t precision) {
	std::ostringstream ss;
	ss.precision(precision);
	ss << v;

	return ss.str();
}

std::wstring App::toMultiByte(std::string str) {
	size_t conv;
	const size_t size = str.length() + 1;
	wchar_t *wc = new wchar_t[size];
	mbstowcs_s(&conv, wc, size, str.c_str(), size);
	std::wstring nstr = wc;
	delete wc;

	return nstr;
}

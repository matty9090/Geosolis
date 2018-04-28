#include <irr/irrlicht.h>
#include <string>

class App { 
	public:
		App(std::wstring title);

		int run();

	private:
		irr::IrrlichtDevice *mDevice;
		irr::video::IVideoDriver *mDriver;
		irr::scene::ISceneManager *mScene;
		irr::gui::IGUIEnvironment *mGui;
};
#include <irr/irrlicht.h>
#include <string>

#include "Terrain.hpp"

class EventReciever : public irr::IEventReceiver {
	public:
		EventReciever() {
			for (irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i)
				KeyIsDown[i] = false;
		}

		virtual bool OnEvent(const irr::SEvent& event) {
			if (event.EventType == irr::EET_KEY_INPUT_EVENT)
				KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

			return false;
		}

		virtual bool IsKeyDown(irr::EKEY_CODE keyCode) const {
			return KeyIsDown[keyCode];
		}
	
	private:
		bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
};

class App  {
	public:
		App(std::wstring title);
		~App();

		int run();

	private:
		irr::IrrlichtDevice *mDevice;
		irr::video::IVideoDriver *mDriver;
		irr::scene::ISceneManager *mScene;
		irr::gui::IGUIEnvironment *mGui;

		irr::u32 dt;
		irr::core::stringw mTitle;

		Terrain *mTerrain;
		EventReciever *mEventReciever;

		void handleEvents();
};
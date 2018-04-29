#include <irr/irrlicht.h>
#include <string>

#include "Terrain.hpp"

class EventReciever : public irr::IEventReceiver {
	public:
		enum KeyState { NotPressed, Pressed, Held };

		EventReciever() {
			for (irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; ++i)
				keyStates[i] = NotPressed;
		}

		virtual bool OnEvent(const irr::SEvent& event) {
			if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
				if (event.KeyInput.PressedDown) {
					if (keyStates[event.KeyInput.Key] == NotPressed)
						keyStates[event.KeyInput.Key] = Pressed;
					else
						keyStates[event.KeyInput.Key] = Held;
				} else
					keyStates[event.KeyInput.Key] = NotPressed;
			}

			return false;
		}

		virtual bool KeyHeld(irr::EKEY_CODE keyCode) {
			if (keyStates[keyCode] == NotPressed)
				return false;

			keyStates[keyCode] = Held;

			return true;
		}

		virtual bool KeyHit(irr::EKEY_CODE keyCode) {
			if (keyStates[keyCode] == Pressed) {
				keyStates[keyCode] = Held;
				return true;
			}

			return false;
		}
	
	private:
		KeyState keyStates[irr::KEY_KEY_CODES_COUNT];
};

class App  {
	public:
		App(std::wstring title);
		~App();

		int run();

	private:
		bool mWireframe;
		
		irr::IrrlichtDevice *mDevice;
		irr::video::IVideoDriver *mDriver;
		irr::scene::ISceneManager *mScene;
		irr::gui::IGUIEnvironment *mGui;

		irr::u32 dt;
		irr::core::stringw mTitle;

		Terrain *mTerrain;
		EventReciever *mEventReciever;
		irr::scene::ISceneNode *mTerrainNode;

		void handleEvents();
};
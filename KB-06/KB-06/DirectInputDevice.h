#ifndef __INPUT_DIRECTINPUTDEVICE_H__
#define __INPUT_DIRECTINPUTDEVICE_H__

//Define version to directinput version 8.
//If not defined, it will default to version 8 as well, but we like a sense of security.
#define DIRECTINPUT_VERSION 0x0800

#include "InputDevice.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

namespace Input
{
	class DirectInputDevice : public InputDevice
	{
	public:
		DirectInputDevice();
		~DirectInputDevice();
		virtual bool Initialize(HWND, LPDIRECTINPUT8) = 0;
		bool AcquireDevice();
		void ReleaseDevice();
		virtual bool Update() = 0;
		void SetActionMapping(std::map<Input, int>* actionMapping);

	protected:
		std::map<Input, int>* actionMapping;
		LPDIRECTINPUTDEVICE8 dInputDevice;
	};
}

#endif
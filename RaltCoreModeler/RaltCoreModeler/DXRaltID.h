#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

#define KS(x) ((x & 0x80) ? TRUE : FALSE)

enum DXRaltID_rgbb
{
	DXRID_LEFT_BUTTON = 0,
	DXRID_RIGHT_BUTTON = 1,
	DXRID_MIDDLE_BUTTON = 2,

	DXRID_END_BUTTON = 4,
};

class DXRaltCoreApp;
class DXRaltID
{
	public:
		DXRaltID();
		~DXRaltID() { Release(); };

		HRESULT Init(HINSTANCE hinstance, HWND hwnd);
		HRESULT InitKeyboard(HWND hwnd);
		HRESULT InitMouse(HWND hwnd);

		void UpdateInputDevice(INT d_time);
		void UpdateInputDeviceWithDelay(INT d_time);
		HRESULT Update();

		void OnMouseMove(bool inBlock, INT blockID, int x, int y);
		void OnMouseDownUp(UINT type_mb, int x, int y, BOOL down);
		void OnKeyDown(UINT btnState);
		void OnKeyUp(UINT btnState);
		void OnMouseWheel();
		
		void OnFocus();
		void OnClick();

		unsigned char KBState(INT index) { return g_keyboardState[index]; };
		DIMOUSESTATE MouseState() { return g_mouseState; };

		BOOL GetrBMClck() { return rBMClick; };

		BOOL GetSphereRS() { return sphereRS1; };
		BOOL GetShowWL() { return showWL; };

		FLOAT GetMouseXPos() { return mouseXpos; };
		FLOAT GetMouseYPos() { return mouseYpos; };

		void UpdateMousePos(FLOAT x, FLOAT y);

		void Release();

	private:
		IDirectInput8*			g_directInput = NULL;
		IDirectInputDevice8*	g_keyboard = NULL;
		IDirectInputDevice8*	g_mouse = NULL;

		unsigned char			g_keyboardState[256];
		DIMOUSESTATE			g_mouseState;

		INT						bID;
		BOOL					sphereRS1, showWL;
		BOOL					lBMClick, rBMClick, lshift, lctrl;
		FLOAT					mouseXpos, mouseYpos, o_mouseXpos, o_mouseYpos;

};
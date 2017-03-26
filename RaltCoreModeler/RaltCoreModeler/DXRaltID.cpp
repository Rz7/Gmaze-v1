#include "DXRaltID.h"


DXRaltID::DXRaltID()
{
	g_keyboard = 0;
	g_mouse = 0;
	g_directInput = 0;
}

HRESULT DXRaltID::Init(HINSTANCE hinstance, HWND hwnd)
{
	HRESULT hr;
	lBMClick = false;
	rBMClick = false;
	mouseXpos = 0.0f;
	mouseYpos = 0.0f;

	sphereRS1 = FALSE;

	bID = 0;

	hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_directInput, NULL);
	if (FAILED(hr))
	{
		//DRCA()->GetLog()->WriteLog(LOG_ERROR_INITCR8);
		return hr;
	}

	hr = InitKeyboard(hwnd);
	if (FAILED(hr))
	{
		//DRCA()->GetLog()->WriteLog(LOG_ERROR_IKEYBOARD);
		return hr;
	}

	hr = InitMouse(hwnd);
	if (FAILED(hr))
	{
		//DRCA()->GetLog()->WriteLog(LOG_ERROR_IMOUSE);
		return hr;
	}

	return S_OK;
}

HRESULT DXRaltID::InitKeyboard(HWND hwnd)
{
	HRESULT hr;
	hr = g_directInput->CreateDevice(GUID_SysKeyboard, &g_keyboard, NULL);
	if (FAILED(hr))
		return hr;

	hr = g_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		return hr;

	hr = g_keyboard->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return hr;

	hr = g_keyboard->Acquire();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT DXRaltID::InitMouse(HWND hwnd)
{
	HRESULT hr;

	hr = g_directInput->CreateDevice(GUID_SysMouse, &g_mouse, NULL);
	if (FAILED(hr))
		return hr;

	hr = g_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		return hr;

	hr = g_mouse->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return hr;

	hr = g_mouse->Acquire();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void DXRaltID::UpdateInputDevice(INT d_time)
{
	if (FAILED(Update()) || DRCA()->GetHWND() != GetActiveWindow())
	{
		o_mouseXpos = mouseXpos;
		o_mouseYpos = mouseYpos;
		return;
	}

	// Вычисление блока, в пределах которого находится курсор мыши
	bID = DRCA()->GetInterface()->GetBlockIdInCoord(mouseXpos, mouseYpos);

	FLOAT timeUpDelay = d_time;

	FLOAT d_XPOS = mouseXpos - o_mouseXpos;
	FLOAT d_YPOS = mouseYpos - o_mouseYpos;

	for (UINT i = DXRID_LEFT_BUTTON; i < DXRID_END_BUTTON; ++i)
		OnMouseDownUp(i, g_mouseState.lX, g_mouseState.lY, g_mouseState.rgbButtons[i]);

	OnMouseMove(bID != -1, bID, g_mouseState.lX, g_mouseState.lY);
	OnMouseWheel(); // Прокрутка колесика мыши

	// Обновление фокусов
	OnFocus();

	// Если минимум 1 блок в фокусе мыши
	if (bID != -1 && g_mouseState.rgbButtons[DXRID_LEFT_BUTTON])
		OnClick();

	lshift = KS(g_keyboardState[DIK_LSHIFT]);
	lctrl = KS(g_keyboardState[DIK_LCONTROL]);

	if (KS(g_keyboardState[DIK_S]))
		OnKeyDown(DIK_S);

	UpdateInputDeviceWithDelay(timeUpDelay);

	o_mouseXpos = mouseXpos;
	o_mouseYpos = mouseYpos;
}

void DXRaltID::UpdateInputDeviceWithDelay(INT d_time)
{
	static INT time_wd = 0;
	time_wd += d_time;

	if (time_wd >= 80)
		time_wd = 0;
	else return;

	if (KS(g_keyboardState[DIK_H]))
		sphereRS1 = !sphereRS1;

	if (KS(g_keyboardState[DIK_1]))
	{
		RADXModel* model = new RADXModel();
		model->InitStInfo("Cube");

		Cube* cb = DRCA()->CubeList[0];

		model->CopyCubeToModel(cb);
	}

	if (KS(g_keyboardState[DIK_2]))
	{
		RADXModel* model = new RADXModel();
		model->InitStInfo("Pyramid");

		Pyramid* prmd = DRCA()->PyramidList[0];

		model->CopyPyramidToModel(prmd);
	}

	if (KS(g_keyboardState[DIK_3]))
	{
		RADXModel* model = new RADXModel();
		model->InitStInfo("EPolygon");

		EPolygon* ep = DRCA()->PolygonList[0];

		model->CopyEPolygonToModel(ep);
	}

	if (KS(g_keyboardState[DIK_4]))
	{
		RADXModel* model = new RADXModel();
		model->InitStInfo("Sphere");

		Sphere* sp = DRCA()->SphereList[0];

		model->CopySphereToModel(sp);
	}

	if (KS(g_keyboardState[DIK_8]) && g_mouseState.rgbButtons[DXRID_LEFT_BUTTON])
	{
		D3DXVECTOR3 coord_pos = DRCA()->GetGeometry()->GetSelPointInGridPlace();

		Sphere* new_sphere = new Sphere();
		new_sphere->SetSDV(coord_pos);
		new_sphere->LoadEM();

		RADXModel* model = new RADXModel();
		model->InitStInfo("Sphere");
		model->CopySphereToModel(new_sphere);

		model->SaveModelToFile();
	}

	if (KS(g_keyboardState[DIK_9]))
	{
		RADXModel* model = new RADXModel();
		model->InitStInfo("Sphere");
		model->LoadModelFromFile();
	}

	showWL = KS(g_keyboardState[DIK_5]);
}

HRESULT DXRaltID::Update()
{
	HRESULT hr;

	hr = g_keyboard->GetDeviceState(sizeof(UCHAR[256]), (LPVOID)&g_keyboardState);
	if (FAILED(hr))
		return hr;

	hr = g_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&g_mouseState);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void DXRaltID::OnMouseMove(bool inBlock, INT blockID, int x, int y)
{
	if (inBlock)
	{
		DRCA()->GetInterface()->GetBlockById(blockID)->OnMove(mouseXpos, mouseYpos, false);
	}
	else
	{
		FLOAT dx = 0.2f * x;	// width
		FLOAT dy = 0.2f * y;	// heigth

		DRCA()->GetCamera()->RotateCamera(rBMClick, dy, dx);
	}
}

void DXRaltID::OnMouseDownUp(UINT type_mb, int x, int y, BOOL down)
{
	switch (type_mb)
	{
		case DXRID_LEFT_BUTTON:
		{
			RADXIBlock* block = DRCA()->GetInterface()->GetBlockById(bID);

			if (lBMClick && !down)
			{
				// Клавиша была нажата, а теперь отпущена
				if (block)
					block->OnLKUp(mouseXpos, mouseYpos);
			}

			if (!lBMClick && down)
			{
				// Клавиша была отпущена, а теперь нажата
				if (block)
					block->OnLKDown(mouseXpos, mouseYpos);
			}

			lBMClick = down;
		}	
		break;
		case DXRID_RIGHT_BUTTON:
			rBMClick = down;
		break;
		case DXRID_MIDDLE_BUTTON: break;
		default: break;
	}
}

void DXRaltID::OnFocus()
{
	// Проверка фокуса мыши на какой-либо из блоков
	for each (RADXIBlock* block in DRCA()->GetInterface()->IBlockList)
		block->OnFocus(mouseXpos, mouseYpos);
}

void DXRaltID::OnClick()
{
	// Проверка клика ЛКМ на какой-либо из блоков
	for each (RADXIBlock* block in DRCA()->GetInterface()->IBlockList)
		block->OnClick(mouseXpos, mouseYpos);
}

void DXRaltID::OnMouseWheel()
{
	if (g_mouseState.lZ == 0)
		return;

	DRCA()->GetCamera()->ZoomingCamera(g_mouseState.lZ);
}

void DXRaltID::OnKeyDown(UINT btnState)
{
	BOOL ud = FALSE;
	FLOAT updown = 0;
	FLOAT left_rot = 0;
	FLOAT forward_r = 0;

	FLOAT speed_up = 1.0f;

	switch (btnState)
	{
		case DIK_A: left_rot = -30.0f; break;	// a
		case DIK_D: left_rot = 30.0f; break;	// d
		case DIK_S: forward_r = -30.0f; break;	// s
		case DIK_W: forward_r = 30.0f; break;	// w
		case DIK_SPACE:							// space
			if (!lctrl && !lshift)
				return;

			ud = TRUE;
			if (lctrl)
				updown = -30.0f;
			else updown = 30.0f;
		break;
		//case DIK_F4: DRCA()->SFS(); break;
	}

	updown *= speed_up;
	forward_r *= speed_up;
	left_rot *= speed_up;

	//DRCA()->GetCamera()->MoveCamera(ud, updown, forward_r, left_rot);
}

void DXRaltID::OnKeyUp(UINT btnState)
{
	switch (btnState)
	{
		case DIK_LSHIFT: lshift = FALSE; return;
		case DIK_LCONTROL: lctrl = FALSE; return;
	}
}

void DXRaltID::UpdateMousePos(FLOAT x, FLOAT y)
{
	mouseXpos = x;
	mouseYpos = y;
}

void DXRaltID::Release()
{
	if (g_keyboard)
	{
		g_keyboard->Unacquire();
		SafeRelease(g_keyboard);
	}

	if (g_mouse)
	{
		g_mouse->Unacquire();
		SafeRelease(g_mouse);
	}

	SafeRelease(g_directInput);
}

#pragma once

#include "resource.h"

#include <windows.h>
#include <Windowsx.h>
#include <WinDef.h>

#include <algorithm>
#include <memory>
#include <vector>
#include <list>
#include <tchar.h>
#include <comdef.h>

#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx9core.h>
#include <d3dx11effect.h>
#include <dinput.h>

using namespace std;

#define WINDOW_WIDTH  ::GetSystemMetrics(SM_CXSCREEN) < 1280 ? ::GetSystemMetrics(SM_CXSCREEN) : 1280
#define WINDOW_HEIGHT ::GetSystemMetrics(SM_CYSCREEN) < 800  ? ::GetSystemMetrics(SM_CYSCREEN) - 50 : 800

HRESULT             InitWindow(int nCmdShow);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

#define randomize() srand((unsigned)GetTickCount())
#define SafeRelease(x) if (x) { x->Release(); x = NULL; }
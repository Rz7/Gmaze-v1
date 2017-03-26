//--------------------------------------------------------------------------------------
//  RaltCore Map Creator
//--------------------------------------------------------------------------------------
#include "afxwin.h"
#include "Resource.h"
#include "RaltCoreModeler.h"
#include "DRXCA.h"

HINSTANCE					g_hInstance = NULL;
HWND						g_hWnd = NULL;
DRXCA*						drxcaM = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;
	if (FAILED(InitWindow(nCmdShow)))
		return 0;

	// Инициализация DRXCA
	drxcaM = new DRXCA(g_hInstance, g_hWnd);

	MSG msg = { 0 };
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		if (!drxcaM)
			continue;

		if (drxcaM->GetExit())
			break;

		if (!drxcaM->GetIntD())
			continue;

		drxcaM->GenScene();

		// Обновление с указанием времени (счетчик)
		drxcaM->GetUpdater()->Update();
	}

	return (int)msg.wParam;
}

//--------------------------------------------------------------------------------------
// Регистрация класса и создание окна
//--------------------------------------------------------------------------------------
HRESULT InitWindow(int nCmdShow)
{
	// Регистрируем класс окна
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "SimpleWindowClass";

	wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_RALTCOREMODELER));
	wc.hIconSm = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_SMALL));

	if (!RegisterClassEx(&wc))
		return E_FAIL;

	// Создаем окно
	RECT rect = { 0, 0, WINDOW_WIDTH + 20, WINDOW_HEIGHT + 43 };
	g_hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "SimpleWindowClass", "RAltCore Modeler 0.0.7[IN DEV]", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, g_hInstance, NULL);

	//Если не удалось создать окно - выходим из функции
	if (!g_hWnd)
		return E_FAIL;

	//Отображаем окно на экране
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Обработка сообщений
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
	
		case WM_SIZE:
			if (drxcaM && drxcaM->GetIntD())
				drxcaM->ResizeBuffers();
		break;

		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* mmi = (MINMAXINFO*)lParam;
			mmi->ptMinTrackSize.x = 800;
			mmi->ptMinTrackSize.y = 680;
		}
		break;

		case WM_MOUSEMOVE:
			DRCA()->GetRaltID()->UpdateMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

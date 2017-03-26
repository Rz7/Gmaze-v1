#include "DRXCA.h"


string maindir = "C:\\RaltCoreMod\\";
string MainDir() { return maindir; };

DRXCA* drxca;
DRXCA* DRCA() { return drxca; };

DRXCA::DRXCA(HINSTANCE hInst, HWND hWND)
{
	DX_HIns = hInst;
	DX_HWND = hWND;
	drxca = this;
	Init();
}

DRXCA::~DRXCA()
{
	Break();
}

void DRXCA::Break()
{
	initDone = false;
	exit = false;

	delete mCamera;
	delete radxMath;
	delete gridPlace;
	delete rADXUpdater;
	delete rADXInterface;
	delete rIDController;
	
	delete wModel;
	delete radxGeometry;

	delete loginfo;
	delete console;
	delete eventScripts;

	SafeRelease(modelShaderTechnique);
	SafeRelease(modelShaderEffect);
	SafeRelease(modelShaderWorldVariable);
	SafeRelease(modelShaderViewVariable);
	SafeRelease(modelShaderProjectionVariable);
	SafeRelease(modelShaderColor);
	SafeRelease(modelShaderLightColor);
	SafeRelease(modelShaderLightPosition);

	SafeRelease(m_pRasterStateSolidCMF);
	SafeRelease(m_pRasterStateSolidCMN);

	SafeRelease(MapSegmentEffect);

	SafeRelease(g_pd3dDeviceContext);
	SafeRelease(g_pDepthStencilView);
	SafeRelease(g_pRenderTargetView);
	SafeRelease(g_pSwapChain);
	SafeRelease(g_pd3dDevice);
}

void DRXCA::Init()
{
	// Инициализация консоли
	console = new Console();

	// Инициализация логгера
	loginfo = new LogInfo();

	// Инициализация класса с математическими функциями и функциями расчета
	radxMath = new RADXMath();

	// Инициализация класса с геометрическими функциями и функциями расчета
	radxGeometry = new RADXGeometry();

	// Инициализация класса редактирования модели
	wModel = new WarpModel();

	// Инициализация обработчика событий
	eventScripts = new EventScripts();
	eventScripts->Init();

	// Инициализация Апдейтера
	rADXUpdater = new RADXUpdater();

	HRESULT hr = InitDirect3DGraphic();
	if (FAILED(hr))
	{
		Break();
		return;
	}

	// Инициализация интерфейса
	rADXInterface = new RADXInterface();
	rADXInterface->CompileShader();
	rADXInterface->OnResize(DRCA()->GetWidth(), DRCA()->GetHeight());
	rADXInterface->InitInterface();

	// Формируем пол для модели
	gridPlace = new GridPlace();

	// Инициализация камеры
	mCamera = new RADXCamera();
	mCamera->OnResize(width, height);

	// Инициализация шейдера моделей
	SetModelShader();

	// Инициализация контроллера клавиатуры и мыши
	rIDController = new DXRaltID();
	rIDController->Init(DX_HIns, DX_HWND);

	Cube* new_cube = new Cube(height, width);
	new_cube->LoadEM();
	CubeList.push_back(new_cube);

	Sphere* new_sphere = new Sphere();
	new_sphere->LoadEM();
	SphereList.push_back(new_sphere);

	Pyramid* new_pyramid = new Pyramid();
	new_pyramid->LoadEM();
	PyramidList.push_back(new_pyramid);

	EPolygon* new_polygon = new EPolygon();
	new_polygon->LoadEM();
	PolygonList.push_back(new_polygon);

	initDone = true;
}

HRESULT DRXCA::InitDirect3DGraphic()
{
	HRESULT hr;

	// Размеры клиентской области окна окна
	RECT rc;
	GetClientRect(DX_HWND, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	// Список возможных типов устройства
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = DX_HWND;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	// Пытаемся создать устройство, проходя по списку
	// как только получилось - выходим из цикла
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D3D_FEATURE_LEVEL m_featureLevel;
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		g_driverType = driverTypes[driverTypeIndex];

		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd,
			&g_pSwapChain, &g_pd3dDevice, &m_featureLevel, NULL);
		if (SUCCEEDED(hr))
		{
			GetLog()->WriteLog(LOG_INFO_DEVLOAD);
			break;
		}
	}
	if (FAILED(hr))
	{
		GetLog()->WriteLog(LOG_ERROR_LOAD_DEVICE);
		return hr;
	}

	/*ID3D10Blob*	g_pEffectBlob_dvx = 0;
	string data_effect = maindir + "/Shaders/dvx.fx";
	hr = D3DX11CompileFromFile(data_effect.c_str(), NULL, NULL, "VS", "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS, NULL, NULL, &g_pEffectBlob_dvx, NULL, NULL);
	if (FAILED(hr))
		return hr;

	hr = D3DX11CreateEffectFromMemory(g_pEffectBlob_dvx->GetBufferPointer(), g_pEffectBlob_dvx->GetBufferSize(), 0, g_pd3dDevice, &MapSegmentEffect);
	if (FAILED(hr))
	{
		GetLog()->WriteLog(LOG_ERROR_MAPSE);
		return hr;
	}

	SafeRelease(g_pEffectBlob_dvx);*/

	// Задаем тип и способ построения примитивов
	g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);
	g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	ResizeBuffers();
	SetRasterizerState();
	SetSamplerState();
	
	GetLog()->WriteLog(LOG_INFO_3DGRAPHIC);
	return S_OK;
}

void DRXCA::ResizeBuffers()
{
	HRESULT hr = S_OK;
	SafeRelease(g_pRenderTargetView);

	if (g_pSwapChain)
		g_pSwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	BOOL FS;
	width = 0;
	height = 0;

	g_pSwapChain->GetFullscreenState(&FS, NULL);

	if (FS)
	{
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		RECT rc;
		GetClientRect(DX_HWND, &rc);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;
	}

	if (rADXInterface)
	{
		rADXInterface->InitRLWBuffers();
		rADXInterface->OnResize(width, height);
	}		

	if (mCamera)
		mCamera->OnResize(width, height);

	// Представление данных для буфера визуализации
	ID3D11Texture2D *pBackBuffer_r;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer_r);
	if (FAILED(hr))
		return;

	// Создадим представление данных как буфера визуализации
	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer_r, NULL, &g_pRenderTargetView);
	SafeRelease(pBackBuffer_r);
	if (FAILED(hr))
		return;

	SetDepthStencilState();

	vpMain.Width = (FLOAT)width;
	vpMain.Height = (FLOAT)height;
	vpMain.MinDepth = 0.0f;
	vpMain.MaxDepth = 1.0f;
	vpMain.TopLeftX = 0;
	vpMain.TopLeftY = 0;
	SetViewPort(vpMain);
}

void DRXCA::Clear2Buffer()
{
	// Очищаем вторичный буфер
	static float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	g_pd3dDeviceContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pd3dDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DRXCA::PresentScene()
{
	g_pSwapChain->Present(1, 0);
}

void DRXCA::GenScene()
{
	Clear2Buffer();

	for each (Cube* cube in CubeList)
		cube->Render();

	for each (Sphere* sphere in SphereList)
		sphere->Render();

	for each (Pyramid* pyramid in PyramidList)
		pyramid->Render();

	for each (EPolygon* polygon in PolygonList)
		polygon->Render();

	for each (RADXModel* model in ModelList)
		model->Render();

	// Рендер сетчатого поля для модели
	gridPlace->Render();

	// Рендер интерфейса
	rADXInterface->Render();

	PresentScene();
}

HRESULT DRXCA::SetDepthStencilState()
{
	HRESULT hr = S_OK;

	SafeRelease(g_pDepthStencil);
	//SafeRelease(m_pDepthStencilState);
	//SafeRelease(m_pDepthDisabledStencilState);
	SafeRelease(g_pDepthStencilView);

	// Создаем шаблонный буфер глубины
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	/*
	hr = g_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	if (FAILED(hr))
		return hr;

	depthStencilDesc.DepthEnable = FALSE;
	hr = g_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthDisabledStencilState);
	if (FAILED(hr))
		return hr;
	*/

	// Создаем представление данных как шаблонного буфера глубины
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2DMS.UnusedField_NothingToDefine = 0;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);

	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
	return hr;
}

HRESULT DRXCA::SetRasterizerState()
{
	HRESULT hr = S_OK;

	SafeRelease(m_pRasterStateSolidCMF);
	SafeRelease(m_pRasterStateSolidCMN);

	// Создаем буфер растеризатора
	D3D11_RASTERIZER_DESC  RasterDesc;
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_FRONT;
	RasterDesc.FrontCounterClockwise = true;
	RasterDesc.DepthBias = false;
	RasterDesc.SlopeScaledDepthBias = 0.0f;
	RasterDesc.DepthBiasClamp = 0.0f;
	RasterDesc.DepthClipEnable = true;
	RasterDesc.ScissorEnable = false;
	RasterDesc.MultisampleEnable = false;
	RasterDesc.AntialiasedLineEnable = false;
	g_pd3dDevice->CreateRasterizerState(&RasterDesc, &m_pRasterStateSolidCMF);

	RasterDesc.CullMode = D3D11_CULL_NONE;
	RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	g_pd3dDevice->CreateRasterizerState(&RasterDesc, &m_pRasterStateSolidCMN);

	SetRSState(0);
	return hr;
}

HRESULT DRXCA::SetSamplerState()
{
	HRESULT hr = S_OK;

	SafeRelease(g_pPointSampler);

	float border[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	CD3D11_SAMPLER_DESC pointSamplerDesc(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP, 0.f, 32, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

	hr = g_pd3dDevice->CreateSamplerState(&pointSamplerDesc, &g_pPointSampler);
	if (FAILED(hr))
		return hr;

	//SetSSState(true);
	//g_pd3dDeviceContext->PSSetSamplers(0, 1, &g_pPointSampler);
	return hr;
}

void DRXCA::SetModelShader()
{
	HRESULT hr = S_OK;

	ID3D10Blob*	g_pEffectBlob = 0;

	string shader_data = MainDir() + "/Shaders/model.fx";
	hr = D3DX11CompileFromFile(shader_data.c_str(), NULL, NULL, "VS", "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS, NULL, NULL, &g_pEffectBlob, NULL, NULL);
	if (FAILED(hr))
		return;

	hr = D3DX11CreateEffectFromMemory(g_pEffectBlob->GetBufferPointer(), g_pEffectBlob->GetBufferSize(), 0, DRCA()->GetDevice(), &modelShaderEffect);
	if (FAILED(hr))
		return;

	SafeRelease(g_pEffectBlob);

	// Связь с переменными шейдера
	modelShaderTechnique = modelShaderEffect->GetTechniqueByName("RenderTexture");

	modelShaderWorldVariable = modelShaderEffect->GetVariableByName("World")->AsMatrix();
	modelShaderViewVariable = modelShaderEffect->GetVariableByName("View")->AsMatrix();
	modelShaderProjectionVariable = modelShaderEffect->GetVariableByName("Projection")->AsMatrix();

	modelShaderColor = modelShaderEffect->GetVariableByName("Color")->AsVector();
	modelShaderLightColor = modelShaderEffect->GetVariableByName("LightColor")->AsVector();
	modelShaderLightPosition = modelShaderEffect->GetVariableByName("LightDirection")->AsVector();
}

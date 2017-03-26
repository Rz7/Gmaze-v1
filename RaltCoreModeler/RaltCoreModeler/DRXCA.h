#pragma once
#include "RaltCoreModeler.h"
#include "Console.h"
#include "RADXCamera.h"
#include "LogInfo.h"
#include "DXRaltID.h"
#include "GridPlace.h"
#include "RADXUpdater.h"
#include "RADXTexture.h"
#include "EventScripts.h"

#include "RADXInterface.h"
#include "RADXIText.h"

#include "Cube.h"
#include "Sphere.h"
#include "Pyramid.h"
#include "EPolygon.h"

#include "RADXMath.h"
#include "RADXGeometry.h"

#include "RADXModel.h"
#include "WarpModel.h"


string MainDir();

class DRXCA;
DRXCA* DRCA();

class RADXTexture;

class Cube;
class Sphere;
class Pyramid;
class EPolygon;

class Console;
class LogInfo;

class DXRaltID;
class RADXCamera;

class GridPlace;
class RADXUpdater;
class EventScripts;

class RADXInterface;
class RADXIText;

class RADXMath;
class RADXGeometry;

class RADXModel;
class WarpModel;

class DRXCA
{
	public:
		DRXCA(HINSTANCE hInst, HWND hWND);
		~DRXCA();

		void Init();
		HRESULT InitDirect3DGraphic();

		void Clear2Buffer();
		void PresentScene();
		void GenScene();

		void ResizeBuffers();
		void Break();

		void SetViewPort(D3D11_VIEWPORT vp) { g_pd3dDeviceContext->RSSetViewports(1, &vp); }

		Console* GetConsole() { return console; };
		LogInfo* GetLog() { return loginfo; };

		BOOL GetIntD()	{ return initDone; };
		BOOL GetExit() { return exit; };
		HWND GetHWND() { return DX_HWND; };

		DXRaltID* GetRaltID() { return rIDController; };
		RADXCamera* GetCamera() { return mCamera; };
		
		RADXUpdater* GetUpdater() { return rADXUpdater; };
		RADXInterface* GetInterface() { return rADXInterface; };

		RADXMath* GetMath() { return radxMath; };
		RADXGeometry* GetGeometry() { return radxGeometry; };

		GridPlace* GetGridPlace() { return gridPlace; };

		WarpModel* GetWarpModel() { return wModel; };

		EventScripts* GetEventScripts() { return eventScripts; };
		
		std::vector<Cube*> CubeList;
		std::vector<Sphere*> SphereList;
		std::vector<Pyramid*> PyramidList;
		std::vector<EPolygon*> PolygonList;
		std::vector<RADXModel*> ModelList;

		ID3D11Device* GetDevice() { return g_pd3dDevice; };
		ID3D11DeviceContext* GetDeviceContext() { return g_pd3dDeviceContext; };

		INT GetHeight() { return height; };
		INT GetWidth() { return width; };

		HRESULT SetDepthStencilState();
		HRESULT SetRasterizerState();
		HRESULT SetSamplerState();

		void SetRSState(int number)
		{
			switch (number)
			{
				case 0: g_pd3dDeviceContext->RSSetState(m_pRasterStateSolidCMF); break;
				case 1: g_pd3dDeviceContext->RSSetState(m_pRasterStateSolidCMN); break;
			}
		}

		void SaveStringToStream(ofstream& out, string str)
		{
			size_t len = str.size();
			out.write((char*)&len, sizeof(size_t));
			out.write(str.c_str(), str.length());
		}

		string LoadStringFromStream(ifstream& in)
		{
			string result;

			size_t len;
			in.read((char*)&len, sizeof(size_t));
			char* temp = new char[len + 1];
			in.read(temp, len);
			temp[len] = '\0';
			result = temp;
			delete[] temp;

			return result;
		}

		void SetModelShader();

	private:
		HWND				DX_HWND;
		HINSTANCE			DX_HIns;
		BOOL				initDone;		// Конец инициализации
		BOOL				exit;			// Выход из программы

		Console*			console;		// Консоль
		LogInfo*			loginfo;		// Логгер
		RADXCamera*			mCamera;		// Главная камера
		
		GridPlace*			gridPlace;		// Сетка-пол для модели
		DXRaltID*			rIDController;	// Device input
		RADXUpdater*		rADXUpdater;	// Обновление с указанием времени
		RADXInterface*		rADXInterface;	// Интерфейс
		EventScripts*		eventScripts;	// Обработчик событий

		RADXMath*			radxMath;		// Различные математические функции и функции расчетов
		RADXGeometry*		radxGeometry;	// Различные геометрические функции и функции расчетов

		WarpModel*			wModel;			// Класс изменения модели

		INT height;					// Высота окна
		INT width;					// Ширина окна

		D3D11_VIEWPORT				vpMain;
		D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
		ID3D11Device*				g_pd3dDevice = NULL;

		ID3D11DeviceContext*		g_pd3dDeviceContext = NULL;

		IDXGISwapChain*				g_pSwapChain = NULL;
		IDXGISurface1*				g_pdxgiSurface = NULL;

		ID3D11SamplerState*			g_pPointSampler = NULL;
		ID3D11RenderTargetView*		g_pRenderTargetView = NULL;

		ID3D11Texture2D*            g_pDepthStencil = NULL;
		ID3D11DepthStencilView*     g_pDepthStencilView = NULL;

		ID3D11RasterizerState*		m_pRasterStateSolidCMF = NULL;
		ID3D11RasterizerState*		m_pRasterStateSolidCMN = NULL;

		ID3DX11Effect*				MapSegmentEffect = NULL;


	public:
		//////////////////////////////
		// Model Shader Information //
		//////////////////////////////

		ID3DX11EffectTechnique*					modelShaderTechnique = NULL;

		ID3DX11Effect*							modelShaderEffect = NULL;
		ID3DX11EffectMatrixVariable*			modelShaderWorldVariable = NULL;
		ID3DX11EffectMatrixVariable*			modelShaderViewVariable = NULL;
		ID3DX11EffectMatrixVariable*			modelShaderProjectionVariable = NULL;

		ID3DX11EffectVectorVariable*			modelShaderColor = NULL;
		ID3DX11EffectVectorVariable*			modelShaderLightColor = NULL;
		ID3DX11EffectVectorVariable*			modelShaderLightPosition = NULL;
};

#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

#include "RADXIBlock.h"
#include "RADXICompiler.h"

class RADXTexture;
class RADXIBlock;
class RADXICompiler;
class RADXInterface
{
	struct StandartBlock
	{
		D3DXVECTOR2 Pos;
		D3DXVECTOR2 Tex;
	};

	public:
		RADXInterface();
		~RADXInterface();

		void InitInterface();
		//void InitFont();
		void SetCamera(INT width, INT heigth);

		void CompileShader();

		void InitRLWBuffers();

		// Функция создания интерфейса
		void Render();

		// Функция рендера окна загрузки
		void RenderLoadWindow();

		void OnResize(INT width, INT heigth);
		
		BOOL InitDone() { return initDone; };
		RADXICompiler* GetICompiler() { return iCompiler; };

		RADXIBlock* GetBlockByName(string bname);
		RADXIBlock* GetBlockById(INT bId);
		INT GetBlockIdInCoord(INT x, INT y);

	private:
		BOOL initDone;

		RADXICompiler*							iCompiler;

		ID3DX11EffectMatrixVariable*			g_pWorldVariable = NULL;
		ID3DX11EffectMatrixVariable*			g_pViewVariable = NULL;
		ID3DX11EffectMatrixVariable*			g_pProjectionVariable = NULL;

		D3DXMATRIX								InterfaceView;
		D3DXMATRIX								InterfaceProjection;
		D3DXMATRIX								InterfaceWorld;

		ID3D11Buffer*							g_pWLoadingIndexBuffer = NULL;
		ID3D11Buffer*							g_pWLoadingVertexBuffer = NULL;
		ID3D11InputLayout*						g_pWLoadingVertexLayout = NULL;

		RADXTexture*							wLoadingTex;

	public:
		ID3DX11Effect*							InterfaceShaderEffect = NULL;
		ID3DX11EffectTechnique*					g_pTechniqueInterfaceShader = NULL;
		ID3DX11EffectShaderResourceVariable*	g_pTexResourceInterfaceShader = NULL;

		ID3DX11EffectScalarVariable*			lTex = NULL;
		ID3DX11EffectScalarVariable*			visibleSV = NULL;
		ID3DX11EffectVectorVariable*			colorTexture = NULL;

		std::vector<RADXIBlock*>				IBlockList;
};


#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class GridPlace
{
	struct SimpleVertex
	{
		D3DXVECTOR3 Pos;
	};

	public:
		GridPlace();
		~GridPlace();

		void Loading();
		void InitObjects();

		void Render();

		void UpdateGPSize(int value);

	private:
		UINT									size_SimpleVertex = sizeof(SimpleVertex);

	public:
		D3DXMATRIX								g_World;

		ID3D11Buffer*							g_pIndexBuffer = NULL;
		ID3D11Buffer*							g_pVertexBuffer = NULL;
		ID3D11InputLayout*						g_pVertexLayout = NULL;
		ID3DX11EffectTechnique*					g_pTechnique = NULL;

		ID3DX11Effect*							g_pEffect = NULL;
		ID3DX11EffectVectorVariable*			g_pColor = NULL;
		ID3DX11EffectMatrixVariable*			g_pWorldVariable = NULL;
		ID3DX11EffectMatrixVariable*			g_pViewVariable = NULL;
		ID3DX11EffectMatrixVariable*			g_pProjectionVariable = NULL;
};


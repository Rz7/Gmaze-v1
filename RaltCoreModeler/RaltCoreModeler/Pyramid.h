#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class DXRaltCoreApp;
class Pyramid
{
	struct SimpleVertex
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR2 Tex;
		D3DXVECTOR3 Normal;
	};

	public:
		Pyramid();
		~Pyramid();

		HRESULT LoadEM();
		HRESULT InitObjects();

		void Render();

		void Clean();

		void SetSDV(D3DXVECTOR3 n_sdv)
		{
			sDV = n_sdv;
		}

	private:
		UINT									size_SimpleVertex = sizeof(SimpleVertex);
		UINT height;
		UINT width;

		D3DXVECTOR3 sDV; // координаты сдвига всех координатных точек

	public:
		BOOL visible;

		DWORD* indices;
		SimpleVertex* vertices;

		D3DXMATRIX								g_World;

		ID3D11Buffer*							g_pIndexBuffer = NULL;
		ID3D11Buffer*							g_pVertexBuffer = NULL;
		ID3D11InputLayout*						g_pVertexLayout = NULL;
};


#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class DXRaltCoreApp;
class Sphere
{
	struct SimpleVertex
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR2 Tex;
		D3DXVECTOR3 Normal;
	};

	public:
		Sphere();
		~Sphere();

		HRESULT LoadEM();
		HRESULT InitObjects(FLOAT radius, FLOAT slices, FLOAT stacks);

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

		UINT indxCount;

		float c;
		D3DXVECTOR3 sDV; // ���������� ������ ���� ������������ �����

	public:
		BOOL visible;

		DWORD* indices;
		SimpleVertex* vertices;

		INT										vertexCount;	// ���������� �����
		INT 									indexCount;		// ���������� �������� �����

		D3DXMATRIX								g_World;

		ID3D11Buffer*							g_pIndexBuffer = NULL;
		ID3D11Buffer*							g_pVertexBuffer = NULL;
		ID3D11InputLayout*						g_pVertexLayout = NULL;
};
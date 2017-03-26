#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class Cube;
class Sphere;
class Pyramid;
class EPolygon;

class RADXModel
{
	struct ModelVertex
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR2 Tex;
		D3DXVECTOR3 Normal;
	};

	public:
		RADXModel()
		{
			loadDone = false;
		};

		~RADXModel() {};

		/*
			����� ������ ������ � �������.
			��������, ����������, �������� ������.
		*/
		
		void InitStInfo(string name);

		void LoadModelFromFile();
		void SaveModelToFile();

		void CopyCubeToModel(Cube* cb);
		void CopySphereToModel(Sphere* sp);
		void CopyPyramidToModel(Pyramid* sp);
		void CopyEPolygonToModel(EPolygon* sp);

		void SetShader();
		void SetBuffers();
		void SetVertexBuffer();
		void SetIndexBuffer();
		void SetLayout();

		void Render();

		void ComputeAllNormal();

	private:
		int idModel;		// ����� ������
		string modelName;	// ��� ������
		string fileName;	// ��� �����, � ������� ���������� ������

		ID3D11Buffer*				g_pIndexBuffer = NULL;
		ID3D11Buffer*				g_pVertexBuffer = NULL;
		ID3D11InputLayout*			g_pVertexLayout = NULL;

		D3DXMATRIX					g_World;

		float						c = 0.0f;	// �������
		float						rgb[3];		// ���� �������
	
	public:
		BOOL						loadDone;		// ��������� �� �������� (��� �������)

		int							mVcount;		// ���������� ModelVertex
		int							mIndcount;		// ���������� mIndices

		ModelVertex*				mVertex;		// ������ ������������ �����
		DWORD*						mIndices;		// ������ �������� �����

		

};


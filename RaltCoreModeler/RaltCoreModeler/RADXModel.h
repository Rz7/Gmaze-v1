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
			Класс полной работы с моделей.
			Загрузка, сохранение, создание модели.
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
		int idModel;		// Номер модели
		string modelName;	// Имя модели
		string fileName;	// Имя файла, в котором содержится модель

		ID3D11Buffer*				g_pIndexBuffer = NULL;
		ID3D11Buffer*				g_pVertexBuffer = NULL;
		ID3D11InputLayout*			g_pVertexLayout = NULL;

		D3DXMATRIX					g_World;

		float						c = 0.0f;	// счетчик
		float						rgb[3];		// Цвет объекта
	
	public:
		BOOL						loadDone;		// Завершена ли загрузка (для рендера)

		int							mVcount;		// Количество ModelVertex
		int							mIndcount;		// Количество mIndices

		ModelVertex*				mVertex;		// Массив координатных точек
		DWORD*						mIndices;		// Массив индексов точек

		

};


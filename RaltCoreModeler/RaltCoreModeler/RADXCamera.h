#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class RADXCamera
{
	public:
		RADXCamera();
		~RADXCamera() {};

		void InitCamera();

		void MatrixLook();
		void OnResize(UINT width, UINT height);
		void SetProjectionMatrix(UINT width, UINT height);

		void SetStartPosition();

		void ZoomingCamera(FLOAT value);
		void RotateCamera(BOOL rBMClick, FLOAT dy, FLOAT dx);

	public:
		D3DXMATRIX			g_View;
		D3DXMATRIX			g_Projection;

		D3DXVECTOR3			mPos;		// вектор расположения камеры
		D3DXVECTOR3			mRight;		// правый вектор камеры
		D3DXVECTOR3			mTarget;	// вектор направления взгляда камеры
		D3DXVECTOR3			mUp;		// верхний вектор камеры

		FLOAT				angleR, angleU;
		FLOAT				near_plane; // Видимость объектов; ближняя грань
		FLOAT				fart_plane; // Дальний конец
};


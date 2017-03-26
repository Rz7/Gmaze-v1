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

		D3DXVECTOR3			mPos;		// ������ ������������ ������
		D3DXVECTOR3			mRight;		// ������ ������ ������
		D3DXVECTOR3			mTarget;	// ������ ����������� ������� ������
		D3DXVECTOR3			mUp;		// ������� ������ ������

		FLOAT				angleR, angleU;
		FLOAT				near_plane; // ��������� ��������; ������� �����
		FLOAT				fart_plane; // ������� �����
};


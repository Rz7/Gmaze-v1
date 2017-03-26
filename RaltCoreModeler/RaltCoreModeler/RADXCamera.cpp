#include "RADXCamera.h"
#include <DirectXMath.h>

RADXCamera::RADXCamera()
{
	InitCamera();
}

void RADXCamera::InitCamera()
{
	angleR = 0.0f;
	angleU = 0.0f;

	near_plane = 0.1f;
	fart_plane = 500.0f;

	SetStartPosition();
}

void RADXCamera::MatrixLook()
{
	D3DXVec3Normalize(&mTarget, &mTarget);
	D3DXVec3Cross(&mUp, &mTarget, &mRight);
	D3DXVec3Normalize(&mUp, &mUp);

	D3DXVec3Cross(&mRight, &mUp, &mTarget);
	D3DXVec3Normalize(&mRight, &mRight);

	float x = -D3DXVec3Dot(&mRight, &mPos);
	float y = -D3DXVec3Dot(&mUp, &mPos);
	float z = -D3DXVec3Dot(&mTarget, &mPos);

	g_View(0, 0) = mRight.x; g_View(0, 1) = mUp.x; g_View(0, 2) = mTarget.x; g_View(0, 3) = 0.0f;
	g_View(1, 0) = mRight.y; g_View(1, 1) = mUp.y; g_View(1, 2) = mTarget.y; g_View(1, 3) = 0.0f;
	g_View(2, 0) = mRight.z; g_View(2, 1) = mUp.z; g_View(2, 2) = mTarget.z; g_View(2, 3) = 0.0f;
	g_View(3, 0) = x;        g_View(3, 1) = y;     g_View(3, 2) = z;	     g_View(3, 3) = 1.0f;
}

void RADXCamera::SetProjectionMatrix(UINT w, UINT h)
{
	// Инициализируем матрицу проекции
	D3DXMatrixPerspectiveFovLH(&g_Projection, (float)D3DX_PI * 0.25f, (float)w / (float)h, near_plane, fart_plane);
}

void RADXCamera::SetStartPosition()
{
	mPos = D3DXVECTOR3(-15.0f, 1.0f, -15.0f);
	mRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mTarget = D3DXVECTOR3(1.0f, 0.0f, 1.0f);

	MatrixLook();
}

void RADXCamera::ZoomingCamera(FLOAT value)
{
	FLOAT distance_by_center = Distance(mPos, D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	if (value > 0 && distance_by_center < 10.0f)
		return;

	if (value < 0 && distance_by_center > 100.0f)
		return;

	//distance_by_center *= 0.3f;
	//DRCA()->GetGridPlace()->UpdateGPSize(distance_by_center);

	mPos += 0.02f * mTarget * value;
	MatrixLook();
}

void RADXCamera::RotateCamera(BOOL rBMClick, FLOAT dy, FLOAT dx)
{
	if (!rBMClick)
		return;

	D3DXMATRIX axis1, axis2;
	FLOAT d_dy = dy > 0 ? dy : -dy;
	FLOAT d_dx = dx > 0 ? dx : -dx;

	if (angleR + dy < 90.0f && angleR + dy > -90.0f)
	{
		D3DXMatrixRotationAxis(&axis1, &mRight, (DOUBLE)D3DXToRadian(dy));
		D3DXVec3TransformNormal(&mPos, &mPos, &axis1);
		D3DXVec3TransformNormal(&mRight, &mRight, &axis1);
		D3DXVec3TransformNormal(&mTarget, &mTarget, &axis1);
		angleR += dy;
	}

	D3DXMatrixRotationAxis(&axis2, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), (DOUBLE)D3DXToRadian(dx));
	D3DXVec3TransformNormal(&mPos, &mPos, &axis2);
	D3DXVec3TransformNormal(&mRight, &mRight, &axis2);
	D3DXVec3TransformNormal(&mTarget, &mTarget, &axis2);
	angleU += dx;

	MatrixLook();
}

void RADXCamera::OnResize(UINT width, UINT height)
{
	SetProjectionMatrix(width, height);

	// Установить размер сетки
	DRCA()->GetGridPlace()->UpdateGPSize(3.0f);
}

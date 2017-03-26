#include "RADXGeometry.h"

D3DXVECTOR3 RADXGeometry::GetVectorCampos_t()
{
	// Функция расчитывает точку перед камерой

	FLOAT centerposx = DRCA()->GetWidth() / 2;
	FLOAT centerposy = DRCA()->GetHeight() / 2;

	FLOAT d_mypos = (DRCA()->GetRaltID()->GetMouseYPos() - centerposy) / centerposy;
	FLOAT d_mxpos = (DRCA()->GetRaltID()->GetMouseXPos() - centerposx) / centerposx;

	FLOAT r_rddx = 0.65f * d_mxpos;
	FLOAT r_rddy = 0.40f * d_mypos * -1.0f;

	D3DXVECTOR3 campos_t = DRCA()->GetCamera()->mPos;
	campos_t += DRCA()->GetCamera()->mTarget * 1.0f;
	campos_t += DRCA()->GetCamera()->mUp * r_rddy;
	campos_t += DRCA()->GetCamera()->mRight * r_rddx;
	return campos_t;
}

D3DXVECTOR3 RADXGeometry::GetVectorCameraLook()
{
	// Функция предназначена для создания вектора направления взгляда камеры с учетом положения мыши
	// Результат функции записывается в mTarget и возвращается

	D3DXVECTOR3 mTarget;
	D3DXVec3Subtract(&mTarget, &GetVectorCampos_t(), &DRCA()->GetCamera()->mPos);
	D3DXVec3Normalize(&mTarget, &mTarget);

	return mTarget;
}

D3DXVECTOR3 RADXGeometry::GetSelPointInGridPlace()
{
	// Функция вычисляет координаты x и z на GridPlace, на которые указывает курсор мыши
	// При этом: y = 0
	// Возвращает значение, равное координатам точки

	D3DXPLANE polygon;
	D3DXVECTOR3 point[3];
	point[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	point[1] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	point[2] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXPlaneFromPoints(&polygon, &point[0], &point[1], &point[2]);

	D3DXVECTOR3 f_vector; // Точка пересечения плоскости polygon и отрезка (s_point, e_point)
	D3DXPlaneIntersectLine(&f_vector, &polygon, &DRCA()->GetCamera()->mPos, &GetVectorCampos_t());

	return f_vector;
}

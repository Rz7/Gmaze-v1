#include "RADXMath.h"

bool RADXMath::PointInTriangle(D3DXVECTOR3 pointsTriangle[], D3DXVECTOR3 point)
{
	// Функция возвращает true, если вектор point проходит через треугольник, состоящий
	// из 3-х точек pointsTriangle. Иначе - false.

	D3DXVECTOR3 B, C, P;
	D3DXVec3Subtract(&B, &pointsTriangle[1], &pointsTriangle[0]);
	D3DXVec3Subtract(&C, &pointsTriangle[2], &pointsTriangle[0]);
	D3DXVec3Subtract(&P, &point, &pointsTriangle[0]);

	FLOAT m = (P.x * B.y - B.x * P.y) / (C.x * B.y - B.x * C.y);
	if (m >= 0.0f && m <= 1.0f)
	{
		FLOAT l = (P.x - m * C.x) / B.x;
		if (l >= 0.0f && m + l <= 1.0f)
			return true;
	}
	return false;
}
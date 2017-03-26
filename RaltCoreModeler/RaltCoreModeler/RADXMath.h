#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class RADXMath
{
	public:
		RADXMath() {};
		~RADXMath() {};

		// ќпределение прохождени€ вектора через плоскость
		bool PointInTriangle(D3DXVECTOR3 pointsTriangle[], D3DXVECTOR3 point);

		friend BOOL VectorZero(D3DXVECTOR3 vector)
		{
			return (vector == D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}

		friend BOOL VectorEquival(D3DXVECTOR3 m1, D3DXVECTOR3 m2)
		{
			return (m1.x == m2.x && m1.y == m2.y && m1.z == m2.z);
		}

		inline friend FLOAT Distance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2)
		{
			D3DXVECTOR3 vec12;
			return D3DXVec3Length(D3DXVec3Subtract(&vec12, &vec1, &vec2));
		}

		inline friend FLOAT DistanceY(D3DXVECTOR3 vec1_3D, D3DXVECTOR3 vec2_3D)
		{
			// ¬озвращает дистацию между двум€ векторами, игнориру€ Y-позицию.
			D3DXVECTOR2 vec12, vec1, vec2; vec1.x = vec1_3D.x; vec1.y = vec1_3D.z; vec2.x = vec2_3D.x; vec2.y = vec2_3D.z;
			return D3DXVec2Length(D3DXVec2Subtract(&vec12, &vec1, &vec2));
		}

		inline friend FLOAT Distance2D(D3DXVECTOR2 vec1, D3DXVECTOR2 vec2)
		{
			D3DXVECTOR2 vec12;
			return D3DXVec2Length(D3DXVec2Subtract(&vec12, &vec1, &vec2));
		}

		inline friend void ComputeNormal(const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, D3DXVECTOR3& out)
		{
			// –асчет вектора нормали
			D3DXVECTOR3 u = p1 - p0; D3DXVECTOR3 v = p2 - p0; D3DXVec3Cross(&out, &u, &v);
			D3DXVec3Normalize(&out, &out);
		}
};


#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class DRXCA;
class RADXGeometry
{
	/*
		����� ��� ��������� �������, ���������� � ����������.
	*/

	public:
		RADXGeometry() {};
		~RADXGeometry() {};

		D3DXVECTOR3 GetVectorCampos_t();
		D3DXVECTOR3 GetVectorCameraLook();
		
		
		D3DXVECTOR3 GetSelPointInGridPlace();
};


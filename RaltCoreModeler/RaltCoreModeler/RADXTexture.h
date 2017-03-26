#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class RADXTexture
{
	public:
		RADXTexture(string d_i);
		~RADXTexture()
		{
			SafeRelease(texture2D);
			SafeRelease(textureSRV);
		};

		void AddToImageList();


		HRESULT ImageLoad();

		ID3D11Texture2D* Get2D() { return texture2D; }
		ID3D11ShaderResourceView* GetSRV() { return textureSRV; }

	private:
		SIZE_T tSize = NULL;
		string dataURL;

		ID3D11Texture2D* texture2D = NULL;
		ID3D11ShaderResourceView* textureSRV = NULL;
};


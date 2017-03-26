#include "RADXTexture.h"

RADXTexture::RADXTexture(string d_i)
{
	dataURL = MainDir() + d_i;
}

void RADXTexture::AddToImageList()
{
	// TODO: add to image list
}

HRESULT RADXTexture::ImageLoad()
{
	HRESULT hr = S_OK;

	const char* ch_DATAURL = dataURL.c_str();

	D3DX11_IMAGE_INFO		InfoFromFile;
	D3DX11_IMAGE_LOAD_INFO	LoadImageInfo;

	ZeroMemory(&InfoFromFile, sizeof(InfoFromFile));
	ZeroMemory(&LoadImageInfo, sizeof(LoadImageInfo));

	D3DX11GetImageInfoFromFile(ch_DATAURL, NULL, &InfoFromFile, &hr);
	if (FAILED(hr))
		return hr;

	LoadImageInfo.Width = InfoFromFile.Width;
	LoadImageInfo.Height = InfoFromFile.Height;
	LoadImageInfo.Depth = InfoFromFile.Depth;
	LoadImageInfo.FirstMipLevel = 0;
	LoadImageInfo.MipLevels = 6;
	LoadImageInfo.Usage = D3D11_USAGE_DEFAULT;
	LoadImageInfo.BindFlags = D3DX11_DEFAULT;
	LoadImageInfo.CpuAccessFlags = D3DX11_DEFAULT;
	LoadImageInfo.MiscFlags = D3DX11_DEFAULT;
	LoadImageInfo.Format = InfoFromFile.Format;
	LoadImageInfo.Filter = D3DX11_FILTER_TRIANGLE;
	LoadImageInfo.MipFilter = D3DX11_FILTER_TRIANGLE;
	LoadImageInfo.pSrcInfo = &InfoFromFile;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.ArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Width = InfoFromFile.Width;
	texDesc.Height = InfoFromFile.Height;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.MiscFlags = 0;
	texDesc.CPUAccessFlags = 0;
	DRCA()->GetDevice()->CreateTexture2D(&texDesc, NULL, &texture2D);


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	hr = DRCA()->GetDevice()->CreateShaderResourceView(texture2D, &shaderResourceViewDesc, &textureSRV);
	if (FAILED(hr))
		return hr;

	D3DX11CreateShaderResourceViewFromFile(DRCA()->GetDevice(), ch_DATAURL, &LoadImageInfo, NULL, &textureSRV, &hr);
	return hr;
}

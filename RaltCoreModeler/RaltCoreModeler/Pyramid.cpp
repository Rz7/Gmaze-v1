#include "Pyramid.h"

Pyramid::Pyramid()
{
	sDV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

Pyramid::~Pyramid()
{
	Clean();
}

HRESULT Pyramid::LoadEM()
{
	HRESULT hr = S_OK;

	// Описываем формат входных данных
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Создаем объект входных данных
	D3DX11_PASS_DESC PassDesc;
	DRCA()->modelShaderTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	hr = DRCA()->GetDevice()->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout);
	if (FAILED(hr))
		return hr;

	hr = InitObjects();
	if (FAILED(hr))
		return hr;

	// Инициализируем мировую матрицу
	D3DXMatrixIdentity(&g_World);
	return S_OK;
}

HRESULT Pyramid::InitObjects()
{
	HRESULT hr = S_OK;

	vertices = new SimpleVertex[5];
	vertices[0] = { D3DXVECTOR3(0.0f, 2.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f) };	// 0 - вершина
	vertices[1] = { D3DXVECTOR3(-1.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f) };
	vertices[2] = { D3DXVECTOR3(1.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f) };
	vertices[3] = { D3DXVECTOR3(1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f) };
	vertices[4] = { D3DXVECTOR3(-1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f) };

	for (int i = 0; i < 5; ++i)
		vertices[i].Pos += sDV;

	indices = new DWORD[18];
	indices[0] = 4;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 4;
	indices[4] = 2;
	indices[5] = 3;
	indices[6] = 0;
	indices[7] = 2;
	indices[8] = 1;
	indices[9] = 0;
	indices[10] = 3;
	indices[11] = 2;
	indices[12] = 0;
	indices[13] = 4;
	indices[14] = 3;
	indices[15] = 0;
	indices[16] = 1;
	indices[17] = 4;

	for (int i = 0; i < 18; i += 3)
	{
		D3DXVECTOR3 resultNormal;
		ComputeNormal(vertices[indices[i]].Pos, vertices[indices[i + 1]].Pos, vertices[indices[i + 2]].Pos, resultNormal);
		for (int k = 0; k < 3; ++k)
		{
			if (VectorZero(vertices[indices[i + k]].Normal))
				vertices[indices[i + k]].Normal = resultNormal;
			else
				vertices[indices[i + k]].Normal = (vertices[indices[i + k]].Normal + resultNormal) / 2;
		}
	}

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 5;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	hr = DRCA()->GetDevice()->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);

	// Создаем индексный буфер
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DWORD) * 18;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = indices;
	hr = DRCA()->GetDevice()->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;
	return S_OK;
}

void Pyramid::Render()
{
	// Передаем информацию о матрицах преобразования
	DRCA()->modelShaderWorldVariable->SetMatrix((float*)&g_World);
	DRCA()->modelShaderViewVariable->SetMatrix((float*)&DRCA()->GetCamera()->g_View);
	DRCA()->modelShaderProjectionVariable->SetMatrix((float*)&DRCA()->GetCamera()->g_Projection);

	// Цвет самого куба
	float CubeColor[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
	DRCA()->modelShaderColor->SetFloatVector((float*)&CubeColor);

	// Передаем информацию об источнике света
	float LightColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	DRCA()->modelShaderLightColor->SetFloatVector((float*)&LightColor);

	float LightDir[4] = { 0.0f, 0.8f, -1.5f, 1.0f };
	DRCA()->modelShaderLightPosition->SetFloatVector((float*)&LightDir);

	// Рисуем куб и новую геометрию
	D3DX11_TECHNIQUE_DESC techDesc;
	DRCA()->modelShaderTechnique->GetDesc(&techDesc);

	UINT stride = size_SimpleVertex;
	UINT offset = 0;

	// Связываем объект входных данных с графическим конвейером
	DRCA()->GetDeviceContext()->IASetInputLayout(g_pVertexLayout);
	DRCA()->GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	DRCA()->GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		DRCA()->modelShaderTechnique->GetPassByIndex(p)->Apply(0, DRCA()->GetDeviceContext());
		DRCA()->GetDeviceContext()->DrawIndexed(18, 0, 0);
	}
}

void Pyramid::Clean()
{
	SafeRelease(g_pIndexBuffer);
	SafeRelease(g_pVertexBuffer);
	SafeRelease(g_pVertexLayout);
}

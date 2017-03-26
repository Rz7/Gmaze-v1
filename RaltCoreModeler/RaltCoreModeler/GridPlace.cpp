#include "GridPlace.h"

GridPlace::GridPlace()
{
	Loading();
}

GridPlace::~GridPlace()
{
	SafeRelease(g_pIndexBuffer);
	SafeRelease(g_pVertexBuffer);
	SafeRelease(g_pVertexLayout);
	SafeRelease(g_pTechnique);

	SafeRelease(g_pEffect);
	SafeRelease(g_pColor);
	SafeRelease(g_pWorldVariable);
	SafeRelease(g_pViewVariable);
	SafeRelease(g_pProjectionVariable);
}

void GridPlace::Loading()
{
	HRESULT hr = S_OK;

	ID3D10Blob*	g_pEffectBlob = 0;

	string shader_data = MainDir() + "/Shaders/gridplace.fx";
	hr = D3DX11CompileFromFile(shader_data.c_str(), NULL, NULL, "VS", "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS, NULL, NULL, &g_pEffectBlob, NULL, NULL);
	if (FAILED(hr))
		return;

	hr = D3DX11CreateEffectFromMemory(g_pEffectBlob->GetBufferPointer(), g_pEffectBlob->GetBufferSize(), 0, DRCA()->GetDevice(), &g_pEffect);
	if (FAILED(hr))
		return;

	SafeRelease(g_pEffectBlob);

	// Связь с переменными шейдера
	g_pTechnique = g_pEffect->GetTechniqueByName("RenderTexture");

	g_pColor = g_pEffect->GetVariableByName("Color")->AsVector();
	g_pWorldVariable = g_pEffect->GetVariableByName("World")->AsMatrix();
	g_pViewVariable = g_pEffect->GetVariableByName("View")->AsMatrix();
	g_pProjectionVariable = g_pEffect->GetVariableByName("Projection")->AsMatrix();

	// Описываем формат входных данных
	D3D11_INPUT_ELEMENT_DESC layout[] = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Создаем объект входных данных
	D3DX11_PASS_DESC PassDesc;
	g_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	hr = DRCA()->GetDevice()->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout);
	if (FAILED(hr))
		return;

	InitObjects();
	// Инициализируем мировую матрицу
	D3DXMatrixIdentity(&g_World);
	return;
}

void GridPlace::InitObjects()
{
	HRESULT hr = S_OK;

	SimpleVertex vertices[] =
	{
		{ D3DXVECTOR3(-1.0f, 0.0f, -1.0f) },
		{ D3DXVECTOR3(1.0f, 0.0f, -1.0f) },
		{ D3DXVECTOR3(1.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(-1.0f, 0.0f, 1.0f) },
	};

	DWORD indices[] =
	{
		3, 1, 0,
		2, 1, 3
	};

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	hr = DRCA()->GetDevice()->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);

	// Создаем индексный буфер
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DWORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = indices;
	hr = DRCA()->GetDevice()->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
		return;
}

void GridPlace::Render()
{
	DRCA()->SetRSState(1);

	g_pWorldVariable->SetMatrix((float*)&g_World);
	g_pViewVariable->SetMatrix((float*)&DRCA()->GetCamera()->g_View);
	g_pProjectionVariable->SetMatrix((float*)&DRCA()->GetCamera()->g_Projection);

	// Цвет самого куба
	float CubeColor[4] = { 0.84534f, 0.72265f, 0.75432f, 1.0f };
	g_pColor->SetFloatVector((float*)&CubeColor);

	// Рисуем куб и новую геометрию
	D3DX11_TECHNIQUE_DESC techDesc;
	g_pTechnique->GetDesc(&techDesc);

	UINT stride = size_SimpleVertex;
	UINT offset = 0;

	// Связываем объект входных данных с графическим конвейером
	DRCA()->GetDeviceContext()->IASetInputLayout(g_pVertexLayout);
	DRCA()->GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	DRCA()->GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		g_pTechnique->GetPassByIndex(p)->Apply(0, DRCA()->GetDeviceContext());
		DRCA()->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}

	DRCA()->SetRSState(0);
}

void GridPlace::UpdateGPSize(int value)
{
	SimpleVertex vertices[] =
	{
		{ D3DXVECTOR3(-1.0f, 0.0f, -1.0f) },
		{ D3DXVECTOR3(1.0f, 0.0f, -1.0f) },
		{ D3DXVECTOR3(1.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(-1.0f, 0.0f, 1.0f) },
	};

	for (int i = 0; i < 4; ++i)
	{
		vertices[i].Pos.x *= value;
		vertices[i].Pos.z *= value;
	}	

	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = DRCA()->GetDeviceContext()->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	if (FAILED(hr))
		return;

	memcpy(resource.pData, vertices, sizeof(vertices));
	DRCA()->GetDeviceContext()->Unmap(g_pVertexBuffer, 0);
}

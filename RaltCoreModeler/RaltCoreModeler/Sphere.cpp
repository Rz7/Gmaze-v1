#include "Sphere.h"


Sphere::Sphere()
{
	sDV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

Sphere::~Sphere()
{
	Clean();
}

HRESULT Sphere::LoadEM()
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

	FLOAT slices = 50.0f;	// Количество колец
	FLOAT stacks = 50.0f;	// Одно кольцо
	FLOAT radius = 2.0f;	// Радиус сферы

	hr = InitObjects(radius, slices, stacks);
	if (FAILED(hr))
		return hr;

	// Инициализируем мировую матрицу
	D3DXMatrixIdentity(&g_World);
	return S_OK;
}

HRESULT Sphere::InitObjects(FLOAT radius, FLOAT slices, FLOAT stacks)
{
	HRESULT hr = S_OK;

	FLOAT stackStep = D3DX_PI / stacks;
	FLOAT sliceStep = 2 * D3DX_PI / slices;
	FLOAT sdvY = 0.0f;	//  Сдвиг по Y

	vertexCount = slices * (stacks - 1) + 2;
	
	INT primitiveCount = 1 + slices * (stacks - 1) * 2;
	indexCount = primitiveCount * 3;

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SimpleVertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	hr = DRCA()->GetDevice()->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

	// Создаем индексный буфер
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(DWORD) * indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	hr = DRCA()->GetDevice()->CreateBuffer(&bd, NULL, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	D3D11_MAPPED_SUBRESOURCE resourceV, resourceI;
	hr = DRCA()->GetDeviceContext()->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resourceV);
	hr = DRCA()->GetDeviceContext()->Map(g_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resourceI);
	
	vertices = (SimpleVertex*)resourceV.pData;
	indices = (DWORD*)resourceI.pData;

	INT vCounter = 0; // Счетчик vertex'ов
	INT iCounter = 0; // Счетчик index'ов
	
	FLOAT stackAngle = stackStep;

	vertices[vCounter++].Pos = D3DXVECTOR3(0.0f, 2 * radius + sdvY, 0.0f);
	for (int i = 0; i < stacks - 1; ++i)
	{
		FLOAT sliceAngle = 0;
		for (int j = 0; j < slices; ++j)
		{
			// Просчет координатных точек
			FLOAT x = FLOAT(radius * sinf(stackAngle) * cosf(sliceAngle));
			FLOAT y = radius + FLOAT(radius * cosf(stackAngle));
			FLOAT z = FLOAT(radius * sinf(stackAngle) * sinf(sliceAngle));
			
			vertices[vCounter].Pos = D3DXVECTOR3(x, y + sdvY, z);
			vertices[vCounter].Normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			// Просчет индексов
			if (j > 0)
			{
				if (i == 0)
				{
					indices[iCounter++] = vCounter;
					indices[iCounter++] = vCounter - 1;
					indices[iCounter++] = 0;

					D3DXVECTOR3 resultNormal; ComputeNormal(vertices[vCounter].Pos, vertices[vCounter - 1].Pos, vertices[0].Pos, resultNormal);

					for (int k = 0; k < 3; ++k)
					{
						if (VectorZero(vertices[indices[iCounter - k]].Normal))
							vertices[indices[iCounter - k]].Normal = resultNormal;
						else vertices[indices[iCounter - k]].Normal = (vertices[indices[iCounter - k]].Normal + resultNormal) / 2;
					}

					if (j + 1 == slices)
					{
						indices[iCounter++] = vCounter - (slices - 1);
						indices[iCounter++] = vCounter;
						indices[iCounter++] = 0;

						D3DXVECTOR3 resultNormalJ; ComputeNormal(vertices[indices[iCounter - 3]].Pos, vertices[indices[iCounter - 2]].Pos, vertices[indices[iCounter - 1]].Pos, resultNormalJ);
						for (int k = 0; k < 3; ++k)
						{
							if (VectorZero(vertices[indices[iCounter - k]].Normal))
								vertices[indices[iCounter - k]].Normal = resultNormalJ;
							else vertices[indices[iCounter - k]].Normal = (vertices[indices[iCounter - k]].Normal + resultNormalJ) / 2;
						}
					}
				}
				else
				{
					indices[iCounter++] = vCounter;
					indices[iCounter++] = vCounter - 1;
					indices[iCounter++] = vCounter - slices - 1;

					indices[iCounter++] = vCounter;
					indices[iCounter++] = vCounter - slices - 1;
					indices[iCounter++] = vCounter - slices;
					
					D3DXVECTOR3 resultNormal; ComputeNormal(vertices[indices[iCounter - 6]].Pos, vertices[indices[iCounter - 5]].Pos, vertices[indices[iCounter - 4]].Pos, resultNormal);
					for (int k = 4; k < 7; ++k)
					{
						if (VectorZero(vertices[indices[iCounter - k]].Normal))
							vertices[indices[iCounter - k]].Normal = resultNormal;
						else vertices[indices[iCounter - k]].Normal = (vertices[indices[iCounter - k]].Normal + resultNormal) / 2;
					}

					ComputeNormal(vertices[indices[iCounter - 3]].Pos, vertices[indices[iCounter - 2]].Pos, vertices[indices[iCounter - 1]].Pos, resultNormal);
					for (int k = 1; k < 4; ++k)
					{
						if (VectorZero(vertices[indices[iCounter - k]].Normal))
							vertices[indices[iCounter - k]].Normal = resultNormal;
						else vertices[indices[iCounter - k]].Normal = (vertices[indices[iCounter - k]].Normal + resultNormal) / 2;
					}

					if (j + 1 == slices)
					{
						
						indices[iCounter++] = vCounter - (slices - 1);
						indices[iCounter++] = vCounter;
						indices[iCounter++] = vCounter - slices;
						
						indices[iCounter++] = vCounter - (slices - 1);
						indices[iCounter++] = vCounter - slices;
						indices[iCounter++] = vCounter - slices - (slices - 1);

						D3DXVECTOR3 resultNormalJ; ComputeNormal(vertices[indices[iCounter - 6]].Pos, vertices[indices[iCounter - 5]].Pos, vertices[indices[iCounter - 4]].Pos, resultNormal);
						for (int k = 4; k < 7; ++k)
						{
							if (VectorZero(vertices[indices[iCounter - k]].Normal))
								vertices[indices[iCounter - k]].Normal = resultNormalJ;
							else vertices[indices[iCounter - k]].Normal = (vertices[indices[iCounter - k]].Normal + resultNormalJ) / 2;
						}

						ComputeNormal(vertices[indices[iCounter - 3]].Pos, vertices[indices[iCounter - 2]].Pos, vertices[indices[iCounter - 1]].Pos, resultNormal);
						for (int k = 1; k < 4; ++k)
						{
							if (VectorZero(vertices[indices[iCounter - k]].Normal))
								vertices[indices[iCounter - k]].Normal = resultNormalJ;
							else vertices[indices[iCounter - k]].Normal = (vertices[indices[iCounter - k]].Normal + resultNormalJ) / 2;
						}
					}
				}
			}

			++vCounter;
			sliceAngle += sliceStep;
		}

		//system("pause");

		stackAngle += stackStep;
	}
	vertices[vCounter++].Pos = D3DXVECTOR3(0.0f, sdvY, 0.0f);

	for (int i = 0; i < vCounter; ++i)
		vertices[i].Pos += sDV;

	for (int j = 1; j < slices; ++j)
	{
		indices[iCounter++] = vCounter - 1;
		indices[iCounter++] = vCounter - j - 2;
		indices[iCounter++] = vCounter - j - 1;

		D3DXVECTOR3 resultNormal; ComputeNormal(vertices[indices[iCounter - 3]].Pos, vertices[indices[iCounter - 2]].Pos, vertices[indices[iCounter - 1]].Pos, resultNormal);
		for (int k = 1; k < 4; ++k)
		{
			if (VectorZero(vertices[indices[iCounter - k]].Normal))
				vertices[indices[iCounter - k]].Normal = resultNormal;
			else vertices[indices[iCounter - k]].Normal = (vertices[indices[iCounter - k]].Normal + resultNormal) / 2;
		}
	}

	indices[iCounter++] = vCounter - 1;
	indices[iCounter++] = vCounter - 2;
	indices[iCounter++] = vCounter - slices - 1;
	
	D3DXVECTOR3 resultNormal; ComputeNormal(vertices[indices[iCounter - 3]].Pos, vertices[indices[iCounter - 2]].Pos, vertices[indices[iCounter - 1]].Pos, resultNormal);
	for (int k = 1; k < 4; ++k)
	{
		if (VectorZero(vertices[indices[iCounter - k]].Normal))
			vertices[indices[iCounter - k]].Normal = resultNormal;
		else vertices[indices[iCounter - k]].Normal = (vertices[indices[iCounter - k]].Normal + resultNormal) / 2;
	}

	indxCount = iCounter; // Количество индексов (для прорисовки)

	DRCA()->GetDeviceContext()->Unmap(g_pVertexBuffer, 0);
	DRCA()->GetDeviceContext()->Unmap(g_pIndexBuffer, 0);

	return S_OK;
}

void Sphere::Render()
{
	if (!DRCA()->GetRaltID()->GetrBMClck())
	{
		//c += D3DX_PI * 0.005f;
		//D3DXMatrixRotationY(&g_World, c);
	}

	// Передаем информацию о матрицах преобразования
	DRCA()->modelShaderWorldVariable->SetMatrix((float*)&g_World);
	DRCA()->modelShaderViewVariable->SetMatrix((float*)&DRCA()->GetCamera()->g_View);
	DRCA()->modelShaderProjectionVariable->SetMatrix((float*)&DRCA()->GetCamera()->g_Projection);

	// Цвет самого куба
	float CubeColor[4] = { 1.0f, 0.502f, 0.0f, 1.0f };
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
		DRCA()->GetDeviceContext()->DrawIndexed(indxCount, 0, 0);
	}

	DRCA()->SetRSState(0);
}

void Sphere::Clean()
{
	SafeRelease(g_pIndexBuffer);
	SafeRelease(g_pVertexBuffer);
	SafeRelease(g_pVertexLayout);
}

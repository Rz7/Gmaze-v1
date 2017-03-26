#include "RADXModel.h"

void RADXModel::InitStInfo(string n)
{
	modelName = n;
	fileName = MainDir() + "\\Models\\" + modelName + ".ramod";

	mVcount = 0;
	mIndcount = 0;

	rgb[0] = 1.0f;
	rgb[1] = 0.522115f;
	rgb[2] = 0.0f;

	// ����������� � ����������� ���������
	SetLayout();

	DRCA()->ModelList.push_back(this);

	D3DXMatrixIdentity(&g_World);
}

void RADXModel::LoadModelFromFile()
{
	ifstream out(fileName, std::fstream::binary);

	// ��������� id ������
	out.read((char*)&idModel, sizeof(idModel));

	// ��������� ��� ������
	modelName = DRCA()->LoadStringFromStream(out);

	// ��������� ����� ���������� �����
	out.read((char*)&mVcount, sizeof(mVcount));

	// �������������� ������ �����
	mVertex = new ModelVertex[mVcount];

	// ��������� ������� �����
	for (int i = 0; i < mVcount; ++i)
	{
		out.read((char*)&mVertex[i].Pos.x, sizeof(mVertex[i].Pos.x));
		out.read((char*)&mVertex[i].Pos.y, sizeof(mVertex[i].Pos.y));
		out.read((char*)&mVertex[i].Pos.z, sizeof(mVertex[i].Pos.z));

		out.read((char*)&mVertex[i].Tex.x, sizeof(mVertex[i].Tex.x));
		out.read((char*)&mVertex[i].Tex.y, sizeof(mVertex[i].Tex.y));
	}

	// ��������� ����� ���������� �������� �����
	out.read((char*)&mIndcount, sizeof(mIndcount));

	// �������������� ������ �������� �����
	mIndices = new DWORD[mIndcount];

	// ��������� ������� �����
	for (int i = 0; i < mIndcount; ++i)
		out.read((char*)&mIndices[i], sizeof(mIndices[i]));

	out.close();

	ComputeAllNormal();
	SetBuffers();
}

void RADXModel::SaveModelToFile()
{
	ofstream out(fileName, std::fstream::binary);
	out.clear();

	// ��������� id ������
	out.write((char*)&idModel, sizeof(idModel));

	// ��������� ��� ������
	DRCA()->SaveStringToStream(out, modelName);

	// ��������� ����� ���������� �����
	out.write((char*)&mVcount, sizeof(mVcount));

	// ��������� ������� �����
	for (int i = 0; i < mVcount; ++i)
	{
		out.write((char*)&mVertex[i].Pos.x, sizeof(mVertex[i].Pos.x));
		out.write((char*)&mVertex[i].Pos.y, sizeof(mVertex[i].Pos.y));
		out.write((char*)&mVertex[i].Pos.z, sizeof(mVertex[i].Pos.z));

		out.write((char*)&mVertex[i].Tex.x, sizeof(mVertex[i].Tex.x));
		out.write((char*)&mVertex[i].Tex.y, sizeof(mVertex[i].Tex.y));
	}

	// ��������� ����� ���������� �������� �����
	out.write((char*)&mIndcount, sizeof(mIndcount));

	// ��������� ������� �����
	for (int i = 0; i < mIndcount; ++i)
		out.write((char*)&mIndices[i], sizeof(mIndices[i]));

	out.close();
}

void RADXModel::ComputeAllNormal()
{
	for (int i = 0; i < mIndcount; i += 3)
	{
		D3DXVECTOR3 resultNormal; ComputeNormal(mVertex[mIndices[i]].Pos, mVertex[mIndices[i + 1]].Pos, mVertex[mIndices[i + 2]].Pos, resultNormal);
		
		for (int k = 0; k < 3; ++k)
		{
			if (VectorZero(mVertex[mIndices[i + k]].Normal))
				mVertex[mIndices[i + k]].Normal = resultNormal;
			else
			{
				mVertex[mIndices[i + k]].Normal = (mVertex[mIndices[i + k]].Normal + resultNormal) / 2;
				D3DXVec3Normalize(&mVertex[mIndices[i + k]].Normal, &mVertex[mIndices[i + k]].Normal);
			}
		}
	}
}

void RADXModel::CopyCubeToModel(Cube* cb)
{
	mVcount = 8;
	mVertex = new ModelVertex[mVcount];

	for (int i = 0; i < mVcount; ++i)
	{
		mVertex[i].Pos = cb->vertices[i].Pos;
		mVertex[i].Tex = cb->vertices[i].Tex;
		mVertex[i].Normal = cb->vertices[i].Normal;
	}

	mIndcount = 36;
	mIndices = new DWORD[mIndcount];

	for (int i = 0; i < mIndcount; ++i)
		mIndices[i] = cb->indices[i];

	// ������ ��� ��������� (����� ����� � ������� ������)
	//cb->visible = false;

	g_World = cb->g_World;

	SetBuffers();
}

void RADXModel::CopySphereToModel(Sphere* sp)
{
	mVcount = sp->vertexCount;
	mVertex = new ModelVertex[mVcount];

	for (int i = 0; i < mVcount; ++i)
	{
		mVertex[i].Pos = sp->vertices[i].Pos;
		mVertex[i].Tex = sp->vertices[i].Tex;
		mVertex[i].Normal = sp->vertices[i].Normal;
	}

	for (int i = 0; i < mVcount; ++i)
	{
		mVertex[i].Pos.x += 5.0f;
		mVertex[i].Pos.z += -5.0f;
	}

	mIndcount = sp->indexCount;
	mIndices = new DWORD[mIndcount];

	for (int i = 0; i < mIndcount; ++i)
		mIndices[i] = sp->indices[i];

	g_World = sp->g_World;

	SetBuffers();
}

void RADXModel::CopyPyramidToModel(Pyramid* prd)
{
	mVcount = 5;
	mVertex = new ModelVertex[mVcount];

	for (int i = 0; i < mVcount; ++i)
	{
		mVertex[i].Pos = prd->vertices[i].Pos;
		mVertex[i].Tex = prd->vertices[i].Tex;
		mVertex[i].Normal = prd->vertices[i].Normal;
	}

	mIndcount = 18;
	mIndices = new DWORD[mIndcount];

	for (int i = 0; i < mIndcount; ++i)
		mIndices[i] = prd->indices[i];

	g_World = prd->g_World;

	SetBuffers();
}

void RADXModel::CopyEPolygonToModel(EPolygon* ep)
{
	mVcount = 3;
	mVertex = new ModelVertex[mVcount];

	for (int i = 0; i < mVcount; ++i)
	{
		mVertex[i].Pos = ep->vertices[i].Pos;
		mVertex[i].Tex = ep->vertices[i].Tex;
		mVertex[i].Normal = ep->vertices[i].Normal;
	}

	mIndcount = 3;
	mIndices = new DWORD[mIndcount];

	for (int i = 0; i < mIndcount; ++i)
		mIndices[i] = ep->indices[i];

	g_World = ep->g_World;

	SetBuffers();
}

void RADXModel::SetBuffers()
{
	SetVertexBuffer();
	SetIndexBuffer();

	loadDone = true;
}

void RADXModel::SetVertexBuffer()
{
	SafeRelease(g_pVertexBuffer);

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = mVertex;

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(ModelVertex) * mVcount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	DRCA()->GetDevice()->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
}

void RADXModel::SetIndexBuffer()
{
	SafeRelease(g_pIndexBuffer);

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = mIndices;

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(DWORD) * mIndcount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	DRCA()->GetDevice()->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
}

void RADXModel::SetLayout()
{
	SafeRelease(g_pVertexLayout);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// ������� ������ ������� ������
	D3DX11_PASS_DESC PassDesc;
	DRCA()->modelShaderTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	DRCA()->GetDevice()->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout);
}

void RADXModel::Render()
{
	if (!loadDone)
		return;

	if (!DRCA()->GetRaltID()->GetrBMClck())
	{
		c += D3DX_PI * 0.005f;
		D3DXMatrixRotationY(&g_World, c);
	}

	// �������� ���������� � �������� ��������������
	DRCA()->modelShaderWorldVariable->SetMatrix((float*)&g_World);
	DRCA()->modelShaderViewVariable->SetMatrix((float*)&DRCA()->GetCamera()->g_View);
	DRCA()->modelShaderProjectionVariable->SetMatrix((float*)&DRCA()->GetCamera()->g_Projection);

	// ���� ������ ����
	float CubeColor[4] = { rgb[0], rgb[1], rgb[2], 1.0f };
	DRCA()->modelShaderColor->SetFloatVector((float*)&CubeColor);

	// �������� ���������� �� ��������� �����
	float LightColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	DRCA()->modelShaderLightColor->SetFloatVector((float*)&LightColor);

	float LightDir[4] = { 0.0f, 0.8f, -1.5f, 1.0f };
	DRCA()->modelShaderLightPosition->SetFloatVector((float*)&LightDir);

	// ������ ��� � ����� ���������
	D3DX11_TECHNIQUE_DESC techDesc;
	DRCA()->modelShaderTechnique->GetDesc(&techDesc);

	UINT stride = sizeof(ModelVertex);
	UINT offset = 0;

	// ��������� ������ ������� ������ � ����������� ����������
	DRCA()->GetDeviceContext()->IASetInputLayout(g_pVertexLayout);
	DRCA()->GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	DRCA()->GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		DRCA()->modelShaderTechnique->GetPassByIndex(p)->Apply(0, DRCA()->GetDeviceContext());
		DRCA()->GetDeviceContext()->DrawIndexed(mIndcount, 0, 0);
	}
}
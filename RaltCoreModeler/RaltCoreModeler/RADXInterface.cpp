#include "RADXInterface.h"


RADXInterface::RADXInterface()
{
	initDone = false;

	initDone = true;
}

RADXInterface::~RADXInterface()
{
	delete wLoadingTex;

	SafeRelease(lTex);
	SafeRelease(visibleSV);
	SafeRelease(colorTexture);

	SafeRelease(InterfaceShaderEffect);
	SafeRelease(g_pTechniqueInterfaceShader);
	SafeRelease(g_pTexResourceInterfaceShader);

	SafeRelease(g_pWorldVariable);
	SafeRelease(g_pViewVariable);
	SafeRelease(g_pProjectionVariable);

	SafeRelease(g_pWLoadingIndexBuffer);
	SafeRelease(g_pWLoadingVertexBuffer);
	SafeRelease(g_pWLoadingVertexLayout);
}

void RADXInterface::InitInterface()
{
	wLoadingTex = new RADXTexture("\\Interface\\images\\wloading.png");
	wLoadingTex->ImageLoad();
	InitRLWBuffers();

	RenderLoadWindow();

	iCompiler = new RADXICompiler();
	iCompiler->InitCompiler("main.raI");
	iCompiler->CompileBlock();
}

void RADXInterface::CompileShader()
{
	HRESULT hr = S_OK;
	ID3D10Blob*	g_pEffectBlob_dvx = 0;

	string data_file = MainDir() + "/Shaders/dInterface.fx";
	hr = D3DX11CompileFromFile(data_file.c_str(), NULL, NULL, "VS", "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS, NULL, NULL, &g_pEffectBlob_dvx, NULL, NULL);
	if (FAILED(hr))
	{
		DRCA()->GetLog()->WriteLog(LOG_ERROR_CDINTESHDR);
		return;
	}

	hr = D3DX11CreateEffectFromMemory(g_pEffectBlob_dvx->GetBufferPointer(), g_pEffectBlob_dvx->GetBufferSize(), 0, DRCA()->GetDevice(), &InterfaceShaderEffect);
	if (FAILED(hr))
		return;
	SafeRelease(g_pEffectBlob_dvx);

	g_pTechniqueInterfaceShader = InterfaceShaderEffect->GetTechniqueByName("RenderTexture");
	g_pTexResourceInterfaceShader = InterfaceShaderEffect->GetVariableByName("g_pTexture")->AsShaderResource();

	lTex = InterfaceShaderEffect->GetVariableByName("lTex")->AsScalar();
	visibleSV = InterfaceShaderEffect->GetVariableByName("visible")->AsScalar();
	colorTexture = InterfaceShaderEffect->GetVariableByName("ColorTexture")->AsVector();

	g_pWorldVariable = InterfaceShaderEffect->GetVariableByName("World")->AsMatrix();
	g_pViewVariable = InterfaceShaderEffect->GetVariableByName("View")->AsMatrix();
	g_pProjectionVariable = InterfaceShaderEffect->GetVariableByName("Projection")->AsMatrix();
}

void RADXInterface::SetCamera(INT width, INT heigth)
{
	FLOAT w_value = FLOAT(-width / 2.0f);
	FLOAT h_value = FLOAT(-heigth / 2.0f);

	InterfaceView(0, 0) = 1.0f;		InterfaceView(0, 1) = 0.0f;		InterfaceView(0, 2) = 0.0f; InterfaceView(0, 3) = 0.0f;
	InterfaceView(1, 0) = 0.0f;		InterfaceView(1, 1) = 1.0f;		InterfaceView(1, 2) = 0.0f; InterfaceView(1, 3) = 0.0f;
	InterfaceView(2, 0) = 0.0f;		InterfaceView(2, 1) = 0.0f;		InterfaceView(2, 2) = 1.0f; InterfaceView(2, 3) = 0.0f;
	InterfaceView(3, 0) = w_value;	InterfaceView(3, 1) = h_value;	InterfaceView(3, 2) = 0.0f; InterfaceView(3, 3) = 1.0f;
	D3DXMatrixOrthoRH(&InterfaceProjection, width, -heigth, 0.0f, 1.0f);
	D3DXMatrixIdentity(&InterfaceWorld);

	g_pViewVariable->SetMatrix((float*)&InterfaceView);
	g_pWorldVariable->SetMatrix((float*)&InterfaceWorld);
	g_pProjectionVariable->SetMatrix((float*)&InterfaceProjection);
}

void RADXInterface::InitRLWBuffers()
{
	SafeRelease(g_pWLoadingIndexBuffer);
	SafeRelease(g_pWLoadingVertexBuffer);
	SafeRelease(g_pWLoadingVertexLayout);

	DWORD indices[] =
	{
		0, 2, 3,
		0, 3, 1
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(StandartBlock) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	DRCA()->GetDevice()->CreateBuffer(&bd, NULL, &g_pWLoadingVertexBuffer);

	// Создаем индексный буфер
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(DWORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = indices;
	DRCA()->GetDevice()->CreateBuffer(&bd, &InitData, &g_pWLoadingIndexBuffer);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Создаем объект входных данных
	D3DX11_PASS_DESC PassDesc;
	DRCA()->GetInterface()->g_pTechniqueInterfaceShader->GetPassByIndex(0)->GetDesc(&PassDesc);
	DRCA()->GetDevice()->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pWLoadingVertexLayout);

	StandartBlock blockVert[4];
	blockVert[0] = { D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f) };
	blockVert[1] = { D3DXVECTOR2(0.0f, FLOAT(DRCA()->GetHeight())), D3DXVECTOR2(0.0f, 1.0f) };
	blockVert[2] = { D3DXVECTOR2(FLOAT(DRCA()->GetWidth()), 0.0f), D3DXVECTOR2(1.0f, 0.0f) };
	blockVert[3] = { D3DXVECTOR2(FLOAT(DRCA()->GetWidth()), FLOAT(DRCA()->GetHeight())), D3DXVECTOR2(1.0f, 1.0f) };

	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = DRCA()->GetDeviceContext()->Map(g_pWLoadingVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, blockVert, sizeof(blockVert));
	DRCA()->GetDeviceContext()->Unmap(g_pWLoadingVertexBuffer, 0);
}

void RADXInterface::OnResize(INT width, INT height)
{
	// Обновление информации о размерах окна
	SetCamera(width, height);
}

void RADXInterface::Render()
{
	if (DRCA()->GetRaltID()->GetShowWL())
	{
		RenderLoadWindow();
		return;
	}

	if (IBlockList.empty())
		return;

	//bool wDrcaRSState = DRCA()->dxrca_rsState;
	//DRCA()->SetRSState(true);

	for (int i = IBlockList.size() - 1; i >= 0; --i)
		IBlockList[i]->RenderBlock();

	//DRCA()->SetRSState(wDrcaRSState);
}

void RADXInterface::RenderLoadWindow()
{
	DRCA()->Clear2Buffer();

	// TODO: здесь создание кадра окна загрузки
	UINT stride = sizeof(StandartBlock);
	UINT offset = 0;

	DRCA()->GetDeviceContext()->IASetInputLayout(g_pWLoadingVertexLayout);
	DRCA()->GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pWLoadingVertexBuffer, &stride, &offset);
	DRCA()->GetDeviceContext()->IASetIndexBuffer(g_pWLoadingIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	DRCA()->GetInterface()->g_pTexResourceInterfaceShader->SetResource(wLoadingTex->GetSRV());
	DRCA()->GetInterface()->lTex->SetBool(true);
	DRCA()->GetInterface()->visibleSV->SetFloat(1.0f);

	D3DX11_TECHNIQUE_DESC techDesc;
	DRCA()->GetInterface()->g_pTechniqueInterfaceShader->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		DRCA()->GetInterface()->g_pTechniqueInterfaceShader->GetPassByIndex(p)->Apply(0, DRCA()->GetDeviceContext());
		DRCA()->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}
	
	DRCA()->PresentScene();
}

RADXIBlock* RADXInterface::GetBlockByName(string bname)
{
	for each (RADXIBlock* block in IBlockList)
		if (block->mname == bname)
			return block;

	return NULL;
}

RADXIBlock* RADXInterface::GetBlockById(INT bId)
{
	if (IBlockList.size() <= bId)
		return NULL;

	return IBlockList[bId];
}

INT RADXInterface::GetBlockIdInCoord(INT x, INT y)
{
	for (int i = 0; i < IBlockList.size(); ++i)
	{
		RADXIBlock* block = IBlockList[i];

		if (block->visible == 0.0f)
			continue;

		if (block->BlockInCoord(x, y))
			return i;
	}

	return -1;
}

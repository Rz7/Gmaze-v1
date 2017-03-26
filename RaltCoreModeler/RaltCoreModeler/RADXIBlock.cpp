#include "RADXIBlock.h"


RADXIBlock::RADXIBlock()
{
	InitBuffers();
}

void RADXIBlock::InitBuffers()
{
	DWORD indices[] =
	{
		0, 2, 3,
		0, 3, 1
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Block) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	DRCA()->GetDevice()->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

	// Создаем индексный буфер
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(DWORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = indices;
	DRCA()->GetDevice()->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Создаем объект входных данных
	D3DX11_PASS_DESC PassDesc;
	DRCA()->GetInterface()->g_pTechniqueInterfaceShader->GetPassByIndex(0)->GetDesc(&PassDesc);
	DRCA()->GetDevice()->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout);

	// Относительная позиция мыши от topX и topY
	tXYRelative[0] = 0;
	tXYRelative[1] = 0;
}

void RADXIBlock::UpdBfrs(UINT width, UINT height, INT topXLeft, INT topYLeft)
{
	blockVert[0].Pos = D3DXVECTOR2(topXLeft, topYLeft);
	blockVert[1].Pos = D3DXVECTOR2(topXLeft, topYLeft + height);
	blockVert[2].Pos = D3DXVECTOR2(topXLeft + width, topYLeft);
	blockVert[3].Pos = D3DXVECTOR2(topXLeft + width, topYLeft + height);

	for (int i = 0; i < 4; ++i)
		blockVert[i].Tex = D3DXVECTOR2(img_pos[stateBck][i][0], img_pos[stateBck][i][1]);

	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = DRCA()->GetDeviceContext()->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, blockVert, sizeof(blockVert));
	DRCA()->GetDeviceContext()->Unmap(g_pVertexBuffer, 0);

	sizeParam[0] = width;
	sizeParam[1] = height;
	sizeParam[2] = topXLeft;
	sizeParam[3] = topYLeft;
}

void RADXIBlock::UpdBck()
{
	UpdBfrs(sizeParam[0], sizeParam[1], sizeParam[2], sizeParam[3]);
	lImage = !(image_str[aliasImg[stateBck]] == "");
}

void RADXIBlock::SetImageByURL()
{
	for (int i = 0; i < 4; ++i)
	{
		if (image_str[aliasImg[i]] == "")
			continue;

		if (texture2d[aliasImg[i]])
			delete texture2d[aliasImg[i]];

		texture2d[aliasImg[i]] = new RADXTexture(image_str[aliasImg[i]]);
		texture2d[aliasImg[i]]->ImageLoad();

		printf_s("%u\n", aliasImg[i]);
	}
}

void RADXIBlock::OnLKDown(UINT x, UINT y)
{
	tXYRelative[0] = FLOAT(x - sizeParam[2]);
	tXYRelative[1] = FLOAT(y - sizeParam[3]);
}

void RADXIBlock::OnLKUp(UINT x, UINT y)
{
	tXYRelative[0] = 0.0f;
	tXYRelative[1] = 0.0f;
}

void RADXIBlock::OnMove(UINT x, UINT y, bool by_parent) // x и y - позиция мыши
{
	if (!by_parent && !cmove)
		return;

	for (int i = 0; i < 2; ++i)
		if (tXYRelative[i] == 0.0f)
			return;

	INT topXLeft = x - tXYRelative[0];
	INT topYLeft = y - tXYRelative[1];

	UpdBfrs(sizeParam[0], sizeParam[1], topXLeft, topYLeft);
}

void RADXIBlock::OnFocus(UINT x, UINT y)
{
	if (BlockInCoord(x, y)) // Блок находится в фокусе мыши.
	{
		if (stateBck == 1)
			return;

		stateBck = 1;
	}
	else
	{
		if (stateBck == 0)
			return;

		stateBck = 0;
	}

	UpdBck();
	OnFocusScripts();
}

void RADXIBlock::OnClick(UINT x, UINT y)
{
	if (BlockInCoord(x, y))
	{
		if (stateBck == 2)
			return;

		stateBck = 2;
	}
	/*else
	{
		if (stateBck == 0)
			return;

		stateBck = 0;
	}*/

	UpdBck();
	OnClickScripts();
}

void RADXIBlock::OnFocusScripts()
{
	// Обработчик событий
}

void RADXIBlock::OnClickScripts()
{
	// Обработчик событий
}

void RADXIBlock::RenderBlock()
{
	//if (!CheckBlockInPlatform())
	//	return;

	//RADXIBlock* main_block = GetMainBlockParent();
	//if (main_block && !main_block->isVisible)
	//	return;

	// Рисуем текст на блоке //
	//if (text)
	//	text->RDrawText();
	///////////////////////////

	UINT stride = sizeof(Block);
	UINT offset = 0;

	//DRCA()->GetInterface()->allocationWHTBlock->SetBool(allocation);
	DRCA()->GetDeviceContext()->IASetInputLayout(g_pVertexLayout);
	DRCA()->GetDeviceContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	DRCA()->GetDeviceContext()->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	if (lImage)
		DRCA()->GetInterface()->g_pTexResourceInterfaceShader->SetResource(texture2d[aliasImg[stateBck]]->GetSRV());
	else
		DRCA()->GetInterface()->colorTexture->SetFloatVector(bckRGBA[aliasImg[stateBck]]);
	
	DRCA()->GetInterface()->lTex->SetBool(lImage);
	DRCA()->GetInterface()->visibleSV->SetFloat(visible);

	D3DX11_TECHNIQUE_DESC techDesc;
	DRCA()->GetInterface()->g_pTechniqueInterfaceShader->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		DRCA()->GetInterface()->g_pTechniqueInterfaceShader->GetPassByIndex(p)->Apply(0, DRCA()->GetDeviceContext());
		DRCA()->GetDeviceContext()->DrawIndexed(6, 0, 0);
	}
}

RADXIBlock* RADXIBlock::GetMainParentBlock()
{
	for each (RADXIBlock* block in DRCA()->GetInterface()->IBlockList)
		if (block->mname == pname)
			return block;

	return NULL;
}

bool RADXIBlock::BlockInCoord(INT x, INT y)
{
	// Функция, возвращающая истину, если координаты x и y принадлежат данному блоку
	return ((x > sizeParam[2] && x < sizeParam[2] + sizeParam[0]) && (y > sizeParam[3] && y < sizeParam[3] + sizeParam[1]));
}

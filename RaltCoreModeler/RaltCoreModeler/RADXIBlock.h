#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class RADXTexture;
class RADXIBlock
{
	struct Block
	{
		D3DXVECTOR2 Pos;
		D3DXVECTOR2 Tex;
	};

	public:
		RADXIBlock();
		~RADXIBlock()
		{
			SafeRelease(g_pIndexBuffer);
			SafeRelease(g_pVertexBuffer);
			SafeRelease(g_pVertexLayout);
			
			for (int i = 0; i < 4; ++i)
				delete texture2d[i];
		};

		void InitBuffers();
		void SetImageByURL();
		void UpdBfrs(UINT width, UINT height, INT topXLeft, INT topYLeft);
		void UpdBck();

		void RenderBlock();

		void OnLKDown(UINT x, UINT y);
		void OnLKUp(UINT x, UINT y);
		void OnMove(UINT x, UINT y, bool by_parent);

		void OnFocus(UINT x, UINT y);
		void OnClick(UINT x, UINT y);

		void OnFocusScripts();
		void OnClickScripts();		

		bool BlockInCoord(INT x, INT y);
		RADXIBlock* GetMainParentBlock();


	private:
		Block							blockVert[4];

		ID3D11Buffer*					g_pIndexBuffer = NULL;
		ID3D11Buffer*					g_pVertexBuffer = NULL;
		ID3D11InputLayout*				g_pVertexLayout = NULL;

		RADXTexture*					texture2d[4];

	public:
		string mname;	// Имя данного блока
		string pname;	// Имя родительского блока

		INT								block_type;		// Тип блока
		BOOL							cmove;			// 0 or 1
		FLOAT							visible;		// 0.0 to 1.0
		FLOAT							tXYRelative[2];	// Расстояние от topX и topY до точки нажатия ЛКМ по блоку (для передвижения блока)
		FLOAT							sizeParam[4];	// 0 - width, 1 - height, 2 - topXLeft, 3 - topYLeft
		
		INT								stateBck;		// Состояние блока (onfocus, onclick, default) - (1, 2, 0)
		
		FLOAT							bckRGBA[4][4];	// 0 - r, 1 - g, 2 - b, 3 - a
		
		BOOL							lImage;			// Загружается ли изображение?
		int								aliasImg[4];	// Алиас изображения
		string							image_str[4];	// URL OF IMAGE
		float							img_pos[4][4][2]; // Загружаемая область изображения
		
};


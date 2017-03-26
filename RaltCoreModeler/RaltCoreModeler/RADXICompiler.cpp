#include "RADXICompiler.h"

RADXICompiler::RADXICompiler()
{

}

RADXICompiler::~RADXICompiler()
{

}

void RADXICompiler::InitCompiler(string mainfile_url)
{
	// Булениарные переменные, показывающие, какая из функций сейчас компилируется
	bool initVMain = false;	// Инициализация входной функции
	bool initBlock = false; // Инициализация блока

	// Переменная, указывающая на количество открытых и закрытых скобок.
	// Число отрицательное - ошибка кода, закрывающих скобок больше
	// Число положительное - скобка открыта
	// Число = 0 - все скобки закрыты
	int braceCount = 0;

	// Строка кода
	string source_text;

	// Индекс массива, указывающий, какое состояние заднего фона блока записывается в память
	int stateBck = 0;

	// Результаты компиляции
	BlockInfoCmpl* nBlock = NULL;
	fstream f(MainDir() + "\\Interface\\code\\" + mainfile_url, ios::in | ios::out);
	if (f && f.is_open())
	{
		while (getline(f, source_text))
		{
			if (source_text[0] == '/')
				continue;

			if (source_text[0] == '{')
			{
				++braceCount;
				continue;
			}

			if (source_text[0] == '}')
			{
				--braceCount;
				continue;
			}

			if (braceCount < 0)
				return;				// CODE_ERROR

			if (braceCount == 0)
			{
				// Функции инициализируются только в том случае, если количество открытых
				// и закрытых скобок равно. Это означает о корректности уже прошедшего кода.

				initVMain = false;
				initBlock = false;

				if (EqSLine(source_text, "void main"))
					initVMain = true;

				if (EqSLine(source_text, "block"))
					initBlock = true;
			}

			if (EqSLine(source_text, "#include"))
			{
				DelSpace(&source_text);
				CutSTRINDX(&source_text, 9, source_text.length() - 1);
				InitCompiler(source_text);
			}

			// Инициализация входной функции
			if (initVMain)
			{
				if (EqSLine(source_text, "init"))
				{
					SelTIBracket(&source_text);
					bNameList[counter_list++] = source_text;
					continue;
				}
			}

			if (initBlock)
			{
				if (braceCount == 0 && EqSLine(source_text, "block"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 5, source_text.length());

					nBlock = new BlockInfoCmpl();
					nBlock->mname = source_text;
					nBlock->pname = "";

					nBlock->sizeX = 0;
					nBlock->sizeY = 0;
					nBlock->topX = 0;
					nBlock->topY = 0;

					nBlock->block_type = 1;
					nBlock->visible = 1.0f;
					nBlock->cmove = false;

					nBlock->stateBck = 0;
					for (int i = 0; i < 4; ++i)
					{
						nBlock->bck_r[i] = 255;
						nBlock->bck_g[i] = 255;
						nBlock->bck_b[i] = 255;
						nBlock->img_str[i] = "";

						// Изначально алиас картинки - сама картинка
						nBlock->aliasImg[i] = i;

						// Таблица позиций - вся картинка
						nBlock->img_pos[i][0][0] = 0.0f;
						nBlock->img_pos[i][0][1] = 0.0f;

						nBlock->img_pos[i][1][0] = 0.0f;
						nBlock->img_pos[i][1][1] = 1.0f;

						nBlock->img_pos[i][2][0] = 1.0f;
						nBlock->img_pos[i][2][1] = 0.0f;

						nBlock->img_pos[i][3][0] = 1.0f;
						nBlock->img_pos[i][3][1] = 1.0f;
					}

					if (BlockInfoCList.size() == 10000)
					{
						DRCA()->GetLog()->WriteLog(LOG_ERROR_INTERFACE_BLOCKLIST_10000);
						return;
					}

					if (!EquNameBlock(nBlock))
						BlockInfoCList.push_back(nBlock);
				}

				if (EqSLine(source_text, "size:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 5, source_text.length());

					int smblIDX = GetINDXSYMT(source_text, 'x');
					if (smblIDX == -1)	// Если символа не существует
						continue;

					string s_sizeX, s_sizeY;
					s_sizeX = source_text;
					s_sizeY = source_text;

					CutSTRINDX(&s_sizeX, 0, smblIDX);
					CutSTRINDX(&s_sizeY, smblIDX + 1, source_text.length());

					int sizeX = atoi(s_sizeX.c_str());	// Размер блока по X
					int sizeY = atoi(s_sizeY.c_str());	// Размер блока по Y

					nBlock->sizeX = sizeX;
					nBlock->sizeY = sizeY;
				}

				if (EqSLine(source_text, "topX:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 5, source_text.length());
					nBlock->topX = atoi(source_text.c_str());	// Отступ по X
				}

				if (EqSLine(source_text, "topY:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 5, source_text.length());
					nBlock->topY = atoi(source_text.c_str());	// Отступ по Y
				}

				if (EqSLine(source_text, "pname:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 6, source_text.length());
					nBlock->pname = source_text;				// Имя родительского блока
				}

				if (EqSLine(source_text, "visible:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 8, source_text.length());
					nBlock->visible = atof(source_text.c_str());	// Видимость блока
				}

				if (EqSLine(source_text, "cmove:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 6, source_text.length());
					nBlock->cmove = atoi(source_text.c_str());		// Передвигается ли блок
				}

				if (EqSLine(source_text, "block_type:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 11, source_text.length());
					nBlock->block_type = atoi(source_text.c_str());	// Тип блока
				}

				if (EqSLine(source_text, "bcg::default:"))
					stateBck = 0;

				if (EqSLine(source_text, "bcg::onfocus:"))
					stateBck = 1;

				if (EqSLine(source_text, "bcg::onclick:"))
					stateBck = 2;

				if (EqSLine(source_text, "bck_r:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 6, source_text.length());
					nBlock->bck_r[stateBck] = atoi(source_text.c_str());	// Цвет заднего фона RGB(r)
				}

				if (EqSLine(source_text, "bck_g:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 6, source_text.length());
					nBlock->bck_g[stateBck] = atoi(source_text.c_str());	// Цвет заднего фона RGB(g)
				}

				if (EqSLine(source_text, "bck_b:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 6, source_text.length());
					nBlock->bck_b[stateBck] = atoi(source_text.c_str());	// Цвет заднего фона RGB(b)
				}

				if (EqSLine(source_text, "img_str:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 8, source_text.length());
					nBlock->img_str[stateBck] = source_text;				// Картинка заднего фона
				}

				if (EqSLine(source_text, "img_alias:"))
				{
					DelSpace(&source_text);
					CutSTRINDX(&source_text, 10, source_text.length());
					nBlock->aliasImg[stateBck] = atoi(source_text.c_str()); // Картинка заднего фона
				}

				if (EqSLine(source_text, "img_pos"))
				{
					DelSpace(&source_text);

					string img_pos = source_text;
					CutSTRINDX(&img_pos, 7, 9);

					CutSTRINDX(&source_text, 10, source_text.length());

					int smblIDX = GetINDXSYMT(source_text, 'x');
					if (smblIDX == -1)	// Если символа не существует
						continue;

					string img_pos_0 = source_text;
					string img_pos_1 = source_text;

					CutSTRINDX(&img_pos_0, 0, smblIDX);
					CutSTRINDX(&img_pos_1, smblIDX + 1, source_text.length());

					int nImgPos = 0;
					switch (atoi(img_pos.c_str()))
					{
						case 0: nImgPos = 0;  break;
						case 1: nImgPos = 1;  break;
						case 10: nImgPos = 2; break;
						case 11: nImgPos = 3; break;
					}

					nBlock->img_pos[stateBck][nImgPos][0] = atof(img_pos_0.c_str());
					nBlock->img_pos[stateBck][nImgPos][1] = atof(img_pos_1.c_str());
				}
			}
		}
	}
	f.close();
}

void RADXICompiler::CompileBlock()
{
	for each (BlockInfoCmpl* nB in BlockInfoCList)
	{
		if (!HINITBlock(nB->mname))
			continue;

		/*
		printf_s("1111111111111111111\n");
		printf_s("Инициализация блока: %s\n", nB->mname.c_str());

		for (int i = 0; i < 4; ++i)
		{
			printf_s("Состояние #%4.2f\n", FLOAT(i));
			printf_s("bck_r: %4.2f\n", FLOAT(nB->bck_r[i]));
			printf_s("bck_g: %4.2f\n", FLOAT(nB->bck_g[i]));
			printf_s("bck_b: %4.2f\n", FLOAT(nB->bck_b[i]));
			printf_s("img_str: %s\n", nB->img_str[i].c_str());
		}
		
		printf_s("1111111111111111111\r\n");
		*/

		RADXIBlock* new_block = new RADXIBlock();
		new_block->UpdBfrs(nB->sizeX, nB->sizeY, nB->topX, nB->topY);
		new_block->block_type = nB->block_type;
		new_block->mname = nB->mname;
		new_block->pname = nB->pname;
		new_block->cmove = nB->cmove;
		new_block->visible = nB->visible;

		for (int i = 0; i < 4; ++i)
		{
			new_block->bckRGBA[i][0] = FLOAT(nB->bck_r[i]) / 255.0f;
			new_block->bckRGBA[i][1] = FLOAT(nB->bck_g[i]) / 255.0f;
			new_block->bckRGBA[i][2] = FLOAT(nB->bck_b[i]) / 255.0f;
			new_block->bckRGBA[i][3] = 1.0f;

			new_block->image_str[i] = nB->img_str[i];

			new_block->aliasImg[i] = nB->aliasImg[i];

			for (int j = 0; j < 4; ++j)
				if (nB->img_str[nB->aliasImg[i]] == "")
					DRCA()->GetLog()->WriteLog(LOG_ERROR_INTERFACE_ALIAS_EMPTLY_URLIMG);
			
			for (int j = 0; j < 4; ++j)
				for (int z = 0; z < 2; ++z)
					new_block->img_pos[i][j][z] = nB->img_pos[i][j][z];
		}

		new_block->SetImageByURL();
		new_block->UpdBck(); // для обновления background

		DRCA()->GetInterface()->IBlockList.push_back(new_block);
	}
}

bool RADXICompiler::EqSLine(string st, string f)
{
	// Удаляем все пробелы в строке с кодом
	DelSpace(&st);
	DelSpace(&f);

	if (st.length() < f.length())
		return false;

	for (int i = 0; i < f.length(); ++i)
	{
		if (st[i] != f[i])
			return false;
	}

	return true;
}

void RADXICompiler::DelSpace(string* str)
{
	string new_str = "";

	for (int i = 0; i < (*str).length(); ++i)
		if ((*str)[i] != ' ' && (*str)[i] != '	')
			new_str += (*str)[i];

	(*str) = new_str;
}

void RADXICompiler::SelTIBracket(string* source)
{
	int bracketC = 0;
	string s_result = "";

	for (int i = 0; i < (*source).length(); ++i)
	{
		if ((*source)[i] == '(')
		{
			++bracketC;
			continue;
		}

		if ((*source)[i] == ')')
			--bracketC;

		if (bracketC > 0)
			s_result += (*source)[i];
	}

	(*source) = s_result;
}

string RADXICompiler::CutSTRINDX(string* str, int indxS, int indxE)
{
	string s_result = "";

	for (int i = indxS; i < indxE; ++i)
	{
		if ((*str)[i] == ';')
			break;

		s_result += (*str)[i];
	}

	(*str) = s_result;
	return s_result;
}

int RADXICompiler::GetINDXSYMT(string str, char symbol)
{
	for (int i = 0; i < str.length(); ++i)
		if (str[i] == symbol)
			return i;

	return -1;
}

bool RADXICompiler::HINITBlock(string bname)
{
	for (int i = 0; i < 1024; ++i)
		if (bNameList[i] == bname)
			return true;

	return false;
}

bool RADXICompiler::EquNameBlock(BlockInfoCmpl* bic)
{
	for each (BlockInfoCmpl* nB in BlockInfoCList)
		if (nB != bic && nB->mname == bic->mname)
			return true;

	return false;
}

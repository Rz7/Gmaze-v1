#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class RADXICompiler
{
	struct BlockInfoCmpl
	{
		// Структура информации о каждом блоке, который необходимо скомпилировать

		// Основная собранная информация
		int topX;		// Отступ по X		от начала родительского блока
		int topY;		// Отступ по Y		или экрана (при block_type = 0)
		int sizeX;		// Размер блока по X
		int sizeY;		// Размер блока по Y

		string mname;	// Имя блока
		string pname;	// Имя родительского блока
		int block_type;	// Тип блока
		float visible;	// Видимость блока
		bool cmove;		// Передвигается ли блок (самостоятельно)

		// Цвет RGB заднего фона
		int stateBck;		// дефолтное состояние блока (0)
		int bck_r[4];		// Компонент красного
		int bck_g[4];		// Компонент зеленого
		int bck_b[4];		// Компонент голубого

		int aliasImg[4];		// Если изображения разных состояний одинаковы, можно указать алиас изображения, чтобы оно было загружено 1 раз
		float img_pos[4][4][2];	// Загружаемая область картинки: 4 состояния, 4 позиции, 2 точки позиции - X и Y
		string img_str[4];		// Или путь к картинке
	};

	public:
		RADXICompiler();
		~RADXICompiler();

		void InitCompiler(string mainfile_url);

		// Функция компиляции блоков
		void CompileBlock();

		// Функция эквивалентности части st и всего f
		bool EqSLine(string st, string f);

		// Функция удаления пробелов
		void DelSpace(string* str);

		// Функция выделения текста, находящегося в скобках
		void SelTIBracket(string* source);

		// Функция вырезаения текста в зависимости от установленных индексов
		string CutSTRINDX(string* str, int indxS, int indxE);

		// Функция нахождения символа в строке и возврата его индекса в массиве
		int GetINDXSYMT(string str, char symbol);

		// Функция, возвращающая true, если блок был инициализирован в коде
		bool HINITBlock(string bname);

		// Функция возвращает true, если блок с данным именем уже существует
		bool EquNameBlock(BlockInfoCmpl* bic);

	private:
		int counter_list = 0;	// счетчик заполнения bNameList

		string bNameList[1024];
		std::vector<BlockInfoCmpl*> BlockInfoCList;
};


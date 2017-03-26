#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

class RADXICompiler
{
	struct BlockInfoCmpl
	{
		// ��������� ���������� � ������ �����, ������� ���������� ��������������

		// �������� ��������� ����������
		int topX;		// ������ �� X		�� ������ ������������� �����
		int topY;		// ������ �� Y		��� ������ (��� block_type = 0)
		int sizeX;		// ������ ����� �� X
		int sizeY;		// ������ ����� �� Y

		string mname;	// ��� �����
		string pname;	// ��� ������������� �����
		int block_type;	// ��� �����
		float visible;	// ��������� �����
		bool cmove;		// ������������� �� ���� (��������������)

		// ���� RGB ������� ����
		int stateBck;		// ��������� ��������� ����� (0)
		int bck_r[4];		// ��������� ��������
		int bck_g[4];		// ��������� ��������
		int bck_b[4];		// ��������� ��������

		int aliasImg[4];		// ���� ����������� ������ ��������� ���������, ����� ������� ����� �����������, ����� ��� ���� ��������� 1 ���
		float img_pos[4][4][2];	// ����������� ������� ��������: 4 ���������, 4 �������, 2 ����� ������� - X � Y
		string img_str[4];		// ��� ���� � ��������
	};

	public:
		RADXICompiler();
		~RADXICompiler();

		void InitCompiler(string mainfile_url);

		// ������� ���������� ������
		void CompileBlock();

		// ������� ��������������� ����� st � ����� f
		bool EqSLine(string st, string f);

		// ������� �������� ��������
		void DelSpace(string* str);

		// ������� ��������� ������, ������������ � �������
		void SelTIBracket(string* source);

		// ������� ���������� ������ � ����������� �� ������������� ��������
		string CutSTRINDX(string* str, int indxS, int indxE);

		// ������� ���������� ������� � ������ � �������� ��� ������� � �������
		int GetINDXSYMT(string str, char symbol);

		// �������, ������������ true, ���� ���� ��� ��������������� � ����
		bool HINITBlock(string bname);

		// ������� ���������� true, ���� ���� � ������ ������ ��� ����������
		bool EquNameBlock(BlockInfoCmpl* bic);

	private:
		int counter_list = 0;	// ������� ���������� bNameList

		string bNameList[1024];
		std::vector<BlockInfoCmpl*> BlockInfoCList;
};


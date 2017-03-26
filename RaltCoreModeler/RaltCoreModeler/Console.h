#pragma once
#include "RaltCoreModeler.h"

using namespace std;

class Console
{
public:
	Console() { RedirectIOToConsole(); };
	~Console() {};

	// maximum mumber of lines the output console should have
	static const WORD MAX_CONSOLE_LINES = 500;

	void RedirectIOToConsole();
	void ConsoleWriteLineL(LPCSTR text)
	{
		cout << text << endl;
	};
	
	void ConsoleWriteLineF(FLOAT text)
	{
		std::wstringstream s; s << text;
		std::wcout << s.str() << "\n";
	};
};
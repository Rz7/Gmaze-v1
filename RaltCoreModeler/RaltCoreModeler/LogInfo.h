#pragma once
#include "RaltCoreModeler.h"
#include "DRXCA.h"

enum LogInfoID
{
	LOG_INFO_START = -1,

	LOG_INFO_LOADSTART = 0,
	LOG_INFO_DEVLOAD = 1,
	LOG_INFO_3DGRAPHIC = 3,
	LOG_INFO_LOADEND = 4,

	LOG_ERROR_LOAD_DEVICE = 100,
	LOG_ERROR_MAPSE = 101,
	LOG_ERROR_SAVETEXTFROMRT = 102,

	LOG_ERROR_CDINTESHDR = 107,
	LOG_ERROR_INTERFACE_ALIAS_EMPTLY_URLIMG = 108,
	LOG_ERROR_INTERFACE_BLOCKLIST_10000 = 109,

	//..........//
	//..........//
	//..........//

	LOG_INFO_END = 512,
};

class LogInfo
{
	public:
		LogInfo();
		~LogInfo() {};

		void WriteLog(INT imessage);
		void WriteToFile(string filename, string new_line);

	private:
		string logmsg[512];		// Выделяется 512 ошибок и предупреждений.
		string logfname;		// Имя файла логов
};


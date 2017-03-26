#include "LogInfo.h"

LogInfo::LogInfo()
{
	logfname = MainDir();// +
	logfname += "/log.txt";

	return;

	ofstream f;
	f.open(logfname);
	f.clear();
	f.close();


	logmsg[LOG_INFO_LOADSTART] = "/////////////////////////////////////\r//RaltCore Map Creator             //\r//Version: 1.0.1 [Release]		   //\r/////////////////////////////////////\r";

	logmsg[LOG_INFO_DEVLOAD] = "[Successfully] Loading device done\r";
	logmsg[LOG_INFO_3DGRAPHIC] = "[Successfully] Loading 3dGraphic done\r";
	logmsg[LOG_INFO_LOADEND] = "[Successfully] Loading DirectX done\r";

	logmsg[LOG_ERROR_LOAD_DEVICE] = "[Error] Loading device fault\r";
	logmsg[LOG_ERROR_MAPSE] = "[Error] MapSegmentEffect loading fault\r";
	logmsg[LOG_ERROR_SAVETEXTFROMRT] = "[Error] Saving image from render target fault\r";
	logmsg[LOG_ERROR_CDINTESHDR] = "[Error] Compile dinterface shader fault\r";

	logmsg[LOG_ERROR_INTERFACE_ALIAS_EMPTLY_URLIMG] = "[Error] Alias image url is emptly\r";
	logmsg[LOG_ERROR_INTERFACE_BLOCKLIST_10000] = "[Error] BlockList size bigger 10000\r";
	
}

void LogInfo::WriteLog(INT imessage)
{
	SYSTEMTIME sm;
	GetSystemTime(&sm);

	string time = to_string(sm.wDay) + "." + to_string(sm.wMonth) + "." + to_string(sm.wYear) + " " + to_string(sm.wHour) + ":" + to_string(sm.wMinute);

	string data_msg = "[" + time + "]" + logmsg[imessage];

	if (imessage == LOG_INFO_LOADSTART)
		data_msg = logmsg[imessage];

	WriteToFile(logfname, data_msg);
}

void LogInfo::WriteToFile(string filename, string new_line)
{
	/*ofstream f(filename, std::ios_base::app);

	if (f && f.is_open())
		f << new_line;

	f.close();*/
}

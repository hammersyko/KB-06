#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <windows.h>
#include <map>
#include <fstream>

namespace Logger {
	class Logger
	{
	public:
		~Logger();
		Logger();
		static enum LogLevel{
			NONE = 0,
			ERR = 1,
			WARNING = 2,
			DEBUG = 3,
			INFO = 4
		};

		void Log(int logType, char* text);
		void Log(int logType, std::string text);
		void LogMemoryDump(int logType, void* const p_address, const int p_size, char* const p_name);
		void SetLogLevel(int logLevel);
		void Reset();
	private:
		void PrintConsole(int logType, char* text);
		char* BuildLogEntry(int logType, char* messasge);

		int logLevel = INFO;
		int consoleColorCodeInfo = gray;
		int consoleColorCodeDebug = white;
		int consoleColorCodeWarning = yellow;
		int consoleColorCodeError = red;
		HANDLE consoleHandle;

		enum Consolecolor
		{
			black = 0,
			dark_blue = 1,
			dark_green = 2,
			dark_aqua, dark_cyan = 3,
			dark_red = 4,
			dark_purple = 5, dark_pink = 5, dark_magenta = 5,
			dark_yellow = 6,
			dark_white = 7,
			gray = 8,
			blue = 9,
			green = 10,
			aqua = 11, cyan = 11,
			red = 12,
			purple = 13, pink = 13, magenta = 13,
			yellow = 14,
			white = 15
		};
	};
}
#endif
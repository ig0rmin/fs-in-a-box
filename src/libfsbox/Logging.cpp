#ifdef ENABLE_LOGGING
#include "Logging.h"

#ifdef _WIN32
#include <Windows.h>
#else
	//TODO: POSIX support
#endif

#include <cstdio>
#include <cstdarg>

static const char* LogLevelToStr(LogLevel lvl)
{
	switch (lvl)
	{
		case LogLevel::Debug:
			return "DEBUG";
		case LogLevel::Error:
			return "ERROR";
		case LogLevel::Info:
			return "INFO";
	}
	return "";
}

void LogMsg(LogLevel lvl, const char* function, long line, const char* msg, ...)
{
	const size_t MAX_USR_MSG = 256;
	char usrMsg[MAX_USR_MSG] = {0};

	va_list args;
	va_start (args, msg);
	vsnprintf(usrMsg, MAX_USR_MSG, msg, args);
	va_end(args);

	const size_t MAX_MSG_PREFIX = 256;
	const size_t MAX_LOG_MSG = MAX_MSG_PREFIX + MAX_USR_MSG;
	char logMsg[MAX_LOG_MSG] = {0};

	snprintf(logMsg, MAX_LOG_MSG, "[%s] - %s:%d %s\n", LogLevelToStr(lvl), function, line, usrMsg);

#ifdef _WIN32
	OutputDebugStringA(logMsg);
#else
	printf("%s\n", logMsg);
#endif
}

#endif
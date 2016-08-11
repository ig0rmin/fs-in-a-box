#ifndef __FSBOX_LOGGING_H__
#define __FSBOX_LOGGING_H__

//TODO: We need better logging library

#ifdef ENABLE_LOGGING

#ifdef _WIN32
#define FUNCTION __FUNCTION__
#else
#define FUNCTION __PRETTY_FUNCTION__
#endif

enum class LogLevel {Debug, Info, Error};

void LogMsg(LogLevel lvl, const char* function, long line, const char* msg, ...);

#define LOG_DEBUG(msg,...) {LogMsg(LogLevel::Debug, FUNCTION, __LINE__, msg, __VA_ARGS__);}
#define LOG_INFO(msg,...) {LogMsg(LogLevel::Info, FUNCTION, __LINE__, msg, __VA_ARGS__);}
#define LOG_ERROR(msg,...) {LogMsg(LogLevel::Error, FUNCTION, __LINE__,msg, __VA_ARGS__);}

#else

#define LOG_DEBUG
#define LOG_INFO
#define LOG_ERROR

#endif // ENABLE_LOGGING

#endif
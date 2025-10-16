#pragma once

#include <string>
#include "noncopyable.h"

// 简易的日志系统，非线程安全版本

#define LOG_INFO(msgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, msgFormat, ##__VA_ARGS__); \
        logger.Log(INFO, buf); \
    } while(0) 

#define LOG_ERROR(msgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, msgFormat, ##__VA_ARGS__); \
        logger.Log(ERROR, buf); \
    } while(0) 

#define LOG_FATAL(msgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, msgFormat, ##__VA_ARGS__); \
        logger.Log(FATAL, buf); \
        exit(-1); \
    } while(0) 

#ifdef MUDEBUG
#define LOG_DEBUG(msgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, msgFormat, ##__VA_ARGS__); \
        logger.Log(DEBUG, buf); \
    } while(0) 
#else
    #define LOG_DEBUG(msgFormat, ...)
#endif

enum LogLevel
{
    FATAL, // core信息
    ERROR, // 错误信息
    INFO,  // 普通信息
    DEBUG, // 调试信息
};

// 输出一个日志类
class Logger : NonCopyable
{
public:
    static Logger& Instance();
    void Log(LogLevel level, const std::string& msg);
};
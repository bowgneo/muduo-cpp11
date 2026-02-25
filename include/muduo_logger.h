#pragma once

#include <string>
#include "muduo_noncopyable.h"

// LOG_INFO("%s %d", arg1, arg2)
#define LOG_INFO(msgFormat, ...)                       \
    do                                                 \
    {                                                  \
        Logger &logger = Logger::instance();           \
        char buf[1024] = {0};                          \
        snprintf(buf, 1024, msgFormat, ##__VA_ARGS__); \
        logger.log(LogLevel::INFO, buf);               \
    } while (0)

#define LOG_ERROR(msgFormat, ...)                      \
    do                                                 \
    {                                                  \
        Logger &logger = Logger::instance();           \
        char buf[1024] = {0};                          \
        snprintf(buf, 1024, msgFormat, ##__VA_ARGS__); \
        logger.log(LogLevel::ERROR, buf);              \
    } while (0)

#define LOG_FATAL(msgFormat, ...)                      \
    do                                                 \
    {                                                  \
        Logger &logger = Logger::instance();           \
        char buf[1024] = {0};                          \
        snprintf(buf, 1024, msgFormat, ##__VA_ARGS__); \
        logger.log(LogLevel::FATAL, buf);              \
        exit(-1);                                      \
    } while (0)

#ifdef MUDEBUG
#define LOG_DEBUG(msgFormat, ...)                      \
    do                                                 \
    {                                                  \
        Logger &logger = Logger::instance();           \
        char buf[1024] = {0};                          \
        snprintf(buf, 1024, msgFormat, ##__VA_ARGS__); \
        logger.log(LogLevel::DEBUG, buf);              \
    } while (0)
#else
#define LOG_DEBUG(msgFormat, ...)
#endif

// 定义日志的级别  INFO  ERROR  FATAL  DEBUG
enum class LogLevel
{
    DEBUG, // 调试信息
    INFO,  // 普通信息
    ERROR, // 错误信息
    FATAL  // core信息
};

class Logger : Noncopyable
{
public:
    // 获取日志唯一的实例对象
    static Logger &instance();

    // 写日志
    static void log(LogLevel log_level, const std::string &msg);
};

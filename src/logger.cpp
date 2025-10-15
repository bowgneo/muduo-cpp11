#include <iostream>
#include "Logger.h"

// 获取日志唯一的实例对象
Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

// 写日志  [级别信息] time : msg
void Logger::log(LogLevel level, const std::string &msg)
{
    switch (level)
    {
        case INFO:
            std::cout << "[INFO]";
            break;
        case ERROR:
            std::cout << "[ERROR]";
            break;
        case FATAL:
            std::cout << "[FATAL]";
            break;
        case DEBUG:
            std::cout << "[DEBUG]";
            break;
        default:
            break;
    }

    std::cout << "[]" << " : " << msg << std::endl;
}
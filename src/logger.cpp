#include <iostream>
#include "time_stamp.h"
#include "Logger.h"

// 获取日志唯一的实例对象
Logger &Logger::Instance()
{
    static Logger logger;
    return logger;
}

// 写日志  [级别信息] time : msg
void Logger::Log(LogLevel level, const std::string &msg)
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

    std::cout << "[" << TimeStamp::Now().ToString() << "]" << " : " << msg << std::endl;
}
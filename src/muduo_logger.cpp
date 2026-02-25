#include <iostream>

#include "muduo_logger.h"
#include "muduo_time_stamp.h"

// 获取日志唯一的实例对象
Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

// 写日志  [级别信息] time : msg
void Logger::log(LogLevel log_level, const std::string &msg)
{
    switch (log_level)
    {
    case LogLevel::INFO:
        std::cout << "[INFO]";
        break;
    case LogLevel::ERROR:
        std::cout << "[ERROR]";
        break;
    case LogLevel::FATAL:
        std::cout << "[FATAL]";
        break;
    case LogLevel::DEBUG:
        std::cout << "[DEBUG]";
        break;
    default:
        break;
    }

    std::cout << Timestamp::now().toString() << " : " << msg << std::endl;
}

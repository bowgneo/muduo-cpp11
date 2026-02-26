#pragma once

#include "muduo_thread.h"
#include "muduo_noncopyable.h"

#include <functional>
#include <mutex>
#include <string>
#include <condition_variable>
#include <memory>

class EventLoop;

class EventLoopThread : Noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    EventLoopThread(ThreadInitCallback cb = ThreadInitCallback(),
                    const std::string &name = std::string());

    ~EventLoopThread();

    EventLoop *startLoop();

private:
    void threadFunc();

    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
    std::unique_ptr<EventLoop> loopPtr_;
};

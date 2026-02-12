#pragma once

#include "muduo_thread.h"
#include "muduo_noncopyable.h"

#include <functional>
#include <mutex>
#include <string>
#include <condition_variable>
#include <memory>

// 前向声明
class EventLoop;

class EventLoopThread : Noncopyable {
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
    std::unique_ptr<EventLoop> loopPtr_; // 用于管理 EventLoop 的生命周期
};

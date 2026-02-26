#include "muduo_event_loop.h"
#include "muduo_event_loop_thread.h"

#include <utility>
#include <memory>

EventLoopThread::EventLoopThread(ThreadInitCallback cb,
                                 const std::string &name)
    : exiting_(false), thread_([this]
                               { threadFunc(); }, name),
      mutex_(), cond_(), callback_(std::move(cb)), loopPtr_(nullptr)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loopPtr_ != nullptr)
    {
        loopPtr_->quit();
        thread_.join();
    }
}

EventLoop *EventLoopThread::startLoop()
{
    thread_.start();
    {
        // 线程的函数内创建 event loop 对象
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]
                   { return loopPtr_ != nullptr; });
    }
    return loopPtr_.get();
}

void EventLoopThread::threadFunc()
{
    std::unique_ptr<EventLoop> loopRaw = std::make_unique<EventLoop>();
    if (callback_)
    {
        callback_(loopRaw.get());
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loopPtr_ = std::move(loopRaw);
        cond_.notify_one();
    }

    loopPtr_->loop();

    std::unique_lock<std::mutex> lock(mutex_);
    loopPtr_.reset(); // 释放 EventLoop 对象
}

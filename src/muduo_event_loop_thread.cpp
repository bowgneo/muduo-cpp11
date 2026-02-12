#include "muduo_event_loop.h"
#include "muduo_event_loop_thread.h"

#include <utility>
#include <memory>

EventLoopThread::EventLoopThread(ThreadInitCallback cb,
                                 const std::string &name)
    : exiting_(false)
      , thread_([this] { threadFunc(); }, name)
      , mutex_()
      , cond_()
      , callback_(std::move(cb))
      , loopPtr_(nullptr) {
}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loopPtr_ != nullptr) {
        loopPtr_->quit();
        thread_.join();
    }
    // loopPtr_ 会在析构时自动释放内存
}

EventLoop *EventLoopThread::startLoop() {
    thread_.start();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return loopPtr_ != nullptr; });
    }
    return loopPtr_.get();
}

void EventLoopThread::threadFunc() {
    // one loop per thread
    // 使用智能指针管理 EventLoop 生命周期

    std::unique_ptr<EventLoop> loopRaw = std::make_unique<EventLoop>();
    if (callback_) {
        callback_(loopRaw.get());
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loopPtr_ = std::move(loopRaw);
        cond_.notify_one();
    }

    // event loop
    loopPtr_->loop();

    std::unique_lock<std::mutex> lock(mutex_);
    loopPtr_.reset(); // 释放 EventLoop 对象
}

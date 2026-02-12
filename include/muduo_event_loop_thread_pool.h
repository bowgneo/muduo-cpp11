#pragma once

#include "muduo_noncopyable.h"

#include <functional>
#include <string>
#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : Noncopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    EventLoopThreadPool(EventLoop *baseLoop, std::string nameArg);

    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }

    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    // 如果工作在多线程中，baseLoop_ 默认以轮询的方式分配 channel 给 subloop
    EventLoop *getNextLoop();

    std::vector<EventLoop *> getAllLoops();

    [[nodiscard]] bool started() const { return started_; }
    [[nodiscard]] std::string name() const { return name_; }

private:
    EventLoop *baseLoop_; // main event loop
    std::string name_;
    bool started_;
    int numThreads_;
    size_t next_;
    std::vector<std::unique_ptr<EventLoopThread> > threads_;
    std::vector<EventLoop *> loops_;
};

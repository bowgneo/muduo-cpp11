#include "muduo_event_loop_thread.h"
#include "muduo_event_loop_thread_pool.h"

#include <memory>
#include <utility>

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, std::string nameArg)
    : baseLoop_(baseLoop)
      , name_(std::move(nameArg))
      , started_(false)
      , numThreads_(0)
      , next_(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() = default;

void EventLoopThreadPool::start(const ThreadInitCallback &cb) {
    for (int i = 0; i < numThreads_; ++i) {
        std::string buf = name_ + std::to_string(i);
        auto *t = new EventLoopThread(cb, buf);
        threads_.emplace_back(t);
        loops_.emplace_back(t->startLoop());
    }

    // 整个服务端只有一个线程，运行着 baseLoop
    if (numThreads_ == 0 && cb) {
        cb(baseLoop_);
    }

    started_ = true;
}

// 如果工作在多线程中，baseLoop_ 默认以轮询的方式分配 channel 给 subloop
EventLoop *EventLoopThreadPool::getNextLoop() {
    EventLoop *loop = baseLoop_;

    if (!loops_.empty()) // 通过轮询获取下一个处理事件的 loop
    {
        loop = loops_[next_++];
        if (next_ >= loops_.size()) {
            next_ = 0;
        }
    }

    return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
    if (loops_.empty()) {
        return {baseLoop_};
    }

    return loops_;
}

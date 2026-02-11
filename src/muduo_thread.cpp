#include "muduo_thread.h"
#include "muduo_current_thread.h"

#include <utility>
#include <mutex>
#include <condition_variable>

std::atomic_int Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, std::string name)
    : started_(false)
      , joined_(false)
      , tid_(0)
      , func_(std::move(func))
      , name_(std::move(name)) {
    setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_) {
        thread_->detach();
    }
}

void Thread::start() {
    bool thread_started_ = false;
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    std::condition_variable cond;

    thread_ = std::make_shared<std::thread>([&]() {
        tid_ = CurrentThread::tid();
        {
            std::lock_guard<std::mutex> lock_(mtx);
            thread_started_ = true;
        }
        cond.notify_one();
        func_();
    });

    cond.wait(lock, [&]() { return thread_started_; });
    started_ = true;
}

void Thread::join() {
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName() {
    if (name_.empty()) {
        char buf[32] = {0};
        snprintf(buf, sizeof buf, "Thread-%d", ++numCreated_);
        name_ = buf;
    }
}

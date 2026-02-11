#pragma once

#include <mutex>
#include <atomic>
#include <vector>
#include <memory>
#include <functional>

#include "muduo_time_stamp.h"
#include "muduo_noncopyable.h"
#include "muduo_current_thread.h"

class Channel;
class Poller;

// 事件循环：从 Poll 获取活跃事件的 Channel，并执行回调
// one loop per thread!!!
class EventLoop : Noncopyable {
public:
    using Functor = std::function<void()>;

    EventLoop();

    ~EventLoop();

    // 开启事件循环
    void loop();

    // 退出事件循环
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    // 在当前 Eventloop中 执行 cb
    void runInLoop(const Functor& cb);

    // 把 cb 放入队列中，唤醒 Eventloop 所在的线程，执行 cb
    void queueInLoop(Functor cb);

    // 唤醒 Eventloop 所在的线程
    void wakeup() const;

    // 更新 channel 后，有可能涉及到在 poll 中进行同步更新，用于提供给 channel 使用
    void updateChannel(Channel *channel) const;

    void removeChannel(Channel *channel) const;

    bool hasChannel(Channel *channel) const;

    // 判断某个 EventLoop 对象是否和当前线程是关联的
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    // Eventloop 中的 wake up channel 唤醒后执行的操作
    void handleRead() const;

    // 处理额外的回调
    void doPendingFunctors();

    using ChannelList = std::vector<Channel *>;

    // Eventloop 状态
    std::atomic_bool looping_;
    std::atomic_bool quit_;

    // 当前 Eventloop 对象的线程 id
    const pid_t threadId_;

    Timestamp pollReturnTime_;
    ChannelList activeChannels_;
    std::unique_ptr<Poller> poller_;

    // 唤醒
    // 主要作用，mainLoop 唤醒 subloop 处理
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    // 当前 Eventloop 线程需要执行的任务队列， 即 cb 操作
    std::atomic_bool callingFunctors_;
    std::vector<Functor> pendingFunctors_;
    std::mutex mutex_;
};

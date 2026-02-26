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
class EventLoop : Noncopyable
{
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
    void runInLoop(const Functor &cb);

    // 把 cb 放入队列中，唤醒 Eventloop 所在的线程，执行 cb
    void queueInLoop(Functor cb);

    // 唤醒 Eventloop 所在的线程
    void wakeup() const;

    // 更新 poll 中管理的 channel
    void updateChannel(Channel *channel) const;

    void removeChannel(Channel *channel) const;

    bool hasChannel(Channel *channel) const;

    // 判断当前 eventloop 关联的线程是否是当前线程
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    // wake up channel 的可读事件
    void handleRead() const;

    // 处理额外的回调
    void doPendingFunctors();

    using ChannelList = std::vector<Channel *>;

    // Eventloop 状态
    std::atomic_bool looping_;
    std::atomic_bool quit_;

    // 当前 Eventloop 关联的线程 id
    const pid_t threadId_;

    Timestamp pollReturnTime_;
    ChannelList activeChannels_;
    std::unique_ptr<Poller> poller_;

    // 唤醒 loop
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    // 标记当前 eventloop 线程正在处理 pendingFunctors
    std::atomic_bool callingFunctors_;
    std::vector<Functor> pendingFunctors_;
    std::mutex mutex_;
};

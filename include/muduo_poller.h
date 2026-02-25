#pragma once

#include "muduo_noncopyable.h"
#include "muduo_time_stamp.h"

#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;

// muduo 库中 io 多路复用的接口
class Poller : Noncopyable
{
public:
    // channel 会通过 eventlop 注册到 poller 中
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop *loop);

    virtual ~Poller() = default;

    // io 多路复用统一的接口， 获取活跃的 channel
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;

    virtual void updateChannel(Channel *channel) = 0;

    virtual void removeChannel(Channel *channel) = 0;

    bool hasChannel(Channel *channel) const;

    // 获取默认的具体实现
    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    // map：sockfd ---> channel
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_;
};
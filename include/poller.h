#pragma once

#include <vector>
#include <unordered_map>

#include "noncopyable.h"
#include "time_stamp.h"

class Channel;
class EventLoop;

class Poller : NonCopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    virtual TimeStamp Poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void UpdateChannel(Channel *channel) = 0;
    virtual void RemoveChannel(Channel *channel) = 0;

    bool HasChannel(Channel *channel) const;

    static Poller* NewDefaultPoller(EventLoop *loop);

protected:
    // key：sockfd  value：sockfd 所属的 channel 通道
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
    
private:
    EventLoop *ownerLoop_; // 定义Poller所属的事件循环EventLoop
};
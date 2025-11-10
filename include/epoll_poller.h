#pragma once

#include <vector>
#include <sys/epoll.h>
#include "poller.h"
#include "time_stamp.h"

class Channel;

/**
 * epoll_create
 * epoll_ctl   add/mod/del
 * epoll_wait
 */
class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop *loop);
    ~EPollPoller() override;

    TimeStamp Poll(int timeoutMs, ChannelList *activeChannels) override;
    void UpdateChannel(Channel *channel) override;
    void RemoveChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;

    void FillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    void Update(int operation, Channel *channel);

    using EventList = std::vector<epoll_event>;

    int epollfd_;
    EventList events_;
};
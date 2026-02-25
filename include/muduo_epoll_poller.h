#pragma once

#include <sys/epoll.h>
#include <vector>

#include "muduo_poller.h"
#include "muduo_time_stamp.h"

class Channel;

/**
 * muduo 中对 epoll 操作进行封装
 * epoll_create
 * epoll_ctl   add/mod/del
 * epoll_wait
 */
class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop *loop);

    ~EPollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;

    // 在 epoll 上更新 channel，执行 epoll_ctl，并更新 channel map
    void updateChannel(Channel *channel) override;

    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;

    // 根据 epoll_wait 返回的 epoll_event 回填 activeChannels
    static void fillActiveChannels(int numEvents, ChannelList *activeChannels);

    static void update(int operation, Channel *channel);

    using EventList = std::vector<epoll_event>;

    int epollfd_{};
    EventList events_;
};

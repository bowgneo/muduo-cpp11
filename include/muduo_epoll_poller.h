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
class EPollPoller : public Poller {
public:
    EPollPoller(EventLoop *loop);

    ~EPollPoller() override;

    // 重写基类接口
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;

    // 保存 channel，在 epoll 上更新 channel，执行 epoll_ctl
    void updateChannel(Channel *channel) override;

    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;

    // 回填 epoll_wait 返回的连接
    static void fillActiveChannels(int numEvents, ChannelList *activeChannels);

    static void update(int operation, Channel *channel);

    using EventList = std::vector<epoll_event>;

    int epollfd_{};
    EventList events_;
};

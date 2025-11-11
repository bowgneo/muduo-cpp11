#include <stdlib.h>

#include "poller.h"
#include "epoll_poller.h"


Poller* Poller::NewDefaultPoller(EventLoop *loop)
{
    if (::getenv("MUDUO_USE_POLL"))
    {
        return nullptr; // 生成poll的实例
    }
    else
    {
        return new EPollPoller(loop); // 生成epoll的实例
    }
}
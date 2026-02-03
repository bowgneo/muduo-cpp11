#include "muduo_poller.h"
#include "muduo_epoll_poller.h"

#include <stdlib.h>

Poller *Poller::newDefaultPoller(EventLoop *loop) {
    if (::getenv("MUDUO_USE_POLL")) {
        return nullptr;
    } else {
        return dynamic_cast<Poller *>(new EPollPoller(loop));
    }
}

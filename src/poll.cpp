#include "poller.h"
#include "channel.h"

Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop)
{
}

bool Poller::HasChannel(Channel *channel) const
{
    auto it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}
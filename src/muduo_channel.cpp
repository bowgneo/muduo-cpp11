#include "muduo_logger.h"
#include "muduo_channel.h"
#include "EventLoop.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

// EventLoop: ChannelList Poller
Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), index_(-1), events_(0), revents_(0), tied_(false) {
}

Channel::~Channel() = default;

// channel 的 tie 方法调时机？一个 TcpConnection 新连接创建的时候 TcpConnection => Channel
void Channel::tie(const std::shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}

/**
 * 当改变 channel 所 events 后，需要在 poller 里面的epoll 执行对应的 epoll_ctl
 */
void Channel::update() {
    loop_->updateChannel(this);
}

// 在 poller 中， 从 epoll 中移除 channel
void Channel::remove() {
    loop_->removeChannel(this);
}

void Channel::handleEvent(const Timestamp receiveTime) {
    if (tied_) {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    } else {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(const Timestamp &receiveTime) {
    LOG_INFO("channel handleEvent events:%d\n", revents_);

    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (revents_ & EPOLLERR) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}

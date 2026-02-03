#include "muduo_logger.h"
#include "muduo_poller.h"
#include "muduo_channel.h"
#include "muduo_event_loop.h"

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/eventfd.h>

// one loop per thread
__thread EventLoop *t_loopInThisThread = nullptr;

constexpr int kPollTimeMs = 10000;

int createEventfd() {
    int eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (eventfd < 0) {
        LOG_FATAL("eventfd error:%d \n", errno);
    }
    return eventfd;
}

EventLoop::EventLoop()
    : looping_(false)
      , quit_(false)
      , threadId_(CurrentThread::tid())
      , poller_(Poller::newDefaultPoller(this))
      , wakeupFd_(createEventfd())
      , wakeupChannel_(new Channel(this, wakeupFd_))
      , callingFunctors_(false) {
    LOG_DEBUG("EventLoop created %p in thread %d \n", this, threadId_);
    if (t_loopInThisThread) {
        LOG_FATAL("Another EventLoop %p exists in this thread %d \n", t_loopInThisThread, threadId_);
    }
    t_loopInThisThread = this;
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping \n", this);

    while (!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (Channel *channel: activeChannels_) {
            // EventLoop 从 poll 中获取活跃的 Channel，然后执行 channel 的回调
            channel->handleEvent(pollReturnTime_);
        }

        // 当前 EventLoop 需要处理的其余回调
        /**
         * IO 线程 mainLoop accept fd -----> subloop
         * subloop 执行 mainloop 注册的 cb 操作
         */
        doPendingFunctors();
    }

    LOG_INFO("EventLoop %p stop looping. \n", this);
    looping_ = false;
}

// 退出事件循环
// 1.loop 在自己的线程中调用 quit
// 2.在非 loop 的线程中，调用次 loop 的 quit
void EventLoop::quit() {
    quit_ = true;

    // 如果是在其它线程中，调用的 quit 需要唤醒
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor &cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
    }

    // || callingFunctors_的意思是：当前 loop 正在执行回调，需要显示唤醒一次，在下次 loop 循环时，执行pendingFunctors_中的 cb
    if (!isInLoopThread() || callingFunctors_) {
        wakeup(); // 唤醒loop所在线程
    }
}

void EventLoop::handleRead() const {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8", n);
    }
}

// 执行唤醒操作， 向 wakeupfd_写一个数据
void EventLoop::wakeup() const {
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n", n);
    }
}

void EventLoop::updateChannel(Channel *channel) const {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) const {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) const {
    return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors() {
    callingFunctors_ = true;
    std::vector<Functor> functors;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor &functor: functors) {
        functor();
    }

    callingFunctors_ = false;
}

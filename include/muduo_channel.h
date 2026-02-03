#pragma once

#include <memory>
#include <functional>

#include "muduo_time_stamp.h"
#include "muduo_noncopyable.h"

class EventLoop;

/**
 * Channel 封装了 fd 和其感兴趣的 event
 * 还绑定了 poller 返回的具体事件
 */
class Channel : Noncopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    // channel 会关联到具体的事件循环（epoll、poll、select）
    Channel(EventLoop *loop, int fd);

    ~Channel();

    // channel 事件触发后，处理事件
    void handleEvent(Timestamp receiveTime);

    // 设置回调函数对象
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    // 防止当 channel 被手动 remove 掉，channel 还在执行回调操作
    void tie(const std::shared_ptr<void> &);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void setRevents(int revt) { revents_ = revt; }

    // 设置 channel 相应的事件状态
    void enableReading() {
        events_ |= kReadEvent;
        update();
    }

    void disableReading() {
        events_ &= ~kReadEvent;
        update();
    }

    void enableWriting() {
        events_ |= kWriteEvent;
        update();
    }

    void disableWriting() {
        events_ &= ~kWriteEvent;
        update();
    }

    void disableAll() {
        events_ = kNoneEvent;
        update();
    }

    // 返回 channel 当前的关注事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() const { return index_; }
    void setIndex(int idx) { index_ = idx; }

    // channel 关联的事件循环
    EventLoop *ownerLoop() const { return loop_; }

    void remove();

private:
    void update();

    void handleEventWithGuard(const Timestamp &receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; // channel 关联的事件循环
    const int fd_; // channel 关联的 fd
    int index_; // channel 的类别，在 poller 中区分，即是否添加到 poller 中等标记
    int events_; // channel 感兴趣的事件
    int revents_; // channel 具体发生的事件

    std::weak_ptr<void> tie_;
    bool tied_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

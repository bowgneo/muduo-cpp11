#pragma once

#include <memory>
#include <functional>

#include "noncopyable.h"
#include "time_stamp.h"

class EventLoop;

/**
 * channel类: 封装 fd、 event，并且通过保存对 epoll obj 的引用
 */
class Channel : NonCopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(TimeStamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    void HandleEvent(TimeStamp receiveTime);

    // 设置回调函数对象
    void SetReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void SetWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void SetCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void SetErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    // 防止当 channel 被手动 remove 掉，channel 还在执行回调操作
    void Tie(const std::shared_ptr<void> &);

    int Fd() const { return fd_; }
    int Events() const { return events_; }
    int SetRevents(int revt) { revents_ = revt; }

    // 设置fd相应的事件状态
    void EnableReading()
    {
        events_ |= kReadEvent;
        Update();
    }
    void DisableReading()
    {
        events_ &= ~kReadEvent;
        Update();
    }
    void EnableWriting()
    {
        events_ |= kWriteEvent;
        Update();
    }
    void DisableWriting()
    {
        events_ &= ~kWriteEvent;
        Update();
    }
    void DisableAll()
    {
        events_ = kNoneEvent;
        Update();
    }

    // 返回 fd 当前的事件状态
    bool IsNoneEvent() const { return events_ == kNoneEvent; }
    bool IsWriting() const { return events_ & kWriteEvent; }
    bool IsReading() const { return events_ & kReadEvent; }

    int Index() { return index_; }
    void SetIndex(int idx) { index_ = idx; }

    // one loop per thread
    EventLoop *OwnerLoop() { return loop_; }
    void Remove();

private:
    void Update();
    void HandleEventWithGuard(TimeStamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; // 事件循环
    const int fd_;    // fd
    int events_;      // fd 感兴趣的事件
    int revents_;     // 返回的具体发生的事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    // 因为 channel 通道里面能够获知 fd 最终发生的具体的事件 revents，具体事件的回调操作
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

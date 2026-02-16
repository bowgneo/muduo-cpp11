#pragma once

#include "muduo_socket.h"
#include "muduo_channel.h"
#include "muduo_noncopyable.h"

#include <functional>

class EventLoop;
class InetAddress;

class Acceptor : Noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int, const InetAddress &)>;

    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb)
    {
        newConnectionCallback_ = cb;
    }

    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

    EventLoop *loop_; // Acceptor 由 main loop 管理
    Socket acceptSocket_;
    Channel acceptChannel_;
    bool listenning_;
    NewConnectionCallback newConnectionCallback_;
};
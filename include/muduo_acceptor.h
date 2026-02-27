#pragma once

#include "muduo_socket.h"
#include "muduo_channel.h"
#include "muduo_noncopyable.h"

#include <functional>

class EventLoop;
class InetAddress;

/*
 * 在 mainloop 中创建 listen channel，并设置读回调，当有新连接时，触发回调函数
 */
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

    void listen();
    bool listenning() const { return listenning_; }

private:
    // listen channel 读回调函数
    void handleRead();

    EventLoop *loop_; // 关联的 main loop
    Socket acceptSocket_;
    Channel acceptChannel_;
    bool listenning_;
    NewConnectionCallback newConnectionCallback_;
};
#pragma once

#include "muduo_noncopyable.h"

class InetAddress;

class Socket : Noncopyable
{
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
    {
    }
    Socket() = default;
    ~Socket();

    static int createNonblocking();
    void setSocket(int sockfd) { sockfd_ = sockfd; }
    int fd() const { return sockfd_; }
    void bindAddress(const InetAddress &localaddr);
    void listen();
    int accept(InetAddress *peeraddr);

    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    int sockfd_;
};
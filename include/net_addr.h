#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

/*
 * InetAddress类封装了IPv4地址和端口
 */
class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr) : addr_(addr)
    {
    }

    std::string ToIp() const;
    uint16_t ToPort() const;
    std::string ToIpPort() const;

    const sockaddr_in &GetSockAddr() const { return addr_; }
    void SetSockAddr(const sockaddr_in &addr) { addr_ = addr; }

private:
    sockaddr_in addr_;
};
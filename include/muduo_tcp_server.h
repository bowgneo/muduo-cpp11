#pragma once

#include "muduo_event_loop.h"
#include "muduo_acceptor.h"
#include "muduo_address.h"
#include "muduo_noncopyable.h"
#include "muduo_event_loop_thread_pool.h"
#include "muduo_tcp_connection.h"
#include "muduo_callbacks.h"
#include "muduo_buffer.h"

#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>

class TcpServer : Noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop *loop,
              const InetAddress &listenAddr,
              const std::string &nameArg,
              Option option = kNoReusePort);
    ~TcpServer();

    void setThreadInitcallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

    // 设置 subloop 的数量
    void setThreadNum(int numThreads);

    // 启动服务端
    void start();

private:
    void newConnection(int sockfd, const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;
    ConnectionMap connections_;
    int nextConnId_;

    EventLoop *loop_;                                 // mainLoop
    std::unique_ptr<Acceptor> acceptor_;              // 在 mainLoop 中添加监听新连接事件的 channel
    std::shared_ptr<EventLoopThreadPool> threadPool_; // one loop per thread

    const std::string ipPort_;
    const std::string name_;

    ConnectionCallback connectionCallback_;       // 连接变化时的回调
    MessageCallback messageCallback_;             // 有读写消息时的回调
    WriteCompleteCallback writeCompleteCallback_; // 消息发送完成以后的回调
    ThreadInitCallback threadInitCallback_;       // loop线程初始化的回调

    std::atomic_int started_;
};
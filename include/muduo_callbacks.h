#pragma once

#include <memory>
#include <functional>

class Buffer;
class Timestamp;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
// 连接状态变换时的回调
using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
// 连接断时的回调
using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
// 消息发送完成时的回调
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
// 有消息时的回调
using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr &, size_t)>;
#include "muduo_buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t Buffer::readFd(int fd, int *saveErrno)
{
    char extrabuf[65536] = {0}; // 栈上的内存空间 64K

    struct iovec vec[2];

    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    // 限制一次性读取的字节数最少 64k
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *saveErrno = errno;
    }
    else if (n <= writable)
    {
        writerIndex_ += n;
    }
    else
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }

    return n;
}

ssize_t Buffer::writeFd(int fd, int *saveErrno)
{
    ssize_t n = ::write(fd, peek(), readableBytes());
    if (n < 0)
    {
        *saveErrno = errno;
    }
    else
    {

        retrieve(n);
    }

    return n;
}

const char *Buffer::peek() const
{
    return begin() + readerIndex_;
}

char *Buffer::begin()
{
    return &*buffer_.begin();
}

const char *Buffer::begin() const
{
    return &*buffer_.begin();
}

size_t Buffer::readableBytes() const
{
    return writerIndex_ - readerIndex_;
}

size_t Buffer::writableBytes() const
{
    return buffer_.size() - writerIndex_;
}

char *Buffer::beginWrite()
{
    return begin() + writerIndex_;
}

const char *Buffer::beginWrite() const
{
    return begin() + writerIndex_;
}

void Buffer::append(const char *data, size_t len)
{
    ensureWriteableBytes(len);
    std::copy(data, data + len, beginWrite());
    writerIndex_ += len;
}

void Buffer::ensureWriteableBytes(size_t len)
{
    if (writableBytes() < len)
    {
        makeSpace(len);
    }
}

void Buffer::makeSpace(size_t len)
{
    if (writableBytes() + readerIndex_ < len + kCheapPrepend)
    {
        buffer_.resize(writerIndex_ + len);
    }

    // 把可读数据移动到 cheapPrepend 之后的最前面，然后复位索引
    size_t readalbe = readableBytes();
    std::copy(begin() + readerIndex_,
              begin() + writerIndex_,
              begin() + kCheapPrepend);
    readerIndex_ = kCheapPrepend;
    writerIndex_ = readerIndex_ + readalbe;
}

std::string Buffer::retrieveAllAsString()
{
    return retrieveAsString(readableBytes());
}

std::string Buffer::retrieveAsString(size_t len)
{
    std::string result(peek(), len > readableBytes() ? readableBytes() : len);
    retrieve(len);
    return result;
}

void Buffer::retrieve(size_t len)
{
    if (len < readableBytes())
    {
        readerIndex_ += len;
    }
    else
    {
        retrieveAll();
    }
}

void Buffer::retrieveAll()
{
    writerIndex_ = kCheapPrepend;
    readerIndex_ = writerIndex_;
}
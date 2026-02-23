#pragma once

#include <vector>
#include <string>
#include <algorithm>

/*
 +***********+***********+***********+
 +  prepend  +  read     +    write  +
 +           +           +           +
 +***********+***********+***********+
 0        readindex    writeindex
*/
class Buffer
{
public:
    static const size_t kCheapPrepend = 8; // 最小預留的空白可写空間, 此 8 字节不用來写真实数据，而是再有需要時记录一些原数据信息
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize),
          readerIndex_(kCheapPrepend), writerIndex_(kCheapPrepend)
    {
    }

    ssize_t readFd(int fd, int *saveErrno);
    ssize_t writeFd(int fd, int *saveErrno);

    const char *begin() const;
    const char *peek() const;
    const char *beginWrite() const;

    size_t readableBytes() const;
    size_t writableBytes() const;

    std::string retrieveAllAsString();
    std::string retrieveAsString(size_t len);

private:
    char *begin();
    char *beginWrite();

    size_t prependableBytes() const;
    void ensureWriteableBytes(size_t len);
    void makeSpace(size_t len);
    void append(const char *data, size_t len);

    void retrieveAll();
    void retrieve(size_t len);

    std::vector<char> buffer_;
    size_t readerIndex_; // 可续读索引
    size_t writerIndex_; // 可续写索引
};
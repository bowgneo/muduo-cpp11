#pragma once

/**
 * @note NonCopyable 类禁止拷贝构造和拷贝赋值
 * @note 该类可以作为基类使用，防止派生类在类外被实例化或者拷贝
 */ 
class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};
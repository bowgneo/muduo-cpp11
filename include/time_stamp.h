#pragma once

#include <string>
#include <cstdint>
#include <iostream>

class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(int64_t microSecondsSinceEpoch);
    static TimeStamp Now();
    std::string ToString() const;
private:
    int64_t microSecondsSinceEpoch_;
};
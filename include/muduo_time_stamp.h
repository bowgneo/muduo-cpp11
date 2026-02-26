#pragma once

#include <string>
#include <iostream>

class Timestamp
{
public:
    Timestamp();

    explicit Timestamp(time_t microSecondsSinceEpoch) : microSecondsSinceEpoch_(microSecondsSinceEpoch) {
                                                        };

    static Timestamp now();

    std::string toString() const;

private:
    time_t microSecondsSinceEpoch_;
};

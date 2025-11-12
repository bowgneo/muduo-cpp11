#pragma once

#include <unistd.h>
#include <sys/syscall.h>

extern __thread int t_cachedTid;

void CacheTid();

inline int Tid()
{
    if (__builtin_expect(t_cachedTid == 0, 0))
    {
        CacheTid();
    }
    return t_cachedTid;
}
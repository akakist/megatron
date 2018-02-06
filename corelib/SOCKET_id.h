#ifndef ______SOCKET_id___H
#define ______SOCKET_id___H
#include "commonError.h"
#include <stdio.h>
#include "ioBuffer.h"
#ifdef DEBUG

struct SOCKET_id
{
    int64_t container;
    SOCKET_id() {}

};
inline int operator<(const SOCKET_id&a, const SOCKET_id&b)
{
    return a.container<b.container;
}
inline int operator==(const SOCKET_id&a, const SOCKET_id&b)
{
    return a.container==b.container;
}
inline int operator!=(const SOCKET_id&a, const SOCKET_id&b)
{
    return a.container!=b.container;
}
inline outBuffer & operator<< (outBuffer& b,const SOCKET_id &s)
{
    b<<s.container;
    return b;
}
inline inBuffer & operator>> (inBuffer& b,  SOCKET_id &s)
{
    b>>s.container;
    return b;
}

#else
typedef int64_t SOCKET_id;
#endif


#endif

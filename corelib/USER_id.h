
#ifndef _______________USER_id_H
#define _______________USER_id_H
#include <string>
#include <stdint.h>
#ifdef DEBUG
#include "commonError.h"
#include "ioBuffer.h"
struct USER_id
{
    int64_t container;
    USER_id(const int64_t& n):container(n) {}
    USER_id() {}
};
inline bool operator < (const USER_id &a,const USER_id &b)
{
    return a.container<b.container;
}
inline bool operator == (const USER_id &a,const USER_id &b)
{
    return a.container==b.container;
}
inline bool operator != (const USER_id &a,const USER_id &b)
{
    return a.container!=b.container;
}

inline outBuffer & operator<< (outBuffer& b,const USER_id &s)
{
    b<<s.container;
    return b;
}
inline inBuffer & operator>> (inBuffer& b,  USER_id &s)
{
    b>>s.container;
    return b;
}


#else
typedef int64_t USER_id;
#endif


#endif


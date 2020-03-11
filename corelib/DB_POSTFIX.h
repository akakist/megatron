#ifndef ______DB_POSTFIX___H
#define ______DB_POSTFIX___H
#include "commonError.h"
#include "ioBuffer.h"
#ifdef DEBUG

/// this is typed int for use in debug mode

struct DB_POSTFIX
{
    int64_t container;
    DB_POSTFIX() {}
};
inline bool operator < (const DB_POSTFIX& a,const DB_POSTFIX& b)
{
    return a.container<b.container;
}
inline bool operator == (const DB_POSTFIX& a,const DB_POSTFIX& b)
{
    return a.container==b.container;
}
inline bool operator != (const DB_POSTFIX& a,const DB_POSTFIX& b)
{
    return a.container!=b.container;
}
inline outBuffer & operator<< (outBuffer& b,const DB_POSTFIX &s)
{
    b<<s.container;
    return b;
}
inline inBuffer & operator>> (inBuffer& b,  DB_POSTFIX &s)
{
    b>>s.container;
    return b;
}

#else
typedef int64_t DB_POSTFIX;
#endif

#endif

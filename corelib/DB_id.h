#ifndef ____________DB__ID__HH
#define ____________DB__ID__HH
#include "commonError.h"
#include "ioBuffer.h"
#include <sys/types.h>

/// this is typed int for use in debug mode

#ifdef DEBUG
struct DB_id
{
    int64_t container;
    DB_id(const int64_t &n): container(n) {}
    DB_id() {}

};
inline bool operator < (const DB_id& a,const DB_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const DB_id& a,const DB_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const DB_id& a,const DB_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const DB_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  DB_id& s)
{
    b>>s.container;
    return b;
}



struct UNIQUE_id
{
    int64_t container;
    UNIQUE_id(const int64_t &n): container(n) {}
    UNIQUE_id() {}

};
inline bool operator < (const UNIQUE_id& a,const UNIQUE_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const UNIQUE_id& a,const UNIQUE_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const UNIQUE_id& a,const UNIQUE_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const UNIQUE_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  UNIQUE_id& s)
{
    b>>s.container;
    return b;
}

struct SHARE_id
{
    int64_t container;
    SHARE_id(const int64_t &n): container(n) {}
    SHARE_id() {}

};
inline bool operator < (const SHARE_id& a,const SHARE_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const SHARE_id& a,const SHARE_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const SHARE_id& a,const SHARE_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const SHARE_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  SHARE_id& s)
{
    b>>s.container;
    return b;
}

struct STAT_id
{
    int64_t container;
    STAT_id(const int64_t &n): container(n) {}
    STAT_id() {}

};
inline bool operator < (const STAT_id& a,const STAT_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const STAT_id& a,const STAT_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const STAT_id& a,const STAT_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const STAT_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  STAT_id& s)
{
    b>>s.container;
    return b;
}



#else
typedef int64_t DB_id;
typedef int64_t HOST_id;
typedef int64_t SHARE_id;
typedef int64_t STAT_id;
typedef int64_t UNIQUE_id;

#endif
#endif

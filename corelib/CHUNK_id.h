#ifndef ______CHUNK_id___H
#define ______CHUNK_id___H
#include "commonError.h"
#include "ioBuffer.h"
#ifdef DEBUG
struct CHUNK_id
{
    int64_t container;
    CHUNK_id() {}
};
inline bool operator < (const CHUNK_id& a,const CHUNK_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const CHUNK_id& a,const CHUNK_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const CHUNK_id& a,const CHUNK_id& b)
{
    return a.container!=b.container;
}
inline outBuffer & operator<< (outBuffer& b,const CHUNK_id &s)
{
    b<<s.container;
    return b;
}
inline inBuffer & operator>> (inBuffer& b,  CHUNK_id &s)
{
    b>>s.container;
    return b;
}

#else
typedef int64_t CHUNK_id;
#endif

#endif

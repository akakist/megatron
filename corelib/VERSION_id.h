#ifndef VERSION__ID__HH
#define VERSION__ID__HH
#include "commonError.h"
#include "ioBuffer.h"
#include <sys/types.h>
#ifdef DEBUG

struct VERSION_id
{
    int32_t container;
    VERSION_id(const int32_t &n): container(n) {}
    VERSION_id() {}

};
inline bool operator < (const VERSION_id& a,const VERSION_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const VERSION_id& a,const VERSION_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const VERSION_id& a,const VERSION_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const VERSION_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  VERSION_id& s)
{
    b>>s.container;
    return b;
}


#else
typedef int32_t VERSION_id;
#endif
#endif

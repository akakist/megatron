#ifndef ____________AUTHCOOKIE__ID__HH
#define ____________AUTHCOOKIE__ID__HH
#include "commonError.h"
#include "ioBuffer.h"
#include <sys/types.h>
#ifdef DEBUG

/// this is typed int for use in debug mode

struct AUTHCOOKIE_id
{
    std::string container;
    AUTHCOOKIE_id() {}

};
inline bool operator < (const AUTHCOOKIE_id& a,const AUTHCOOKIE_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const AUTHCOOKIE_id& a,const AUTHCOOKIE_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const AUTHCOOKIE_id& a,const AUTHCOOKIE_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const AUTHCOOKIE_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  AUTHCOOKIE_id& s)
{
    b>>s.container;
    return b;
}





#else
typedef std::string AUTHCOOKIE_id;

#endif
#endif

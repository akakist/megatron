#ifndef ____________JAVACOOKIE__ID__HH
#define ____________JAVACOOKIE__ID__HH
#include "ioBuffer.h"
#ifdef DEBUG

/// string wrapper for debug
///
struct JAVACOOKIE_id
{
    std::string container;
    JAVACOOKIE_id(const std::string &n): container(n) {}
    JAVACOOKIE_id(const char *n): container(n) {}
    JAVACOOKIE_id() {}

};
inline bool operator < (const JAVACOOKIE_id& a,const JAVACOOKIE_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const JAVACOOKIE_id& a,const JAVACOOKIE_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const JAVACOOKIE_id& a,const JAVACOOKIE_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const JAVACOOKIE_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  JAVACOOKIE_id& s)
{
    b>>s.container;
    return b;
}





#else
typedef std::string JAVACOOKIE_id;

#endif
#endif

#ifndef __________EVENT_ID___H
#define __________EVENT_ID___H
#include "ioBuffer.h"

/// EventID class. Possible be a number (old style) and string
struct EVENT_id
{
private:
    EVENT_id(int i);
public:
    EVENT_id(const std::string& i):s_id(i) {
    }
    EVENT_id() {}

    std::string getSid() const
    {
        return s_id;
    }

private:
    std::string s_id;

    friend int operator<(const EVENT_id&a, const EVENT_id&b);
    friend int operator==(const EVENT_id&a, const EVENT_id&b);
    friend outBuffer& operator<< (outBuffer& b,const EVENT_id& s);
    friend inBuffer& operator>> (inBuffer& b,  EVENT_id& s);

public:
    std::string dump() const
    {
        return s_id;
    }
};

inline int operator<(const EVENT_id&a, const EVENT_id&b)
{
    return a.s_id<b.s_id;
}

inline int operator==(const EVENT_id&a, const EVENT_id&b)
{
    return a.s_id==b.s_id;
}

inline outBuffer& operator<< (outBuffer& b,const EVENT_id& s)
{
    b<<s.s_id;
    return b;
}

inline inBuffer& operator>> (inBuffer& b,  EVENT_id& s)
{

    b>>s.s_id;
    return b;
}



#endif

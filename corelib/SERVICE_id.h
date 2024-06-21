#pragma once
#include <sys/types.h>
#include <stdint.h>
#include "ioBuffer.h"

/// Service ID. Possible be a number (old style) and string

struct SERVICE_id
{
private:
    SERVICE_id(int i);
public:
    SERVICE_id(const std::string& i):s_id(i) {}
    SERVICE_id() {}

    std::string getSid() const
    {
        return s_id;
    }
private:
    std::string s_id;

    friend int operator<(const SERVICE_id&a, const SERVICE_id&b);
    friend int operator==(const SERVICE_id&a, const SERVICE_id&b);
    friend outBuffer& operator<< (outBuffer& b,const SERVICE_id& s);
    friend inBuffer& operator>> (inBuffer& b,  SERVICE_id& s);
    friend int operator!=(const SERVICE_id&a, const SERVICE_id&b);

public:
    std::string dump() const
    {
        return s_id;
    }
};
inline int operator<(const SERVICE_id&a, const SERVICE_id&b)
{
    return a.s_id<b.s_id;
}
inline int operator==(const SERVICE_id&a, const SERVICE_id&b)
{
    return a.s_id==b.s_id;
}

inline outBuffer& operator<< (outBuffer& b,const SERVICE_id& s)
{
    b<<s.s_id;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  SERVICE_id& s)
{
    b>>s.s_id;
    return b;
}
inline int operator!=(const SERVICE_id&a, const SERVICE_id&b)
{
    return a.s_id!=b.s_id;
}

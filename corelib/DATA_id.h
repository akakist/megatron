#pragma once
#include "ioBuffer.h"
#include <string>
struct DATA_id
{
    std::string container;
private:
    DATA_id(const int64_t &) {}
public:
    DATA_id() {}
    DATA_id(const std::string &n): container(n) {}
    std::string toString() const
    {
        return container;
        //return std::to_string(container);
    }

};
inline bool operator < (const DATA_id& a,const DATA_id& b)
{
    return a.container<b.container;
}
inline bool operator == (const DATA_id& a,const DATA_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const DATA_id& a,const DATA_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const DATA_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  DATA_id& s)
{
    b>>s.container;
    return b;
}

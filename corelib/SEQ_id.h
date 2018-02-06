#ifndef ___SEQ_ID_H
#define ___SEQ_ID_H
#include <string>
#include "IInstance.h"
struct SEQ_id
{
    int64_t container;
    SEQ_id(const int64_t &n): container(n) {}
    SEQ_id() {}
    std::string toString() const
    {
        return iUtils->toString(container);
    }

};
inline bool operator < (const SEQ_id& a,const SEQ_id& b)
{
    return a.container<b.container;
}
inline bool operator > (const SEQ_id& a,const SEQ_id& b)
{
    return a.container>b.container;
}
inline bool operator == (const SEQ_id& a,const SEQ_id& b)
{
    return a.container==b.container;
}
inline bool operator != (const SEQ_id& a,const SEQ_id& b)
{
    return a.container!=b.container;
}
inline outBuffer& operator<< (outBuffer& b,const SEQ_id& s)
{
    b<<s.container;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  SEQ_id& s)
{
    b>>s.container;
    return b;
}

#endif

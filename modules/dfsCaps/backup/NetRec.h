#ifndef ___________NETREC_H
#define ___________NETREC_H
#include "ioBuffer.h"
struct NetRec
{
    int32_t startIp,endIp;
    int32_t countryPos;
    int32_t countryMax;
    int32_t locationPos;
    int32_t locationMax;
    int32_t netnumPos;
    int32_t netnumMax;
    /// определаем ИД как endIp
};
inline int operator<(const NetRec& a,const NetRec& b)
{
    return a.endIp<b.endIp;
}
inline outBuffer& operator<< (outBuffer& b,const NetRec& s)
{
    b<<s.startIp<<s.endIp<<s.countryPos<<s.countryMax<<s.locationPos<<s.locationMax<<s.netnumPos<<s.netnumMax;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  NetRec& s)
{
    b>>s.startIp>>s.endIp>>s.countryPos>>s.countryMax>>s.locationPos>>s.locationMax>>s.netnumPos>>s.netnumMax;
    return b;
}
inline int comp(const NetRec& a,const NetRec& b)
{
    return a.endIp<b.endIp;
}

#endif // NETREC_H

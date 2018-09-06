#ifndef ________________IGEOIPAV___G11
#define ________________IGEOIPAV___G11
#include <stdint.h>
#include "ifaces.h"
#include <string>
#include <sys/types.h>
#include "REF.h"
#include "refstring.h"
#include "ioBuffer.h"
#include "IInstance.h"
/// interfaces/wrapper to access OpenGEOIP

struct geoNetRec
{
    std::string startIp,endIp;
    double lat,lon;
};
inline outBuffer & operator<< (outBuffer& b,const geoNetRec &s)
{
    b<<s.startIp<<s.endIp<<s.lat<<s.lon;
    return b;
}
inline inBuffer & operator>> (inBuffer& b,  geoNetRec &s)
{
    b>>s.startIp>>s.endIp>>s.lat>>s.lon;
    return b;
}

inline int operator<(const geoNetRec& a,const geoNetRec& b)
{
    return a.endIp<b.endIp;
}


class I_GEOIP: public Ifaces::Base
{
public:
    virtual bool findNetRec(IInstance* instance,const std::string &ip, bool isV4, geoNetRec &result)=0;

};

#endif

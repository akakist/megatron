#ifndef ___________________ST_RSA_IMPL_H
#define ___________________ST_RSA_IMPL_H

#include <stdint.h>
#include <IGEOIP.h>
#include "CapsGeoIP.h"

class C_GEOIP: public I_GEOIP, public Mutexable
{
public:
    bool inited;
    CapsGeoIP geoip;

    bool findNetRec(const std::string &ip, bool isV4, geoNetRec &result);
    void init();

    C_GEOIP():inited(false)
    {

    }

};



#endif // ST_RSA_IMPL_H

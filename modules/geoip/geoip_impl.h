#ifndef ___________________ST_RSA_IMPL_H
#define ___________________ST_RSA_IMPL_H

#include <stdint.h>
#include "commonError.h"
#include "ioBuffer.h"
#include "IGEOIP.h"
#include "CapsGeoIP.h"
#include "mutexable.h"

class C_GEOIP: public I_GEOIP, public Mutexable
{
public:
    bool inited;
    CapsGeoIP geoip;

    bool findNetRec(IInstance* instance,const std::string &ip, bool isV4, geoNetRec &result);

    C_GEOIP():inited(false)
    {

    }

};



#endif // ST_RSA_IMPL_H

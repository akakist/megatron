#include "geoip_impl.h"
#include "configDB.h"
#ifdef WIN32
#include <malloc.h>
#endif
#include "mutexInspector.h"
#include "st_rsa.h"
#include "st_malloc.h"

bool C_GEOIP::findNetRec(IInstance* instance,const std::string &ip, bool isV4, geoNetRec &result)
{
    M_LOCK(*this);
    if(!inited)
    {
        geoip.init(instance);
        inited=true;
    }
    return geoip.findNetRec(ip,isV4,result);
}

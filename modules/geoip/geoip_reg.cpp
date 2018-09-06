#include <string>
#include "IInstance.h"
//#include "st_rsa_impl.h"
#include "version_mega.h"
#include "geoip_impl.h"

void registerGEOIP(const char* pn)
{

    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_IFACE,(int)Ifaces::GEOIP,"GEOIP");
    }
    else
    {
        I_GEOIP *GEOIP=new C_GEOIP;
        iUtils->registerIface(COREVERSION,(int)Ifaces::GEOIP,GEOIP);
    }
}

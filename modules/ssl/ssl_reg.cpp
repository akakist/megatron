#include <string>
#include "IInstance.h"
#include "st_rsa_impl.h"
#include "version_mega.h"

void registerSSL(const char* pn)
{

    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_IFACE,Ifaces::SSL,"SSL",std::set<EVENT_id>());
    }
    else
    {
        I_ssl *ssl=new C_ssl;
        iUtils->registerIface(COREVERSION,Ifaces::SSL,ssl);
    }
}

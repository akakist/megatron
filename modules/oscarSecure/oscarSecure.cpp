#include "IUtils.h"
IUtils *iUtils;
void registerOscarSecureModule(const char*);
extern "C" void
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f, const char* pn)
{

    iUtils=f;
    registerOscarSecureModule(pn);
}


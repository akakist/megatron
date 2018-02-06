#include <string>
#include <map>
#include "SERVICE_id.h"
#include "objectProxyService.h"

IUtils *iUtils;
void registerObjectProxyModule(const char*);

extern "C" void
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f, const char* pn)
{

    iUtils=f;
    registerObjectProxyModule(pn);
}


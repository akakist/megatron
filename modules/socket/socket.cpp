#include "socketService.h"
#include <string>
#include <map>
#include "SERVICE_id.h"

IUtils *iUtils;
void registerSocketModule(const char*);
extern "C" void

#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f, const char* pn)
{

    iUtils=f;
    registerSocketModule(pn);
}

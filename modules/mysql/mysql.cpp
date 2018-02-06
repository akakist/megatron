#include <string>
//..#include <map>
//#include "SERVICE_id.h"
//#include "systemEvent.h"
//#include "mysqlService.h"
#include "IUtils.h"
IUtils *iUtils;
void registerMysqlModule(const char*pn);

extern "C" void
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f, const char*pn)
{

    iUtils=f;
    registerMysqlModule(pn);
}


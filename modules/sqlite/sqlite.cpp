#include <string>
#include <map>
#include "SERVICE_id.h"
#include "IUtils.h"
#include "sqliteService.h"
IUtils *iUtils;
void registerSqliteModule();

extern "C" void
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f)
{

    iUtils=f;
    registerSqliteModule();
}


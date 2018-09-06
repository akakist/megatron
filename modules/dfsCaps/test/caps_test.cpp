#include "IUtils.h"

extern IUtils *iUtils;
void registerCapsTest(const char*);
extern "C" void
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f, const char* pn)
{

    iUtils=f;
    registerCapsTest(pn);
}


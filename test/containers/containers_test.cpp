#include "IUtils.h"

extern IUtils *iUtils;
void registerCTRTest(const char*);
extern "C" void
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f, const char* pn)
{

    iUtils=f;
    registerCTRTest(pn);
}


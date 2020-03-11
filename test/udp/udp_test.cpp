#include "IUtils.h"

extern IUtils *iUtils;
void registerUDPTest(const char*);
extern "C" void
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IUtils* f, const char* pn)
{

    iUtils=f;
    registerUDPTest(pn);
}


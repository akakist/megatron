#include "IInstance.h"

IInstance *IInstance;
void registerDFSBankService(const char* pn);
extern "C" void
#ifdef _WIN32
__declspec(dllexport)
#endif
#ifdef DEBUG
registerModuleDebug
#else
registerModule
#endif
(IInstance* f,const char* pn)
{

    IInstance=f;
    registerDFSBankService(pn);
}


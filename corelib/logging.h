#ifndef _______________LOGGING____H
#define _______________LOGGING____H
#include "unknown.h"
#include "mutexable.h"
#include "event.h"
#include "IInstance.h"
class st_log
{

public:
    st_log(const std::string& keyword)
    {
        XTRY;
        iUtils->pushLogPrefix(keyword);
        XPASS;
    }
    ~st_log()
    {
        iUtils->popLogPrefix();
    }
};
#ifdef WITH_SLOG
#define S_LOG(a) st_log aaazzz(a)
#else
#define S_LOG(a)
#endif
#endif

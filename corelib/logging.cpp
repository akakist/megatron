#include "logging.h"
#include "IInstance.h"
#include <stdarg.h>
void Logging::log(LOGTYPE lt,const char *fmt, ...)
{
    if(lt>loglevel)
        return;
    va_list ap;
    char str[1024*10];
    va_start(ap, fmt);
    vsnprintf(str, sizeof(str), fmt, ap);
    va_end(ap);
    std::string kwd;
    {
        M_LOCK(mx);
        kwd=iUtils->join("|",msgs[pthread_self()]);
//#endif
    }
    logErr2("%s: %s @ %s",unknown->classname.c_str(),kwd.c_str(),str);
}

void Logging::push_back(const std::string& s)
{
    M_LOCK(mx);
    msgs[pthread_self()].push_back(s);
}
void Logging::pop_back()
{
    M_LOCK(mx);
    msgs[pthread_self()].pop_back();
}


void Logging::sendEventAndLog(const SERVICE_id& svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    ifa->sendEvent(svs,e);
#ifdef DEBUG
    if(loglevel>=TRACE_log)
    {
        log(loglevel,"sendEventAndLog to %s %s %s",iUtils->serviceName(svs).c_str(),e->name,e->dump().c_str());
    }
#endif
    XPASS;
}
void Logging::sendEventAndLog(const msockaddr_in& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    ifa->sendEvent(addr,svs,e);
#ifdef DEBUG
    if(loglevel>=TRACE_log)
    {
        log(loglevel,"sendEventAndLog %s to %s %s %s",addr.dump().c_str(),iUtils->serviceName(svs).c_str(),e->name,e->dump().c_str());
    }
#endif
    XPASS;
}
void Logging::sendEventAndLog(const std::set<msockaddr_in>& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    if(addr.size()>1)
    {
        logErr2("FATAL: must remove void Logging::sendEventAndLog(const std::set<msockaddr_in>& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e)");
    }
    for(auto &i:addr)
        sendEventAndLog(i,svs,e);
    XPASS;
}
void Logging::sendEventAndLog(const std::string& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    ifa->sendEvent(addr,svs,e);
#ifdef DEBUG
    if(loglevel>=TRACE_log)
    {
        log(loglevel,"sendEventAndLog remote addr=%s to serv=%s name=%s dump=%s",addr.c_str(),iUtils->serviceName(svs).c_str(),e->name,e->dump().c_str());
    }
#endif
    XPASS;
}
void Logging::passEventAndLog(const REF_getter<Event::Base>&e)
{
    XTRY;
    ifa->passEvent(e);
#ifdef DEBUG
    if(loglevel>=TRACE_log)
    {
        log(loglevel,"passEventAndLog route=%s name=%s dump=%s",e->route.dump().c_str(),e->name,e->dump().c_str());
    }
#endif
    XPASS;
}

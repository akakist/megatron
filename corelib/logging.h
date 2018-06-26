#ifndef _______________LOGGING____H
#define _______________LOGGING____H
#include "unknown.h"
#include "mutexable.h"
#include "event.h"
#include "IInstance.h"
enum LOGTYPE
{
    ERROR_log,INFO_log
//#ifdef DEBUG
    ,TRACE_log, TRACE_ALL_EVENTS_log
//#endif
};
class Logging
{
    UnknownBase *unknown;
    Mutex mx;
    LOGTYPE loglevel;
    friend class st_log;
    std::map<pthread_t,std::deque<std::string> > msgs;
    void push_back(const std::string& s);
    void pop_back();
private:
    IInstance *ifa;
public:
    Logging(UnknownBase *_unknown,LOGTYPE l, IInstance* _if):unknown(_unknown),loglevel(l),ifa(_if) {}
    void log(LOGTYPE l,const char *fmt, ...);
    void sendEventAndLog(const SERVICE_id& svs, const REF_getter<Event::Base>&e);
    void sendEventAndLog(const msockaddr_in& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e);
    void sendEventAndLog(const std::set<msockaddr_in>& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e);
    void sendEventAndLog(const std::string& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e);
    void passEventAndLog(const REF_getter<Event::Base>&e);

};
class st_log
{
    Logging* l;

public:
    st_log(Logging* _l, const std::string& keyword):l(_l)
    {
        l->push_back(keyword);
    }
    ~st_log()
    {
        l->pop_back();
    }
};
#ifdef DEBUG
#define S_LOG(a) st_log aaazzz(this,a)
#else
#define S_LOG(a)
#endif
#endif

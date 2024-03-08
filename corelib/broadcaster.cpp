#include "broadcaster.h"
void Broadcaster::sendEvent(const SERVICE_id& svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    __BRD$ifa->sendEvent(svs,e);
    XPASS;
}
void Broadcaster::sendEvent( ListenerBase* svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    __BRD$ifa->sendEvent(svs,e);
    XPASS;
}
void Broadcaster::sendEvent(const msockaddr_in& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    __BRD$ifa->sendEvent(addr,svs,e);
    XPASS;
}
void Broadcaster::sendEvent(const std::set<msockaddr_in>& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    if(addr.size()>1)
    {
        logErr2("event dump: %s",e->dump().toStyledString().c_str());
        for(auto& z:addr)
        {
            logErr2("addr %s",z.dump().c_str());
        }
        logErr2("FATAL: must remove void Broadcaster::sendEvent(const std::set<msockaddr_in>& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e)");
    }
    for(auto &i:addr)
    {

        __BRD$ifa->sendEvent(i,svs,e);
    }
    XPASS;
}
void Broadcaster::sendEvent(const std::string& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e)
{
    XTRY;
    __BRD$ifa->sendEvent(addr,svs,e);
    XPASS;
}

void Broadcaster::passEvent(const REF_getter<Event::Base>&e)
{
    XTRY;
    __BRD$ifa->passEvent(e);
    XPASS;
}

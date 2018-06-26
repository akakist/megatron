#ifndef _______________BROADCASTER____H
#define _______________BROADCASTER____H
#include "SERVICE_id.h"
#include "REF.h"
#include "event.h"
#include "IInstance.h"
#include "route_t.h"
class Broadcaster
{
public:
    Broadcaster(IInstance* _if):__BRD$ifa(_if) {}
    void sendEvent(const SERVICE_id& svs, const REF_getter<Event::Base>&e);
    void sendEvent(const msockaddr_in& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e);
    void sendEvent(const std::set<msockaddr_in>& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e);
    void sendEvent(const std::string& addr, const SERVICE_id& svs, const REF_getter<Event::Base>&e);
    void passEvent(const REF_getter<Event::Base>&e);
    IInstance* getIInstance()
    {
        return __BRD$ifa;
    }

private:
    IInstance* __BRD$ifa;

};
#endif

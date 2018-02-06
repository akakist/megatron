#ifndef _______HTTTP___SERVICE____EVENT_______H11
#define _______HTTTP___SERVICE____EVENT_______H11

#include "___httpEvent.h"
namespace httpEvent
{
class DoListen: public Event::NoPacked
{
    /**
    request HTTP to listen port
    line like "INADDR_ANY:8080:128" - ip:port:listenBacklog
    */
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    DoListen(const msockaddr_in& _addr, const route_t & r)
        :NoPacked(httpEventEnum::DoListen,"httpDoListen",r),
         addr(_addr) {}
    const msockaddr_in addr;
    void jdump(Json::Value &) const
    {
    }

};


}

#endif

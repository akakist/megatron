#ifndef _______HTTTP___SERVICE____EVENT_______H3
#define _______HTTTP___SERVICE____EVENT_______H3

#include "___httpEvent.h"
namespace httpEvent
{

class GetBindPortsRSP: public Event::Base
{
    enum {channel=CHANNEL_100};

public:
    static Base* construct(const route_t &r)
    {
        return new GetBindPortsRSP(r);
    }
    GetBindPortsRSP(const route_t& r)
        :Base(httpEventEnum::GetBindPortsRSP,channel,"GetBindPortsRSP",r) {}
    GetBindPortsRSP(const std::set<msockaddr_in>& _ListenAdr,  const route_t &r)
        :Base(httpEventEnum::GetBindPortsRSP,channel,"GetBindPortsRSP",r),
         listenAddrs(_ListenAdr) {}
    std::set<msockaddr_in> listenAddrs;

    void unpack(inBuffer& o)
    {
        o>>listenAddrs;
    }
    void pack(outBuffer&o) const
    {
        o<<listenAddrs;
    }
    void jdump(Json::Value &v) const
    {
        v["listenAddrs"]=iUtils->dump(listenAddrs);
    }

};

}

#endif

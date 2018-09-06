#ifndef _______HTTTP___SERVICE____EVENT_______H2
#define _______HTTTP___SERVICE____EVENT_______H2

#include "___httpEvent.h"
namespace httpEvent
{

class GetBindPortsREQ: public Event::Base
{
    enum {rpcChannel=CHANNEL_100};

public:
    static Base* construct(const route_t &r)
    {
        return new GetBindPortsREQ(r);
    }
    GetBindPortsREQ(const route_t& r)
        :Base(httpEventEnum::GetBindPortsREQ,rpcChannel,"GetBindPortsREQ",r) {}

    void unpack(inBuffer& )
    {
    }
    void pack(outBuffer&) const
    {
    }
    void jdump(Json::Value &) const
    {
    }
};
class GetBindPortsRSP: public Event::Base
{
    enum {rpcChannel=CHANNEL_100};

public:
    static Base* construct(const route_t &r)
    {
        return new GetBindPortsRSP(r);
    }
    GetBindPortsRSP(const route_t& r)
        :Base(httpEventEnum::GetBindPortsRSP,rpcChannel,"GetBindPortsRSP",r) {}
    GetBindPortsRSP(const std::set<msockaddr_in>& _ListenAdr,  const route_t &r)
        :Base(httpEventEnum::GetBindPortsRSP,rpcChannel,"GetBindPortsRSP",r),
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

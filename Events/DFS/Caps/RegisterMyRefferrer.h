#ifndef _________dfsCapsEvent_h1z1
#define _________dfsCapsEvent_h1z1
#include "___dfsCapsEvent.h"
#include <set>
#include "msockaddr_in.h"
#include "event.h"
namespace dfsCapsEvent {
class RegisterMyRefferrerREQ: public Event::Base
{
    enum {channel=CHANNEL_70};


public:

    static Base* construct(const route_t &r)
    {
        return new RegisterMyRefferrerREQ(r);
    }

    RegisterMyRefferrerREQ(const std::set<msockaddr_in> &sas, const route_t& r)
        :Base(dfsCapsEventEnum::RegisterMyRefferrerREQ,channel,"RegisterMyRefferrerREQ",r), externalListenAddr(sas)
    {

    }

    RegisterMyRefferrerREQ(const route_t& r)
        :Base(dfsCapsEventEnum::RegisterMyRefferrerREQ,channel,"RegisterMyRefferrerREQ",r)
    {

    }

    std::set<msockaddr_in> externalListenAddr;

    void unpack(inBuffer& o)
    {
        o>>externalListenAddr;
    }
    void pack(outBuffer&o) const
    {
        o<<externalListenAddr;
    }
    void jdump(Json::Value &v) const
    {
        v["externalListenAddr"]=iUtils->dump(externalListenAddr);
    }

};
}

#endif

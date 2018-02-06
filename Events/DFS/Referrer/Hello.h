#ifndef _________dfsReferrerEvent_h14
#define _________dfsReferrerEvent_h14
#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
    class Hello: public Event::Base
    {
        enum {channel=CHANNEL_100};

    public:
        static Base* construct(const route_t &r)
        {
            return new Hello(r);
        }
        Hello(const route_t& r)
            :Base(dfsReferrerEventEnum::Hello,channel,"Hello",r) {}
        Hello(const unsigned short &_externalListenPort,const std::set<msockaddr_in>& _internalListenAdr,  const route_t &r)
            :Base(dfsReferrerEventEnum::Hello,channel,"Hello",r),
             externalListenPort(_externalListenPort),
             internalListenAdr(_internalListenAdr) {}
        unsigned short externalListenPort;
        std::set<msockaddr_in> internalListenAdr;

        void unpack(inBuffer& o)
        {
            
            o>>externalListenPort>>internalListenAdr;
        }
        void pack(outBuffer&o) const
        {
            
            o<<externalListenPort<<internalListenAdr;
        }
        void jdump(Json::Value &v) const
        {
            v["externalListenPort"]=externalListenPort;
            v["internalListenAdr"]=iUtils->dump(internalListenAdr);
        }

    };

}
#endif

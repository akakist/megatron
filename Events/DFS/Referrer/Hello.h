#ifndef _________dfsReferrerEvent_h14Z1
#define _________dfsReferrerEvent_h14Z1
#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
    class Hello: public Event::Base
    {
        enum {rpcChannel=CHANNEL_100};

    public:
        static Base* construct(const route_t &r)
        {
            return new Hello(r);
        }
        Hello(const route_t& r)
            :Base(dfsReferrerEventEnum::Hello,rpcChannel,r) {}
        Hello(const unsigned short &_externalListenPort,const std::set<msockaddr_in>& _internalListenAdr,  const route_t &r)
            :Base(dfsReferrerEventEnum::Hello,rpcChannel,r),
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
    class Elloh: public Event::Base
    {
        /// данное событие идет в обратную сторону в клиента, который пытался подцепиться для проверки, что он доступен.
        enum {rpcChannel=CHANNEL_100};

    public:
        static Base* construct(const route_t &r)
        {
            return new Elloh(r);
        }
        Elloh(const route_t& r)
            :Base(dfsReferrerEventEnum::Elloh,rpcChannel,r) {}
        Elloh(const unsigned short &_externalListenPort,  const route_t &r)
            :Base(dfsReferrerEventEnum::Elloh,rpcChannel,r),
             externalListenPort(_externalListenPort) {}
        unsigned short externalListenPort;
        void unpack(inBuffer& o)
        {

            o>>externalListenPort;
        }
        void pack(outBuffer&o) const
        {

            o<<externalListenPort;
        }
        void jdump(Json::Value &v) const
        {
            v["externalListenPort"]=externalListenPort;
        }
    };

}
#endif

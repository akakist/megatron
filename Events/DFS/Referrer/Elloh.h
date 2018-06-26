#ifndef _________dfsReferrerEvent_h6
#define _________dfsReferrerEvent_h6
#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
    class Elloh: public Event::Base
    {
        enum {channel=CHANNEL_100};

    public:
        static Base* construct(const route_t &r)
        {
            return new Elloh(r);
        }
        Elloh(const route_t& r)
            :Base(dfsReferrerEventEnum::Elloh,channel,"Elloh",r) {}
        Elloh(const unsigned short &_externalListenPort,  const route_t &r)
            :Base(dfsReferrerEventEnum::Elloh,channel,"Elloh",r),
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

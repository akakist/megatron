#ifndef _________dfsCapsEvent_h1z112
#define _________dfsCapsEvent_h1z112
#include "___dfsCapsEvent.h"
#include <set>
#include "msockaddr_in.h"
#include "event.h"
namespace dfsCapsEvent {
    class RegisterCloudRoot: public Event::Base
    {
        enum {rpcChannel=CHANNEL_70};


    public:

        static Base* construct(const route_t &r)
        {
            return new RegisterCloudRoot(r);
        }

        RegisterCloudRoot(const std::set<msockaddr_in> &my_sas, const msockaddr_in& _uplink,  const route_t& r)
            :Base(dfsCapsEventEnum::RegisterCloudRoot,rpcChannel,r), externalListenAddr(my_sas), uplink(_uplink)
        {

        }

        RegisterCloudRoot(const route_t& r)
            :Base(dfsCapsEventEnum::RegisterCloudRoot,rpcChannel,r)
        {

        }

        std::set<msockaddr_in> externalListenAddr;
        msockaddr_in uplink;

        void unpack(inBuffer& o)
        {
            o>>externalListenAddr>>uplink;
        }
        void pack(outBuffer&o) const
        {
            o<<externalListenAddr<<uplink;
        }
        void jdump(Json::Value &v) const
        {
            v["externalListenAddr"]=iUtils->dump(externalListenAddr);

            v["uplink"]=uplink.dump();
        }

    };
}

#endif

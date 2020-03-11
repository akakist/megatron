#ifndef _________dfsCapsEvent_h1z1
#define _________dfsCapsEvent_h1z1
#include "___dfsCapsEvent.h"
#include <set>
#include "msockaddr_in.h"
#include "event.h"
namespace dfsCapsEvent {
    class RegisterMyRefferrerNodeREQ: public Event::Base
    {
        enum {rpcChannel=CHANNEL_70};


    public:

        static Base* construct(const route_t &r)
        {
            return new RegisterMyRefferrerNodeREQ(r);
        }

        RegisterMyRefferrerNodeREQ(const std::set<msockaddr_in> &my_sas, const msockaddr_in& _uplink, int _downlinkCount, const route_t& r)
            :Base(dfsCapsEventEnum::RegisterMyRefferrerNodeREQ,rpcChannel,r), externalListenAddr(my_sas),uplink(_uplink),downlinkCount(_downlinkCount)
        {

        }

        RegisterMyRefferrerNodeREQ(const route_t& r)
            :Base(dfsCapsEventEnum::RegisterMyRefferrerNodeREQ,rpcChannel,r)
        {

        }

        std::set<msockaddr_in> externalListenAddr;
        msockaddr_in uplink;
        int downlinkCount;

        void unpack(inBuffer& o)
        {
            o>>externalListenAddr>>uplink>>downlinkCount;
        }
        void pack(outBuffer&o) const
        {
            o<<externalListenAddr<<uplink<<downlinkCount;
        }
        void jdump(Json::Value &v) const
        {
            v["externalListenAddr"]=iUtils->dump(externalListenAddr);
            v["uplink"]=uplink.dump();
        }

    };
}

#endif

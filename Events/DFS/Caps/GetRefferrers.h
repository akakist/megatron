#ifndef _________dfsCapsEvent_h1z12
#define _________dfsCapsEvent_h1z12
#include "___dfsCapsEvent.h"
#include <set>
#include "msockaddr_in.h"
#include "event.h"
namespace dfsCapsEvent {
    class GetReferrersREQ: public Event::Base
    {
        enum {rpcChannel=CHANNEL_70};


    public:

        static Base* construct(const route_t &r)
        {
            return new GetReferrersREQ(r);
        }

        GetReferrersREQ(const std::set<msockaddr_in> &sas, const route_t& r)
            :Base(dfsCapsEventEnum::GetReferrersREQ,rpcChannel,r), externalListenAddr(sas)
        {

        }

        GetReferrersREQ(const route_t& r)
            :Base(dfsCapsEventEnum::GetReferrersREQ,rpcChannel,r)
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
    class GetReferrersRSP: public Event::Base
    {
        enum {rpcChannel=CHANNEL_70};


    public:

        static Base* construct(const route_t &r)
        {
            return new GetReferrersRSP(r);
        }

        GetReferrersRSP(const std::vector<msockaddr_in> &sas, const route_t& r)
            :Base(dfsCapsEventEnum::GetReferrersRSP,rpcChannel,r), referrer_addresses(sas)
        {

        }

        GetReferrersRSP(const route_t& r)
            :Base(dfsCapsEventEnum::GetReferrersRSP,rpcChannel,r)
        {

        }

        std::vector<msockaddr_in> referrer_addresses;

        void unpack(inBuffer& o)
        {
            o>>referrer_addresses;
        }
        void pack(outBuffer&o) const
        {
            o<<referrer_addresses;
        }
        void jdump(Json::Value &v) const
        {
            v["externalListenAddr"]=iUtils->dump(referrer_addresses);
        }

    };
}

#endif

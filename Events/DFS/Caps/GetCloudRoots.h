#ifndef _________dfsCapsEvent_h1z1211
#define _________dfsCapsEvent_h1z1211
#include "___dfsCapsEvent.h"
#include <set>
#include "msockaddr_in.h"
#include "event.h"
namespace dfsCapsEvent {
    class GetCloudRootsREQ: public Event::Base
    {
        enum {rpcChannel=CHANNEL_70};


    public:

        static Base* construct(const route_t &r)
        {
            return new GetCloudRootsREQ(r);
        }

        GetCloudRootsREQ(const std::set<msockaddr_in> &sas, const route_t& r)
            :Base(dfsCapsEventEnum::GetCloudRootsREQ,rpcChannel,r), externalListenAddr(sas)
        {

        }

        GetCloudRootsREQ(const route_t& r)
            :Base(dfsCapsEventEnum::GetCloudRootsREQ,rpcChannel,r)
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
    class GetCloudRootsRSP: public Event::Base
    {
        enum {rpcChannel=CHANNEL_70};


    public:

        static Base* construct(const route_t &r)
        {
            return new GetCloudRootsRSP(r);
        }

        GetCloudRootsRSP(const std::vector<msockaddr_in> &sas, const route_t& r)
            :Base(dfsCapsEventEnum::GetCloudRootsRSP,rpcChannel,r), referrer_addresses(sas)
        {

        }

        GetCloudRootsRSP(const route_t& r)
            :Base(dfsCapsEventEnum::GetCloudRootsRSP,rpcChannel,r)
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

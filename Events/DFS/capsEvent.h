#pragma once

#include <string>
#include "IUtils.h"
#include "SERVICE_id.h"
#include "EVENT_id.h"
#include "event_mt.h"
namespace ServiceEnum
{
    const SERVICE_id DFSCaps(genum_DFSCaps);
}

namespace dfsCapsEventEnum
{
    const EVENT_id RegisterMyRefferrerNodeREQ(genum_DFSCaps_RegisterMyRefferrerNodeREQ);
    const EVENT_id RegisterCloudRoot(genum_DFSCaps_RegisterCloudRoot);
    const EVENT_id GetReferrersREQ(genum_DFSCaps_GetReferrersREQ);
    const EVENT_id GetReferrersRSP(genum_DFSCaps_GetReferrersRSP);
    const EVENT_id GetCloudRootsREQ(genum_DFSCaps_GetCloudRootsREQ);
    const EVENT_id GetCloudRootsRSP(genum_DFSCaps_GetCloudRootsRSP);
}
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
            :Base(dfsCapsEventEnum::RegisterMyRefferrerNodeREQ,r), externalListenAddr(my_sas),uplink(_uplink),downlinkCount(_downlinkCount)
        {

        }

        RegisterMyRefferrerNodeREQ(const route_t& r)
            :Base(dfsCapsEventEnum::RegisterMyRefferrerNodeREQ,r)
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
    class RegisterCloudRoot: public Event::Base
    {
        enum {rpcChannel=CHANNEL_70};


    public:

        static Base* construct(const route_t &r)
        {
            return new RegisterCloudRoot(r);
        }

        RegisterCloudRoot(const std::set<msockaddr_in> &my_sas, const msockaddr_in& _uplink,  const route_t& r)
            :Base(dfsCapsEventEnum::RegisterCloudRoot,r), externalListenAddr(my_sas), uplink(_uplink)
        {

        }

        RegisterCloudRoot(const route_t& r)
            :Base(dfsCapsEventEnum::RegisterCloudRoot,r)
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
            :Base(dfsCapsEventEnum::GetReferrersREQ,r), externalListenAddr(sas)
        {

        }

        GetReferrersREQ(const route_t& r)
            :Base(dfsCapsEventEnum::GetReferrersREQ,r)
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
            :Base(dfsCapsEventEnum::GetReferrersRSP,r), referrer_addresses(sas)
        {

        }

        GetReferrersRSP(const route_t& r)
            :Base(dfsCapsEventEnum::GetReferrersRSP,r)
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
            :Base(dfsCapsEventEnum::GetCloudRootsREQ,r), externalListenAddr(sas)
        {

        }

        GetCloudRootsREQ(const route_t& r)
            :Base(dfsCapsEventEnum::GetCloudRootsREQ,r)
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
            :Base(dfsCapsEventEnum::GetCloudRootsRSP,r), referrer_addresses(sas)
        {

        }

        GetCloudRootsRSP(const route_t& r)
            :Base(dfsCapsEventEnum::GetCloudRootsRSP,r)
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

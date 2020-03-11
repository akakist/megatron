#ifndef _________dfsReferrerEvent_h17NotifyReferrerDownlinkConnectedZ1
#define _________dfsReferrerEvent_h17NotifyReferrerDownlinkConnectedZ1
#include "___dfsReferrerEvent.h"
#include "linkinfo.h"
namespace dfsReferrerEvent {
    class NotifyReferrerDownlinkDisconnected: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyReferrerDownlinkDisconnected(const SOCKET_id& _sid, const route_t& r)
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerDownlinkDisconnected,r),socketId(_sid) {}

        const SOCKET_id& socketId;
        void jdump(Json::Value &j) const
        {
            j["socketId"]=std::to_string(CONTAINER(socketId));
        }
    };
    class NotifyReferrerUplinkIsConnected: public Event::NoPacked
    {
        /**
            Clean old uplinks and replace by new
          */
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyReferrerUplinkIsConnected(const msockaddr_in& _sa,const route_t& r)
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerUplinkIsConnected,r),sa(_sa) {}

        msockaddr_in sa;
        void jdump(Json::Value &j) const
        {
            j["sa"]=sa.dump();
        }
    };
    class NotifyReferrerUplinkIsDisconnected: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyReferrerUplinkIsDisconnected(const msockaddr_in& _sa,const route_t& r)
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerUplinkIsDisconnected,r), sa(_sa) {}

        msockaddr_in sa;
        void jdump(Json::Value &j) const
        {
            j["sa"]=sa.dump();
        }

    };

}
#endif

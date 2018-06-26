#ifndef _________dfsReferrerEvent_h17NotifyReferrerDownlinkConnected
#define _________dfsReferrerEvent_h17NotifyReferrerDownlinkConnected
#include "___dfsReferrerEvent.h"
#include "linkinfo.h"
namespace dfsReferrerEvent {
    class NotifyReferrerDownlinkConnected: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyReferrerDownlinkConnected(const REF_getter<dfsReferrer::linkInfoDownReferrer>& _l,const route_t& r)
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerDownlinkConnected,"NotifyReferrerDownlinkConnected",r),l(_l) {}

        REF_getter<dfsReferrer::linkInfoDownReferrer> l;
        void jdump(Json::Value &) const
        {
        }
    };
    class NotifyReferrerDownlinkDisconnected: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyReferrerDownlinkDisconnected(const SOCKET_id& _sid, const route_t& r)
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerDownlinkDisconnected,"NotifyReferrerDownlinkDisconnected",r),socketId(_sid) {}

        const SOCKET_id& socketId;
        void jdump(Json::Value &) const
        {
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
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerUplinkIsConnected,"NotifyReferrerUplinkIsConnected",r),sa(_sa) {}

        msockaddr_in sa;
        void jdump(Json::Value &) const
        {
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
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerUplinkIsDisconnected,"NotifyReferrerUplinkIsDisconnected",r), sa(_sa) {}

        msockaddr_in sa;
        void jdump(Json::Value &) const
        {
        }

    };

}
#endif

#ifndef _________dfsReferrerEvent_h17
#define _________dfsReferrerEvent_h17
#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
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

}
#endif

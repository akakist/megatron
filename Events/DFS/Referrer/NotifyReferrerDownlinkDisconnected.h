#ifndef _________dfsReferrerEvent_h17NotifyReferrerDownlinkDisconnected
#define _________dfsReferrerEvent_h17NotifyReferrerDownlinkDisconnected
#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
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

}
#endif

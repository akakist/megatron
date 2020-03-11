#ifndef _________dfsReferrerEvent_hSubscribeNotificationsZ1
#define _________dfsReferrerEvent_hSubscribeNotificationsZ1
#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
    class SubscribeNotifications: public Event::Base
    {
        enum {rpcChannel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {
            return new SubscribeNotifications(r);
        }
        SubscribeNotifications(const route_t& r)
            :Base(dfsReferrerEventEnum::SubscribeNotifications,rpcChannel,r) {}

        void jdump(Json::Value &) const
        {
        }
        void unpack(inBuffer&)
        {
        }
        void pack(outBuffer&) const
        {
        }
    };


}
#endif

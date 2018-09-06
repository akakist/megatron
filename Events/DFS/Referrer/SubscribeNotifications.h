#ifndef _________dfsReferrerEvent_hSubscribeNotifications
#define _________dfsReferrerEvent_hSubscribeNotifications
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
            :Base(dfsReferrerEventEnum::SubscribeNotifications,rpcChannel,"SubscribeNotifications",r) {}

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

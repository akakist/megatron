#ifndef _________dfsReferrerEvent_h1612
#define _________dfsReferrerEvent_h1612
#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
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

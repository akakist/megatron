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

}
#endif

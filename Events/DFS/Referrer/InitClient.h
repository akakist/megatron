#ifndef _________dfsReferrerEvent_h17InitClient
#define _________dfsReferrerEvent_h17InitClient
#include "___dfsReferrerEvent.h"
#include "linkinfo.h"
namespace dfsReferrerEvent {
    class InitClient: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        InitClient(const std::set<msockaddr_in>& _caps)
            :NoPacked(dfsReferrerEventEnum::InitClient,"InitClient"),caps(_caps) {}

        std::set<msockaddr_in>caps;
        void jdump(Json::Value &) const
        {
        }
    };

}
#endif

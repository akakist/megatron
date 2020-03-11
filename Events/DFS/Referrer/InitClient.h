#ifndef _________dfsReferrerEvent_h17InitClientZ1
#define _________dfsReferrerEvent_h17InitClientZ1
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
            :NoPacked(dfsReferrerEventEnum::InitClient),caps(_caps) {}

        std::set<msockaddr_in>caps;
        void jdump(Json::Value &j) const
        {
            for(auto& z:caps)
            {
                j["caps"].append(z.dump());
            }
        }
    };

}
#endif

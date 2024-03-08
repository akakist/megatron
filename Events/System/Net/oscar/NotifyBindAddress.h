#ifndef ______OSCAR_EVENT___H7
#define ______OSCAR_EVENT___H7
#include "___oscarEvent.h"
#include "msockaddr_in.h"
#include "event_mt.h"
namespace oscarEvent
{
    class NotifyBindAddress: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyBindAddress(const msockaddr_in & __S,const char* _socketDescription,bool _rebind, const route_t&r)
            :NoPacked(oscarEventEnum::NotifyBindAddress,r),
             addr(__S),socketDescription(_socketDescription),rebind(_rebind)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const msockaddr_in  addr;
        const char* socketDescription;
        const bool rebind;
    };

}
#endif

#ifndef ____EVT_SOCKET_OPEN__H___NBA
#define ____EVT_SOCKET_OPEN__H___NBA

#include "_________socketEvent.h"
#include "event_mt.h"
namespace socketEvent
{
    class NotifyBindAddress: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyBindAddress(const msockaddr_in & __S,const char* _socketDescription,bool _rebind,const route_t&r)
            :NoPacked(socketEventEnum::NotifyBindAddress,r),
             addr(__S),socketDescription(_socketDescription),rebind(_rebind)
        {
        }
        void jdump(Json::Value &j) const
        {
//            j["local_name"]=esi->local_name.jdump();
            j["socketDescription"]=socketDescription;
        }
        const msockaddr_in  addr;
        const char* socketDescription;
        const bool rebind;
    };
}


#endif

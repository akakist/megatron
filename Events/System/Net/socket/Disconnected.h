#ifndef ____EVT_SOCKET_OPEN__H___34523Disconnected
#define ____EVT_SOCKET_OPEN__H___34523Disconnected
#include "_________socketEvent.h"

namespace socketEvent
{
    class Disconnected: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Disconnected(const REF_getter<epoll_socket_info>  & __S,const std::string& _reason,const route_t&r)
            :NoPacked(socketEventEnum::Disconnected,r),
             esi(__S),reason(_reason)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const REF_getter<epoll_socket_info>  esi;
        const std::string reason;

    };
}


#endif

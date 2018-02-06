#ifndef ____EVT_SOCKET_OPEN__H___3452
#define ____EVT_SOCKET_OPEN__H___3452
#include "_________socketEvent.h"

namespace socketEvent
{
class Connected: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Connected(const REF_getter<epoll_socket_info>  & __S,const route_t&r)
        :NoPacked(socketEventEnum::Connected,"socketConnected",r),
         esi(__S)
    {
    }
    void jdump(Json::Value &) const
    {
    }
    const REF_getter<epoll_socket_info>  esi;

};
}


#endif

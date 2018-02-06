#ifndef ____EVT_SOCKET_OPEN__H___NBA
#define ____EVT_SOCKET_OPEN__H___NBA

#include "_________socketEvent.h"

namespace socketEvent
{
class NotifyBindAddress: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    NotifyBindAddress(const REF_getter<epoll_socket_info> & __S,const std::string _socketDescription,bool _rebind,const route_t&r)
        :NoPacked(socketEventEnum::NotifyBindAddress,"NotifyBindAddress",r),
         esi(__S),socketDescription(_socketDescription),rebind(_rebind)
    {
    }
    void jdump(Json::Value &) const
    {
    }
    const REF_getter<epoll_socket_info>  esi;
    const std::string socketDescription;
    const bool rebind;
};
}


#endif

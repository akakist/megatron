#ifndef ______OSCAR_EVENT___H7
#define ______OSCAR_EVENT___H7
#include "___oscarEvent.h"

namespace oscarEvent
{
class NotifyBindAddress: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    NotifyBindAddress(const REF_getter<epoll_socket_info> & __S,const std::string& _socketDescription,bool _rebind, const route_t&r)
        :NoPacked(oscarEventEnum::NotifyBindAddress,"NotifyBindAddress",r),
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

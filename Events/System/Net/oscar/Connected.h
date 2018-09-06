#ifndef ______OSCAR_EVENT___H6
#define ______OSCAR_EVENT___H6

#include "___oscarEvent.h"
namespace oscarEvent
{

/// callback notification about socket is connected
class Connected: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Connected(const REF_getter<epoll_socket_info>  & __S,const route_t&r)
        :NoPacked(oscarEventEnum::Connected,"oscarConnected",r),
         esi(__S)
    {
    }
    /// socket object
    const REF_getter<epoll_socket_info>  esi;
    void jdump(Json::Value &) const
    {
    }
};
}
#endif

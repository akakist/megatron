#ifndef ______OSCAR_EVENT___H1
#define ______OSCAR_EVENT___H1
#include "___oscarEvent.h"
namespace oscarEvent
{

/// callback notification about socket is accepted
class Accepted: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Accepted(const REF_getter<epoll_socket_info> &__S, const route_t& r):
        NoPacked(oscarEventEnum::Accepted,"oscarAccepted",r), esi(__S)
    {
    }
    /// socket object
    const REF_getter<epoll_socket_info> esi;
    void jdump(Json::Value &) const
    {
    }
};
}
#endif

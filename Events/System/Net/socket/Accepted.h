#ifndef ___ACCEPTEDEvent_H
#define ___ACCEPTEDEvent_H
#include "_________socketEvent.h"
namespace socketEvent{
class Accepted: public Event::NoPacked
{
public:
    Accepted(const REF_getter<epoll_socket_info> &__S, const route_t& r):
        NoPacked(socketEventEnum::Accepted,"socketAccepted",r), esi(__S)
    {
    }
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    const REF_getter<epoll_socket_info> esi;
    void jdump(Json::Value &) const
    {
    }
};
}

#endif

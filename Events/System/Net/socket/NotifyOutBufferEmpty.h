#ifndef ____EVT_SOCKET_OPEN__H___NOBE
#define ____EVT_SOCKET_OPEN__H___NOBE

#include "_________socketEvent.h"

namespace socketEvent
{
    class NotifyOutBufferEmpty: public Event::NoPacked
    {
    public:
        NotifyOutBufferEmpty(const REF_getter<epoll_socket_info> &__S, const route_t& r):
            NoPacked(socketEventEnum::NotifyOutBufferEmpty,r), esi(__S)
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

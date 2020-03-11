#ifndef ____EVT_SOCKET_OPEN__H___SR
#define ____EVT_SOCKET_OPEN__H___SR
#include "_________socketEvent.h"

namespace socketEvent
{
    class StreamRead: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        StreamRead(const REF_getter<epoll_socket_info>  & esi_, const route_t& r)
            :NoPacked(socketEventEnum::StreamRead,r),
             esi(esi_)
        {
        }
        void jdump(Json::Value &) const
        {
        }
        const REF_getter<epoll_socket_info>  esi;
    };
}


#endif

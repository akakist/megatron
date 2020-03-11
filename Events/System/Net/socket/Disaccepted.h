#ifndef ____EVT_SOCKET_OPEN__H___3452zDisaccepted
#define ____EVT_SOCKET_OPEN__H___3452zDisaccepted
#include "_________socketEvent.h"

namespace socketEvent
{
    class Disaccepted: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Disaccepted(const REF_getter<epoll_socket_info>  & __S,const std::string& _reason,const route_t&r)
            :NoPacked(socketEventEnum::Disaccepted,r),
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

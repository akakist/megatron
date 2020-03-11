#ifndef ____EVT_SOCKET_OPEN__H___1sfdf
#define ____EVT_SOCKET_OPEN__H___1sfdf
#include "_________socketEvent.h"

namespace socketEvent
{
    class ConnectFailed: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        ConnectFailed(const REF_getter<epoll_socket_info>& _esi,const msockaddr_in  &_addr,int _errno_,const route_t & r):
            NoPacked(socketEventEnum::ConnectFailed,r),
            esi(_esi),
            addr(_addr),_errno(_errno_) {}
        REF_getter<epoll_socket_info> esi;
        const msockaddr_in  addr;
        int _errno;
        void jdump(Json::Value &) const
        {
        }
    };

}


#endif

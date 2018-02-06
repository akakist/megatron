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
    ConnectFailed(const msockaddr_in  &_addr,const route_t & r):
        NoPacked(socketEventEnum::ConnectFailed,"SocketConnectFailed",r),
        addr(_addr) {}
    const msockaddr_in  addr;
    void jdump(Json::Value &) const
    {
    }
};

}


#endif

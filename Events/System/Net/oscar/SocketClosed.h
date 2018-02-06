#ifndef ______OSCAR_EVENT___Hc
#define ______OSCAR_EVENT___Hc
#include "___oscarEvent.h"
namespace oscarEvent
{

class SocketClosed: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    SocketClosed(const SOCKET_id& _sid,const route_t &r):
        NoPacked(oscarEventEnum::SocketClosed,"oscarDisconnected",r), socketId(_sid)
    {
    }
    /// socket object
    const SOCKET_id& socketId;
    void jdump(Json::Value &) const
    {
    }
};
}
#endif

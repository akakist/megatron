#ifndef ______OSCAR_EVENT___H2
#define ______OSCAR_EVENT___H2
#include "___oscarEvent.h"

namespace oscarEvent
{


/// скомандовать слушать ip/порт
class AddToListenTCP: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    AddToListenTCP(const SOCKET_id& _socketId, const msockaddr_in& _addr,const std::string& _socketDescription,const route_t & r):
        NoPacked(oscarEventEnum::AddToListenTCP,"oscarAddToListenTCP",r),
        socketId(_socketId), addr(_addr),socketDescription(_socketDescription) {}
    /// socket Id
    const SOCKET_id socketId;
    /// bind address
    const msockaddr_in addr;
    const std::string socketDescription;
    void jdump(Json::Value &) const
    {
    }
};
}
#endif

#ifndef ___RPC_EV1_H4o
#define ___RPC_EV1_H4o
#include "___rpcEvent.h"
namespace rpcEvent{
class SubscribeNotifications: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    SubscribeNotifications(const route_t & r)
        :NoPacked(rpcEventEnum::SubscribeNotifications,"rpcSubscribeNotifications",r)
    {}
    void jdump(Json::Value &) const
    {

    }
};
}
#endif

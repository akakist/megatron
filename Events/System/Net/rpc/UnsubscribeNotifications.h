#ifndef ___RPC_EV1_H4p
#define ___RPC_EV1_H4p
#include "___rpcEvent.h"
namespace rpcEvent{
class UnsubscribeNotifications: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    UnsubscribeNotifications(const route_t & r)
        :NoPacked(rpcEventEnum::UnsubscribeNotifications,"rpcUnsubscribeNotifications",r)
    {}
    void jdump(Json::Value &) const
    {

    }
};
}
#endif

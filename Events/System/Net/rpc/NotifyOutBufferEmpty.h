#ifndef ___RPC_EV1_H4w
#define ___RPC_EV1_H4w
#include "___rpcEvent.h"
namespace rpcEvent{
class NotifyOutBufferEmpty: public Event::NoPacked
{
public:
    NotifyOutBufferEmpty(const route_t& r)
        :NoPacked(rpcEventEnum::NotifyOutBufferEmpty,"rpcNotifyOutBufferEmpty",r)
    {
    }
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    void jdump(Json::Value &) const
    {
    }
};

}
#endif

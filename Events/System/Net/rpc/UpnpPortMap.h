#ifndef ___RPC_EV1_H4aa
#define ___RPC_EV1_H4aa
#include "___rpcEvent.h"
namespace rpcEvent{

class UpnpPortMap: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    std::string cookie;
    UpnpPortMap(const std::string& _cookie, const route_t& r)
        :NoPacked(rpcEventEnum::UpnpPortMap,"UpnpPortMap",r), cookie(_cookie) {}
    void jdump(Json::Value &) const
    {

    }
};
}
#endif

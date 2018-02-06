#ifndef ___RPC_EV1_H48
#define ___RPC_EV1_H48
#include "___rpcEvent.h"
namespace rpcEvent{

class Disconnect: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Disconnect(const REF_getter<epoll_socket_info>& _esi, const std::string& _reason)
        :NoPacked(rpcEventEnum::Disconnect,"rpcDisconnect"),
         esi(_esi),reason(_reason) {}
    const REF_getter<epoll_socket_info>  esi;
    const std::string reason;
    void jdump(Json::Value &) const
    {

    }
};
}
#endif

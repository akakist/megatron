#ifndef ___RPC_EV1_H6
#define ___RPC_EV1_H6
#include "___rpcEvent.h"
namespace rpcEvent{

class Disaccept: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Disaccept(const REF_getter<epoll_socket_info>& _esi, const std::string& _reason)
        :NoPacked(rpcEventEnum::Disaccept,"rpcDisaccept"),
         esi(_esi),reason(_reason) {}
    const REF_getter<epoll_socket_info>  esi;
    const std::string reason;
    void jdump(Json::Value &) const
    {

    }
};
}
#endif

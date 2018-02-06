#ifndef ___RPC_EV1_H49
#define ___RPC_EV1_H49
#include "___rpcEvent.h"
namespace rpcEvent{

class Disconnected: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Disconnected(const REF_getter<epoll_socket_info>& _esi, const msockaddr_in & _addr, const std::string& _reason, const route_t &r)
        :NoPacked(rpcEventEnum::Disconnected,"rpcDisconnected",r),
         esi(_esi),destination_addr(_addr), reason(_reason) {}
    const REF_getter<epoll_socket_info> esi;
    const msockaddr_in destination_addr;
    const std::string  reason;
    void jdump(Json::Value &) const
    {

    }
};

}
#endif

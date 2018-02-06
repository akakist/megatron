#ifndef ___RPC_EV1_H0
#define ___RPC_EV1_H0
#include "___rpcEvent.h"
namespace rpcEvent{
class Accepted: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Accepted(const REF_getter<epoll_socket_info>& _esi, const route_t &r)
        :NoPacked(rpcEventEnum::Accepted,"rpcAccepted",r),
         esi(_esi) {}
    const REF_getter<epoll_socket_info> esi;
    void jdump(Json::Value &) const
    {

    }
};

}
#endif

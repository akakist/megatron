#ifndef ___RPC_EV1_H47
#define ___RPC_EV1_H47
#include "___rpcEvent.h"
namespace rpcEvent{

class Disaccepted: public Event::NoPacked
{
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    Disaccepted(const REF_getter<epoll_socket_info>& _esi, const std::string& _reason, const route_t &r)
        :NoPacked(rpcEventEnum::Disaccepted,"rpcDisaccepted",r),
         esi(_esi),reason(_reason) {}
    const REF_getter<epoll_socket_info> esi;
    const std::string  reason;
    void jdump(Json::Value &) const
    {

    }

};
}
#endif

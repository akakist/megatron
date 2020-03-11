#ifndef ___RPC_EV1_H2
#define ___RPC_EV1_H2
#include "___rpcEvent.h"
namespace rpcEvent {

    class Binded: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Binded(const REF_getter<epoll_socket_info>& _esi)
            :NoPacked(rpcEventEnum::Binded),
             esi(_esi) {}
        const REF_getter<epoll_socket_info>  esi;
        void jdump(Json::Value &) const
        {

        }
    };
}
#endif

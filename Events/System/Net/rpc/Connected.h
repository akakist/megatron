#ifndef ___RPC_EV1_H4
#define ___RPC_EV1_H4
#include "___rpcEvent.h"
namespace rpcEvent {
    class Connected: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Connected(const REF_getter<epoll_socket_info>& _esi, const msockaddr_in & _addr, const route_t &r)
            :NoPacked(rpcEventEnum::Connected,r),
             esi(_esi),addr(_addr) {}
        const REF_getter<epoll_socket_info> esi;
        const msockaddr_in addr;
        void jdump(Json::Value &) const
        {

        }

    };

}
#endif

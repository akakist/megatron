#ifndef ___RPC_EV1_H4a
#define ___RPC_EV1_H4a
#include "___rpcEvent.h"
namespace rpcEvent {

    class IncomingOnAcceptor: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        IncomingOnAcceptor(const REF_getter<epoll_socket_info>&_esi,const REF_getter<Event::Base> & _e)
            :NoPacked(rpcEventEnum::IncomingOnAcceptor),
             esi(_esi),e(_e) {}
        const REF_getter<epoll_socket_info> esi;
        const REF_getter<Event::Base> e;
        void jdump(Json::Value &v) const
        {
            v["e"]=e->dump();
        }
    };
}
#endif

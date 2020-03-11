#ifndef ___RPC_EV1_H4q
#define ___RPC_EV1_H4q
#include "___rpcEvent.h"
namespace rpcEvent {
    class IncomingOnConnector: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        IncomingOnConnector(const REF_getter<epoll_socket_info>&_esi, const msockaddr_in& _addr,const REF_getter<Event::Base> & _e)
            :NoPacked(rpcEventEnum::IncomingOnConnector),
             esi(_esi),connect_addr(_addr),e(_e) {}
        const REF_getter<epoll_socket_info> esi;
        const msockaddr_in connect_addr;
        const REF_getter<Event::Base> e;
        void jdump(Json::Value &v) const
        {
            v["connect_addr"]=connect_addr.dump();
            v["e"]=e->dump();
        }
    };

}
#endif

#ifndef ___RPC_EV1_H3
#define ___RPC_EV1_H3
#include "___rpcEvent.h"
namespace rpcEvent {

    class ConnectFailed: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        ConnectFailed(const msockaddr_in & _addr, const route_t &r)
            :NoPacked(rpcEventEnum::ConnectFailed,r),
             destination_addr(_addr) {}
        const msockaddr_in destination_addr;
        void jdump(Json::Value &) const
        {

        }
    };
}
#endif

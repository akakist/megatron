#ifndef ___RPC_EV1_H4e
#define ___RPC_EV1_H4e
#include "___rpcEvent.h"
namespace rpcEvent {
    class PassPacket: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        PassPacket(const SOCKET_id& _addrTo, const REF_getter<Event::Base>  &_e)
            :NoPacked(rpcEventEnum::PassPacket),
             socketIdTo(_addrTo),e(_e) {}
        const SOCKET_id socketIdTo;
        const REF_getter<Event::Base> e;
        void jdump(Json::Value &j) const
        {
            j["socketIdTo"]=CONTAINER(socketIdTo);
            j["e"]=e->dump();
        }

    };

}
#endif

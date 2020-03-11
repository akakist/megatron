#ifndef ___RPC_EV1_H4i
#define ___RPC_EV1_H4i
#include "___rpcEvent.h"
namespace rpcEvent {


    class SendPacket: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        SendPacket(const msockaddr_in& _addrTo, const SERVICE_id& dst,const REF_getter<Event::Base>& _ev)
            :NoPacked(rpcEventEnum::SendPacket),
             addressTo(_addrTo),destination(dst), ev(_ev) {}
        const msockaddr_in addressTo;
        const SERVICE_id destination;
        const REF_getter<Event::Base> ev;
        void jdump(Json::Value &) const
        {

        }

    };

}
#endif

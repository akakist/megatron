#ifndef ___RPC_EV1_H4bb
#define ___RPC_EV1_H4bb
#include "___rpcEvent.h"
namespace rpcEvent {

    class UpnpResult: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        std::string cookie;
        UpnpResult(const std::string& _cookie, const route_t& r)
            :NoPacked(rpcEventEnum::UpnpResult,r), cookie(_cookie)  {}
        void jdump(Json::Value &) const
        {

        }
    };

}
#endif

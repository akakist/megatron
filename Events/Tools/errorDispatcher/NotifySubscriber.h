#ifndef _______________serviceErrorDispatcherEvent$h1
#define _______________serviceErrorDispatcherEvent$h1
#include "___errorDispatcherEvent.h"
namespace errorDispatcherEvent
{
class NotifySubscriber: public Event::Base
{
    enum {rpcChannel=CHANNEL_100};

public:
    static Base* construct(const route_t &r)
    {
        return new NotifySubscriber(r);
    }
    NotifySubscriber(const std::string& _opcode, const std::string& _msg, const route_t&r)
        :Base(errorDispatcherEventEnum::NotifySubscriber,rpcChannel,"errorDispatcherNotifySubscriber",r),opcode(_opcode),msg(_msg) {}
    NotifySubscriber(const route_t&r)
        :Base(errorDispatcherEventEnum::NotifySubscriber,rpcChannel,"errorDispatcherNotifySubscriber",r) {}
    std::string opcode;
    std::string msg;
    void unpack(inBuffer& o)
    {
        o>>opcode>>msg;
    }
    void pack(outBuffer&o) const
    {
        o<<opcode<<msg;
    }
    void jdump(Json::Value &) const
    {
    }


};


}
#endif

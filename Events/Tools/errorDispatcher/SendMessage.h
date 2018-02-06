#ifndef _______________serviceErrorDispatcherEvent$h2
#define _______________serviceErrorDispatcherEvent$h2
#include "___errorDispatcherEvent.h"
namespace errorDispatcherEvent
{
class SendMessage: public Event::Base
{
    enum {channel=CHANNEL_100};


public:
    static Base* construct(const route_t &r)
    {
        return new SendMessage(r);
    }
    SendMessage(const int &_opcode, const std::string& _msg)
        :Base(errorDispatcherEventEnum::SendMessage,channel,"errorDispatcherSendMessage"),opcode(_opcode),msg(_msg) {}
    SendMessage(const route_t&r)
        :Base(errorDispatcherEventEnum::SendMessage,channel,"errorDispatcherSendMessage",r) {}
    int opcode;
    std::string msg;
    void unpack(inBuffer& o)
    {
        o>>opcode>>msg;
    }
    void pack(outBuffer&o) const
    {
        o<<opcode<<msg;
    }
    void jdump(Json::Value &v) const
    {
        v["opcode"]=opcode;
        v["msg"]=msg;
    }

};


}
#endif

#pragma once
#include "EVENT_id.h"
#include "SERVICE_id.h"
#include "event_mt.h"
#include "route_t.h"







namespace ServiceEnum
{
    const SERVICE_id ErrorDispatcher("2dec@ErrorDispatcher");

}
namespace errorDispatcherEventEnum
{
    const EVENT_id SendMessage("51ab@e43@errorDispatcherSendMessage");
    const EVENT_id Subscribe("e1fc@errorDispatcherSubscribe");
    const EVENT_id Unsubscribe("97c9@errorDispatcherUnsubscribe");
    const EVENT_id NotifySubscriber("1de0@errorDispatcherNotifySubscriber");
}


namespace errorDispatcherEvent
{
    class Unsubscribe: public Event::Base
    {



    public:
        static Base* construct(const route_t &r)
        {
            return new Unsubscribe(r);
        }
        Unsubscribe(const route_t&r)
            :Base(errorDispatcherEventEnum::Unsubscribe,r) {}
        void unpack(inBuffer&)
        {
        }
        void pack(outBuffer&) const
        {
        }
        void jdump(Json::Value &) const
        {
        }
    };

}
namespace errorDispatcherEvent
{

    class Subscribe: public Event::Base
    {



    public:
        static Base* construct(const route_t &r)
        {
            return new Subscribe(r);
        }
        Subscribe(const route_t&r)
            :Base(errorDispatcherEventEnum::Subscribe,r) {}
        void unpack(inBuffer&)
        {
        }
        void pack(outBuffer&) const
        {
        }
        void jdump(Json::Value &) const
        {
        }
    };

}
namespace errorDispatcherEvent
{
    class SendMessage: public Event::Base
    {



    public:
        static Base* construct(const route_t &r)
        {
            return new SendMessage(r);
        }
        SendMessage(const std::string &_opcode, const std::string& _msg)
            :Base(errorDispatcherEventEnum::SendMessage),opcode(_opcode),msg(_msg) {}
        SendMessage(const route_t&r)
            :Base(errorDispatcherEventEnum::SendMessage,r) {}
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
        void jdump(Json::Value &v) const
        {
            v["opcode"]=opcode;
            v["msg"]=msg;
        }

    };


}
namespace errorDispatcherEvent
{
    class NotifySubscriber: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new NotifySubscriber(r);
        }
        NotifySubscriber(const std::string& _opcode, const std::string& _msg, const route_t&r)
            :Base(errorDispatcherEventEnum::NotifySubscriber,r),opcode(_opcode),msg(_msg) {}
        NotifySubscriber(const route_t&r)
            :Base(errorDispatcherEventEnum::NotifySubscriber,r) {}
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

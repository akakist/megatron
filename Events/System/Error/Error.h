#ifndef _________JSON_REF_EVENT_____H5
#define _________JSON_REF_EVENT_____H5
#include "SERVICE_id.h"
#include "route_t.h"

namespace ErrorEventEnum
{
    const EVENT_id ErrorRSP("ErrorRSP");
}

namespace ErrorEvent
{


    class ErrorRSP: public Event::Base
    {
        enum {channel=CHANNEL_100};


    public:
        static Base* construct(const route_t &r)
        {
            return new ErrorRSP(r);
        }
        ErrorRSP(int _errCode, const std::string& _errtext,
                 const route_t&r)
            :Base(ErrorEventEnum::ErrorRSP,channel,r),errcode(_errCode),errText(_errtext) {}
        ErrorRSP(int _errCode, const std::string& _action
                )
            :Base(ErrorEventEnum::ErrorRSP,channel),errcode(_errCode),errText(_action) {}
        ErrorRSP(const route_t&r)
            :Base(ErrorEventEnum::ErrorRSP,channel,r) {}
        int errcode;
        std::string errText;
        void unpack(inBuffer& o)
        {
            o>>errcode;
            o>>errText;
        }
        void pack(outBuffer&o) const
        {
            o<<errcode<<errText;
        }
        void jdump(Json::Value &v) const
        {
            v["errcode"]=errcode;
            v["action"]=errText;
        }

    };

}
#endif

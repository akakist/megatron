#ifndef _________JSON_REF_EVENT_____H5
#define _________JSON_REF_EVENT_____H5
#include "___jsonRefEvent.h"
#include "SERVICE_id.h"
#include "route_t.h"
namespace jsonRefEvent
{


    class JsonREQ: public Event::Base
    {
        enum {channel=CHANNEL_100};


    public:
        static Base* construct(const route_t &r)
        {
            Base* z=new JsonREQ(r);
            return z;
        }
        JsonREQ(const std::string& _action, const std::string &_json, const std::string& _binData,
                const route_t&r)
            :Base(jsonRefEventEnum::JsonREQ,channel,r),action(_action),json(_json),binData(_binData)
        {}
        JsonREQ(const route_t&r)
            :Base(jsonRefEventEnum::JsonREQ,channel,r) {}
        std::string action;
        std::string json;
        std::string binData;
        void unpack(inBuffer& o)
        {
            o>>action>>json>>binData;
        }
        void pack(outBuffer&o) const
        {
            o<<action<<json<<binData;
        }
        void jdump(Json::Value &v) const
        {
            v["json"]=json;
            v["action"]=action;
        }

    };
    class JsonRSP: public Event::Base
    {
        enum {channel=CHANNEL_100};


    public:
        static Base* construct(const route_t &r)
        {
            return new JsonRSP(r);
        }
        JsonRSP(int _errCode, const std::string& _action,const std::string &_json,const std::string &_binData,
                const route_t&r)
            :Base(jsonRefEventEnum::JsonRSP,channel,r),errcode(_errCode),action(_action),json(_json), binData(_binData)
        {}
        JsonRSP(const route_t&r)
            :Base(jsonRefEventEnum::JsonRSP,channel,r) {}
        int errcode;
        std::string action;
        std::string json;
        std::string binData;
        void unpack(inBuffer& o)
        {
            o>>errcode;
            o>>action;
            o>>json;
            o>>binData;
        }
        void pack(outBuffer&o) const
        {
            o<<errcode<<action<<json<<binData;
        }
        void jdump(Json::Value &v) const
        {
            v["errcode"]=errcode;
            v["json"]=json;
            v["action"]=action;
        }

    };

}
#endif

#ifndef _________JSON_REF_EVENT_____H71
#define _________JSON_REF_EVENT_____H71
#include "___jsonRefEvent.h"
namespace jsonRefEvent
{
class JsonRSP: public Event::Base
{
    enum {channel=CHANNEL_100};


public:
    static Base* construct(const route_t &r)
    {
        return new JsonRSP(r);
    }
    JsonRSP(int _errCode, const std::string& _action,const std::string &_json,const std::string &_binData, const JAVACOOKIE_id& _javaCookie,
                 const route_t&r)
        :Base(jsonRefEventEnum::JsonRSP,channel,"JsonRSP",r),errcode(_errCode),action(_action),json(_json), binData(_binData),
         javaCookie(_javaCookie) {}
    JsonRSP(const route_t&r)
        :Base(jsonRefEventEnum::JsonRSP,channel,"JsonRSP",r) {}
    int errcode;
    std::string action;
    std::string json;
    std::string binData;
    JAVACOOKIE_id javaCookie;
    void unpack(inBuffer& o)
    {
        o>>errcode;
        o>>action;
        o>>json;
        o>>binData;
        o>>javaCookie;
    }
    void pack(outBuffer&o) const
    {
        o<<errcode<<action<<json<<binData<<javaCookie;
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

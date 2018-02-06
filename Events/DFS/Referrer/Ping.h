#ifndef _________dfsReferrerEvent_h19
#define _________dfsReferrerEvent_h19
#include "___dfsReferrerEvent.h"
namespace dfsReferrerEvent {
    class Ping: public Event::Base
    {
        enum {channel=CHANNEL_0};


    public:
        static Base* construct(const route_t &r)
        {
            return new Ping(r);
        }
        int pingType;
        GlobalCookie_id globalCookieOfSender;
        unsigned short externalListenPort;
        std::set<msockaddr_in> internalListenAddr;
        int64_t ping_time;
        int connection_sequence_id;
        Ping(const int& _type, const GlobalCookie_id& _globalCookie, unsigned short _externalListenPort, const std::set<msockaddr_in>& _internalListenAddr, int64_t _ping_time, int _connection_sequence_id, const route_t &r)
            :Base(dfsReferrerEventEnum::Ping,channel,"Ping",r),
             pingType(_type),globalCookieOfSender(_globalCookie),externalListenPort(_externalListenPort),internalListenAddr(_internalListenAddr),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id)
        {}

        Ping(const int& _type, const GlobalCookie_id& _globalCookie,
             int64_t _ping_time, int _connection_sequence_id, const route_t &r)
            :Base(dfsReferrerEventEnum::Ping,channel,"Ping",r),
             pingType(_type),globalCookieOfSender(_globalCookie),externalListenPort(0),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id)
        {}

        Ping(const route_t& r)
            :Base(dfsReferrerEventEnum::Ping,channel,"Ping",r) {}
        void unpack(inBuffer& o)
        {
            
            o>>pingType>>globalCookieOfSender>>externalListenPort>>internalListenAddr>>ping_time>>connection_sequence_id;
        }
        void pack(outBuffer& o) const
        {
            
            o<<pingType<<globalCookieOfSender<<externalListenPort<<internalListenAddr<<ping_time<<connection_sequence_id;
        }
        void jdump(Json::Value &v) const
        {
            v["externalListenPort"]=externalListenPort;
            v["internalListenAddr"]=iUtils->dump(internalListenAddr);
            switch(pingType)
            {
            case dfsReferrer::PingType::PT_CACHED:
                v["pingType"]="PT_CACHED";
                break;
            case dfsReferrer::PingType::PT_MASTER_SHORT:
                v["pingType"]="PT_MASTER_SHORT";
                break;
            case dfsReferrer::PingType::PT_MASTER_LONG:
                v["pingType"]="PT_MASTER_LONG";
                break;
            case dfsReferrer::PingType::PT_CAPS_SHORT:
                v["pingType"]="PT_CAPS_SHORT";
                break;
            case dfsReferrer::PingType::PT_CAPS_LONG:
                v["pingType"]="PT_CAPS_LONG";
                break;
            default:
                v["pingType"]="___UNDEF";
                break;
            }
        }

    };
}
#endif

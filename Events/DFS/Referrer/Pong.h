#ifndef _________dfsReferrerEvent_h20
#define _________dfsReferrerEvent_h20
#include "___dfsReferrerEvent.h"
namespace dfsReferrerEvent {

    class Pong: public Event::Base
    {
        enum {rpcChannel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {
            return new Pong(r);
        }
        int pingType;
        msockaddr_in visible_name_of_pinger;
        GlobalCookie_id globalCookieOfResponder;
        int nodeLevelInHierarhy;
        int64_t ping_time;
        int connection_sequence_id;
        Pong(const int& _type,const msockaddr_in& _visible_name_of_pinger,const GlobalCookie_id& _globalCookieOfResponder, const int& nodeLevelInHierarhy_, int64_t _ping_time, int _connection_sequence_id,const route_t& r)
            :Base(dfsReferrerEventEnum::Pong,rpcChannel,"Pong",r),
             pingType(_type),visible_name_of_pinger(_visible_name_of_pinger),globalCookieOfResponder(_globalCookieOfResponder),
             nodeLevelInHierarhy(nodeLevelInHierarhy_),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id)
        {}
        Pong(const route_t& r)
            :Base(dfsReferrerEventEnum::Pong,rpcChannel,"Pong",r) {}
        void unpack(inBuffer& o)
        {
            
            o>>pingType>>visible_name_of_pinger>>globalCookieOfResponder>>nodeLevelInHierarhy>>ping_time>>connection_sequence_id;
        }
        void pack(outBuffer&o) const
        {
            
            o<<pingType<<visible_name_of_pinger<<globalCookieOfResponder<<nodeLevelInHierarhy<<ping_time<<connection_sequence_id;
        }
        void jdump(Json::Value &v) const
        {
            v["visible_name"]=visible_name_of_pinger.dump();
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

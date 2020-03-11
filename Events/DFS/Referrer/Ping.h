#ifndef _________dfsReferrerEvent_h19Z1
#define _________dfsReferrerEvent_h19Z1
#include "___dfsReferrerEvent.h"
namespace dfsReferrerEvent {
    class Ping: public Event::Base
    {
        enum {rpcChannel=CHANNEL_0};


    public:
        enum ClientType
        {
            CT_NODE,
            CT_CLIENT
        };
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
        int clientType;
        Ping(const int& _type, const GlobalCookie_id& _globalCookie, unsigned short _externalListenPort, const std::set<msockaddr_in>& _internalListenAddr, int64_t _ping_time, int _connection_sequence_id, int _clientType,const route_t &r)
            :Base(dfsReferrerEventEnum::Ping,rpcChannel,r),
             pingType(_type),globalCookieOfSender(_globalCookie),externalListenPort(_externalListenPort),internalListenAddr(_internalListenAddr),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id),clientType(_clientType)
        {}

        Ping(const int& _type, const GlobalCookie_id& _globalCookie,
             int64_t _ping_time, int _connection_sequence_id, int _clientType,const route_t &r)
            :Base(dfsReferrerEventEnum::Ping,rpcChannel,r),
             pingType(_type),globalCookieOfSender(_globalCookie),externalListenPort(0),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id),clientType(_clientType)
        {}

        Ping(const route_t& r)
            :Base(dfsReferrerEventEnum::Ping,rpcChannel,r) {}
        void unpack(inBuffer& o)
        {

            o>>pingType>>globalCookieOfSender>>externalListenPort>>internalListenAddr>>ping_time>>connection_sequence_id>>clientType;
        }
        void pack(outBuffer& o) const
        {

            o<<pingType<<globalCookieOfSender<<externalListenPort<<internalListenAddr<<ping_time<<connection_sequence_id<<clientType;
        }
        void jdump(Json::Value &v) const
        {
            v["globalCookieOfSender"]=iUtils->bin2hex(CONTAINER(globalCookieOfSender));
            v["internalListenAddr"]=iUtils->dump(internalListenAddr);
            v["externalListenPort"]=externalListenPort;
            v["ping_time"]=std::to_string(ping_time);
            v["connection_sequence_id"]=connection_sequence_id;
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
            :Base(dfsReferrerEventEnum::Pong,rpcChannel,r),
             pingType(_type),visible_name_of_pinger(_visible_name_of_pinger),globalCookieOfResponder(_globalCookieOfResponder),
             nodeLevelInHierarhy(nodeLevelInHierarhy_),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id)
        {}
        Pong(const route_t& r)
            :Base(dfsReferrerEventEnum::Pong,rpcChannel,r) {}
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

            v["globalCookieOfResponder"]=iUtils->bin2hex(CONTAINER(globalCookieOfResponder));
            v["ping_time"]=std::to_string(ping_time);
            v["connection_sequence_id"]=connection_sequence_id;
            v["visible_name"]=visible_name_of_pinger.dump();
            v["nodeLevelInHierarhy"]=nodeLevelInHierarhy;
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

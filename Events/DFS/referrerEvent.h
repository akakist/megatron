#pragma once
#include "EVENT_id.h"
#include "GlobalCookie_id.h"
#include "IUtils.h"
#include "SERVICE_id.h"
#include "event_mt.h"
#include "mutexInspector.h"
namespace ServiceEnum
{
    const SERVICE_id DFSReferrer(genum_DFSReferrer);
    const SERVICE_id ReferrerClient(genum_RefferrerClient);
}

namespace dfsReferrerEventEnum
{

    const EVENT_id Ping(genum_DFSReferrer_Ping);
    const EVENT_id Hello(genum_DFSReferrer_Hello);
    const EVENT_id SubscribeNotifications(genum_DFSReferrer_SubscribeNotifications);
    const EVENT_id Noop(genum_DFSReferrer_Noop);

    const EVENT_id ToplinkDeliverREQ(genum_DFSReferrer_ToplinkDeliverREQ);
    const EVENT_id ToplinkDeliverRSP(genum_DFSReferrer_ToplinkDeliverRSP);



    const EVENT_id Pong(genum_DFSReferrer_Pong);
    const EVENT_id NotifyReferrerUplinkIsConnected(genum_DFSReferrerNotifyReferrerUplinkIsConnected);
    const EVENT_id NotifyReferrerUplinkIsDisconnected(genum_DFSReferrerNotifyReferrerUplinkIsDisconnected);
    const EVENT_id NotifyReferrerDownlinkDisconnected(genum_DFSReferrerNotifyReferrerDownlinkDisconnected);
    const EVENT_id InitClient(genum_DFSReferrerInitClient);

    const EVENT_id UpdateConfigREQ(genum_DFSReferrerUpdateConfigREQ);
    const EVENT_id UpdateConfigRSP(genum_DFSReferrerUpdateConfigRSP);

}
namespace dfsReferrer {


    namespace PingType {
        enum
        {
            PT_CACHED,
            PT_MASTER_SHORT,
            PT_MASTER_LONG,
            PT_CAPS_SHORT,
            PT_CAPS_LONG,
        };
    }

}
/// if SOCKET_id==0 then socket is local storage


namespace dfsReferrerEvent {
    class InitClient: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        InitClient(const std::set<msockaddr_in>& _caps)
            :NoPacked(dfsReferrerEventEnum::InitClient),caps(_caps) {}

        std::set<msockaddr_in>caps;
        void jdump(Json::Value &j) const
        {
            for(auto& z:caps)
            {
                j["caps"].append(z.dump());
            }
        }
    };

}
namespace dfsReferrerEvent {
    class NotifyReferrerDownlinkDisconnected: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyReferrerDownlinkDisconnected(const SOCKET_id& _sid, const route_t& r)
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerDownlinkDisconnected,r),socketId(_sid) {}

        const SOCKET_id& socketId;
        void jdump(Json::Value &j) const
        {
            j["socketId"]=std::to_string(CONTAINER(socketId));
        }
    };
    class NotifyReferrerUplinkIsConnected: public Event::NoPacked
    {
        /**
            Clean old uplinks and replace by new
          */
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyReferrerUplinkIsConnected(const msockaddr_in& _sa,const route_t& r)
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerUplinkIsConnected,r),sa(_sa) {}

        msockaddr_in sa;
        void jdump(Json::Value &j) const
        {
            j["sa"]=sa.dump();
        }
    };
    class NotifyReferrerUplinkIsDisconnected: public Event::NoPacked
    {

    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        NotifyReferrerUplinkIsDisconnected(const msockaddr_in& _sa,const route_t& r)
            :NoPacked(dfsReferrerEventEnum::NotifyReferrerUplinkIsDisconnected,r), sa(_sa) {}

        msockaddr_in sa;
        void jdump(Json::Value &j) const
        {
            j["sa"]=sa.dump();
        }

    };

}
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
            :Base(dfsReferrerEventEnum::Ping,r),
             pingType(_type),globalCookieOfSender(_globalCookie),externalListenPort(_externalListenPort),internalListenAddr(_internalListenAddr),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id),clientType(_clientType)
        {}

        Ping(const int& _type, const GlobalCookie_id& _globalCookie,
             int64_t _ping_time, int _connection_sequence_id, int _clientType,const route_t &r)
            :Base(dfsReferrerEventEnum::Ping,r),
             pingType(_type),globalCookieOfSender(_globalCookie),externalListenPort(0),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id),clientType(_clientType)
        {}

        Ping(const route_t& r)
            :Base(dfsReferrerEventEnum::Ping,r) {}
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
            :Base(dfsReferrerEventEnum::Pong,r),
             pingType(_type),visible_name_of_pinger(_visible_name_of_pinger),globalCookieOfResponder(_globalCookieOfResponder),
             nodeLevelInHierarhy(nodeLevelInHierarhy_),ping_time(_ping_time),connection_sequence_id(_connection_sequence_id)
        {}
        Pong(const route_t& r)
            :Base(dfsReferrerEventEnum::Pong,r) {}
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


/**
Доставка события вверх по облаку. На некотором уровне оно форвардится в сервис, согласно общей конфигурации ботов.

Eсли destination service == "broadcast", то делаем бродкаст по облаку.
*/
namespace dfsReferrerEvent {
    class ToplinkDeliverREQ: public Event::Base
    {
        enum {rpcChannel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {
            return new ToplinkDeliverREQ(r);
        }
        ToplinkDeliverREQ(const SERVICE_id& _destinationService, const REF_getter<Event::Base>& e, const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverREQ,r),
             destinationService(_destinationService),eventData(NULL)
        {
            outBuffer o;
            iUtils->packEvent(o,e);
            eventData=o.asString();
        }
        ToplinkDeliverREQ(const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverREQ,r),eventData(NULL)
        {
            if(!eventData.valid())
                eventData=new refbuffer;

        }
        SERVICE_id destinationService;
        REF_getter<refbuffer> eventData;

        std::set<GlobalCookie_id> uuid;

        REF_getter<Event::Base> getEvent() const
        {
            MUTEX_INSPECTOR;
            inBuffer in(eventData);
            REF_getter<Event::Base> z=iUtils->unpackEvent(in);
            return z;
        }
        void unpack(inBuffer& o)
        {
            o>>destinationService>>eventData>>uuid;
        }
        void pack(outBuffer&o) const
        {
            o<<destinationService<<eventData<<uuid;
        }
        void jdump(Json::Value &v) const
        {

            v["destinationService"]=iUtils->genum_name(id);
        }

    };
    class ToplinkDeliverRSP: public Event::Base
    {
        enum {rpcChannel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {

            return new ToplinkDeliverRSP(r);
        }
        ToplinkDeliverRSP(const REF_getter<Event::Base>& _e,const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverRSP,r),
             eventData(NULL)
        {
            outBuffer o;
            iUtils->packEvent(o,_e);
            eventData=o.asString();

        }
        ToplinkDeliverRSP(const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverRSP,r),eventData(NULL)
        {
            if(!eventData.valid())
                eventData=new refbuffer;

        }
        REF_getter<refbuffer> eventData;
        void unpack(inBuffer& o)
        {

            o>>eventData;
        }
        void pack(outBuffer&o) const
        {

            o<<eventData;
        }
        void jdump(Json::Value &) const
        {
        }
        REF_getter<Event::Base> getEvent() const
        {
            MUTEX_INSPECTOR;
            inBuffer in(eventData);
            REF_getter<Event::Base> z=iUtils->unpackEvent(in);
            return z;
        }

    };


}

namespace dfsReferrerEvent {
    class SubscribeNotifications: public Event::Base
    {
        enum {rpcChannel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {
            return new SubscribeNotifications(r);
        }
        SubscribeNotifications(const route_t& r)
            :Base(dfsReferrerEventEnum::SubscribeNotifications,r) {}

        void jdump(Json::Value &) const
        {
        }
        void unpack(inBuffer&)
        {
        }
        void pack(outBuffer&) const
        {
        }
    };


}

namespace dfsReferrerEvent {
    class UpdateConfigREQ: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new UpdateConfigREQ(r);
        }
        UpdateConfigREQ(const route_t& r)
            :Base(dfsReferrerEventEnum::UpdateConfigREQ,r) {}
        UpdateConfigREQ(const std::string &_body,  const route_t &r)
            :Base(dfsReferrerEventEnum::UpdateConfigREQ,r),
             bod(_body) {}
        std::string bod;
        void unpack(inBuffer& o)
        {

            o>>bod;
        }
        void pack(outBuffer&o) const
        {

            o<<bod;
        }
        void jdump(Json::Value &v) const
        {
            v["body"]=bod;
        }
    };

    class UpdateConfigRSP: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new UpdateConfigRSP(r);
        }
        UpdateConfigRSP(const route_t& r)
            :Base(dfsReferrerEventEnum::UpdateConfigRSP,r) {}
        UpdateConfigRSP(const std::string &_body,  const route_t &r)
            :Base(dfsReferrerEventEnum::UpdateConfigRSP,r),
             bod(_body) {}
        std::string bod;
        void unpack(inBuffer& o)
        {

            o>>bod;
        }
        void pack(outBuffer&o) const
        {

            o<<bod;
        }
        void jdump(Json::Value &v) const
        {
            v["body"]=bod;
        }
    };
}


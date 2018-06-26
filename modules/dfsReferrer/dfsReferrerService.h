#ifndef ___________RPC__SERVER__H
#define ___________RPC__SERVER__H
#include "listenerBuffered1Thread.h"
#include "broadcaster.h"
#include "REF.h"
#include "SOCKET_id.h"
#include "ioBuffer.h"
#include "route_t.h"
#include "CHUNK_id.h"
#include "msockaddr_in.h"
#include "neighbours.h"
#include "logging.h"
#include "GlobalCookie_id.h"
#include "linkinfo.h"
#include "uplinkconnectionstate.h"
#include "ISSL.h"
#include "uriReferals.h"
#include "timerHelper.h"
#include "url.h"
#include "configDB.h"
#include "Events/System/Run/startService.h"
#include "Events/Tools/webHandler/RequestIncoming.h"
#include "Events/Tools/telnet/CommandEntered.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/Disconnected.h"
#include "Events/System/Net/rpc/Disaccepted.h"
#include "Events/System/Net/rpc/Accepted.h"
#if !defined(WITHOUT_UPNP)
#include "Events/System/Net/rpc/UpnpResult.h"
#endif
#include "Events/System/Net/rpc/Connected.h"
#include "Events/System/Net/rpc/ConnectFailed.h"
#include "Events/System/Net/rpc/Binded.h"
#include "Events/System/timer/TickTimer.h"
#include "Events/System/timer/TickAlarm.h"
#include "Events/DFS/Referrer/Elloh.h"
#include "Events/DFS/Referrer/Hello.h"
#include "Events/DFS/Referrer/NotifyReferrer.h"
#include "Events/DFS/Referrer/NotifyReferrer.h"
#include "Events/DFS/Referrer/NotifyReferrer.h"
#include "Events/DFS/Referrer/NotifyReferrer.h"
#include "Events/DFS/Referrer/Ping.h"
#include "Events/DFS/Referrer/Pong.h"
#include "Events/DFS/Referrer/UpdateConfig.h"
#include "Events/DFS/Referrer/UpdateConfig.h"
#include "Events/DFS/Referrer/SubscribeNotifications.h"
#include "Events/DFS/Referrer/ToplinkBroadcastByBackroute.h"

#include "Events/DFS/Caps/RegisterMyRefferrer.h"
#include "Events/DFS/Caps/GetRefferrers.h"
#include "Events/System/Net/rpc/SubscribeNotifications.h"
#include "Events/System/Net/rpc/UpnpPortMap.h"
#include "Events/Tools/telnet/RegisterCommand.h"
#include "Events/Tools/telnet/Reply.h"
#include "Events/Tools/webHandler/RegisterDirectory.h"
#include "Events/Tools/webHandler/RegisterHandler.h"
#include "Events/DFS/Referrer/ToplinkDeliver.h"
#include "Events/DFS/Referrer/ToplinkDeliver.h"


namespace refTimer {
    enum {
        T_001_common_connect_failed=1001,
        T_002_D3_caps_get_service_request_is_timed_out=1002,
        T_007_D6_resend_ping_caps_short=1007,
        T_008_D6_resend_ping_caps_long=1008,
        T_009_pong_timed_out_caps_long=1009,
        T_011_downlink_ping_timed_out=1011,
        T_012_reregister_referrer=1012,

        T_019_D5_external_connection_check_timeout=1019,

        T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers=1020,

        T_040_D2_cache_pong_timed_out_from_neighbours=1040,

    };
}

using namespace  refTimer;

#define CLOSE_DOWNLINKS

namespace dfsReferrer
{


    class Service:
        private UnknownBase,
        private ListenerBuffered1Thread,
        public Broadcaster,
        public Logging,
        public TimerHelper
    {

        REF_getter<_uplinkConnectionState> uplinkConnectionState;


        msockaddr_in getUL();
        bool hasUL();



        REF_getter<_uriReferals> urirefs;

        bool on_startService(const systemEvent::startService*);

    public:

        void on_Disaccepted(const SOCKET_id& sockId);
        bool handleEvent(const REF_getter<Event::Base>& e);

        bool on_CommandEntered(const telnetEvent::CommandEntered*e);

#if !defined(WITHOUT_UPNP)
        bool on_UpnpResult(const rpcEvent::UpnpResult*);
#endif
        bool on_ConnectFailed(const rpcEvent::ConnectFailed*);
        bool on_Disconnected(const rpcEvent::Disconnected*e);
        bool on_Disaccepted(const rpcEvent::Disaccepted*e);
        bool on_Connected(const rpcEvent::Connected*);
        bool on_IncomingOnConnector(const rpcEvent::IncomingOnConnector*);
        bool on_IncomingOnAcceptor(const rpcEvent::IncomingOnAcceptor*);
        bool on_Accepted(const rpcEvent::Accepted*);
        bool on_Binded(const rpcEvent::Binded*);



        bool on_TickTimer(const timerEvent::TickTimer*);
        bool on_TickAlarm(const timerEvent::TickAlarm*);

        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);
        bool on_UpdateConfigREQ(const dfsReferrerEvent::UpdateConfigREQ*);
        bool on_UpdateConfigRSP(const dfsReferrerEvent::UpdateConfigRSP*);

        void on_deleteDownlink(const SOCKET_id& sock, const msockaddr_in& remote_name);


        void _reset_alarm(int alarm_id);
        void _reset_alarm(int alarm_id, const msockaddr_in& sa);
        void _reset_alarm(int alarm_id1,int alarm_id2, const msockaddr_in& sa);
        void _stop_alarm(int alarm_id);
        void _stop_alarm(int alarm_id, const msockaddr_in& sa);
        void _stop_alarm(int alarm_id1,int alarm_id2, const msockaddr_in& sa);
        void _stop_alarm(int alarm_id1,int alarm_id2,int alarm_id3, const msockaddr_in& sa);

        void reset_T_001_common_connect_failed();



        void STAGE_D2_PING_NEIGHBOURS_start();
        void STAGE_D21_PING_CAPS_start();


        time_t d2_start_time;


        void STAGE_D3_GET_GEFERRERS_start(const msockaddr_in& sa);



        void d4_on_disconnected(const msockaddr_in& sa);
        void d4_on_connect_failed(const msockaddr_in& sa);
        void d4_uplink_mode(const REF_getter<epoll_socket_info> &esi,const msockaddr_in& visibleName);

        /// check external connection
        void d5_start_external_connection_check(const REF_getter<epoll_socket_info>&esi,const msockaddr_in& visibleName);

        /// maintain connection with caps
        void d6_start(const msockaddr_in& sa);
        bool d6_on_pong_PT_CAPS_SHORT(const msockaddr_in&visible_name_of_pinger, const REF_getter<epoll_socket_info>&);
        bool d6_on_pong_PT_CAPS_LONG(const msockaddr_in&visible_name_of_pinger, const REF_getter<epoll_socket_info>&);
        void d6_on_T_011_resend_ping_PT_CAPS_LONG(const msockaddr_in& remote_addr);
        void d6_on_T_012_pong_timed_out_PT_CAPS_LONG(const msockaddr_in& remote_addr);
        void d6_on_disconnected(const msockaddr_in& remote_addr);
        void d6_on_connect_failed(const msockaddr_in& remote_addr);


        bool on_connectionEstablished(const msockaddr_in &remote_addr);

        bool  on_SubscribeNotifications(const dfsReferrerEvent::SubscribeNotifications*e);


        bool on_ToplinkDeliverREQ(const dfsReferrerEvent::ToplinkDeliverREQ *e, const rpcEvent::IncomingOnAcceptor* acc);
        bool on_ToplinkDeliverRSP(const dfsReferrerEvent::ToplinkDeliverRSP *e);

        bool on_Pong(const dfsReferrerEvent::Pong* e,  const REF_getter<epoll_socket_info>& esi);

        bool on_acceptor_Elloh(const dfsReferrerEvent::Elloh* e, const REF_getter<epoll_socket_info>& esi);
        bool on_acceptor_Hello(const dfsReferrerEvent::Hello*, const REF_getter<epoll_socket_info>& esi);
        bool on_Ping(const dfsReferrerEvent::Ping* e, const REF_getter<epoll_socket_info>& esi);


        void sendToplinkReq(const msockaddr_in &uplink, dfsReferrerEvent::ToplinkDeliverREQ *ee);

        std::set<route_t> m_readyNotificationBackroutes;

        void kill_downlinks();

        void registerReferrer(const msockaddr_in &destination);

        bool isCaps(const msockaddr_in& sa);

    public:
        Service(const SERVICE_id &svs, const std::string&  nm,IInstance* ifa);
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa);
        ~Service();

        IInstance *iInstance;
        ////
#if !defined(WITHOUT_UPNP)
        bool m_upnpExecuted;
        bool m_upnpInRequesting;
#endif
        bool externalAccessIsPossible;
        enum _stage
        {
            STAGE_D2_PING_NEIGHBOURS,
            STAGE_D21_PING_CAPS,
            STAGE_D3_GET_GEFERRERS,
            STAGE_D3_1_SEND_PING_TO_RESPONDED_REFERRERS,
            STAGE_D4_MAINTAIN_CONNECTION,
            STAGE_D5_CHECK_EXTERNAL,
            STAGE_D6_MAINTAIN_CONNECTION,

        };
        _stage stage;

        int connection_sequence_id;
        _neighbours neighbours;

        std::string config_body;


    };

}

#endif

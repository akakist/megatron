#ifndef ___________RPC__SERVER__H
#define ___________RPC__SERVER__H
#include <REF.h>
#include <webDumpable.h>
#include <SOCKET_id.h>

#include "upnp_tr.h"
#include "event.h"
#include <epoll_socket_info.h>
#include <json/value.h>
#include <broadcaster.h>
#include <msockaddr_in.h>
#include <listenerBuffered1Thread.h>
#include <IRPC.h>

#include "Events/System/Net/rpc/PassPacket.h"
#include "Events/System/Net/rpc/SendPacket.h"
#include "Events/System/Net/rpc/SubscribeNotifications.h"
#include "Events/System/Net/rpc/UnsubscribeNotifications.h"
#include "Events/System/Net/rpc/UpnpPortMap.h"
#include "Events/System/Net/rpc/UpnpResult.h"
#include "Events/System/Net/rpc/Connected.h"
#include "Events/System/Net/oscar/Connected.h"
#include "Events/System/Net/oscar/PacketOnAcceptor.h"
#include "Events/System/Net/oscar/PacketOnConnector.h"
#include "Events/Tools/webHandler/RegisterHandler.h"
#include "Events/Tools/webHandler/RequestIncoming.h"

#include <Events/System/Net/oscar/SendPacket.h>
#include <Events/System/Net/oscar/Disconnected.h>
#include <Events/System/timer/TickAlarm.h>
#include <Events/Tools/webHandler/RequestIncoming.h>
#include <Events/System/Net/rpc/SubscribeNotifications.h>
#include <Events/System/Net/rpc/SendPacket.h>
#include <Events/System/Net/rpc/PassPacket.h>
#include <Events/System/Run/startService.h>
#include <Events/System/Net/oscar/ConnectFailed.h>
#include <Events/System/Net/oscar/Accepted.h>
#include <Events/System/Net/oscar/NotifyOutBufferEmpty.h>
#include <Events/System/Net/oscar/NotifyBindAddress.h>
#include <Events/System/Net/oscar/Disaccepted.h>

namespace RPC
{

    namespace timers
    {
        enum
        {
            CONNECTION_ACTIVITY
        };
    }

    struct Session: public Refcountable, public Mutexable,WebDumpable
    {

        SOCKET_id socketId;
        std::deque<std::string> m_cache;
        std::map<int,std::deque<REF_getter<oscarEvent::SendPacket> > > m_OutEventCache;
        time_t last_time_hit;
        enum currentState {EMPTY,FULL};
        currentState cstate;
        int64_t bufferSize;
        bool m_connectionEstablished;
        REF_getter<epoll_socket_info> esi;
        void update_last_time_hit()
        {
            last_time_hit=time(NULL);
        }

        Session(SOCKET_id sockId,const REF_getter<epoll_socket_info>& _esi):socketId(sockId),last_time_hit(time(NULL)), cstate(EMPTY), bufferSize(0),m_connectionEstablished(false),esi(_esi) {}

        Json::Value jdump()
        {
            M_LOCK(this);
            Json::Value v;
            v["socketId"]=std::to_string(CONTAINER(socketId));
            v["cache_element_count"]=std::to_string((int64_t)m_cache.size());
            int64_t csum=0;
            for(auto &z: m_cache)
            {
                csum+=z.size();
            }

            v["cache_buffer_size"]=std::to_string(csum);
            v["OutEventCache count"]=std::to_string(m_OutEventCache.size());

            int64_t oecSum=0;
            for(auto &z: m_OutEventCache)
            {
                for(auto &x: z.second)
                {
                    oecSum+=x->buf->size_;
                }
            }
            v["OutEventCache size"]=std::to_string(oecSum);

            v["last_time_hit_dt"]=std::to_string(uint64_t(time(NULL)-last_time_hit));
            v["m_connectionEstablished"]=m_connectionEstablished;
            v["currentState"]=cstate==EMPTY?"EMPTY":"FULL";
            v["bufferSize"]=std::to_string(bufferSize);
            return v;
        }
        std::string wname()
        {
            return std::to_string(CONTAINER(socketId));
        }
        Json::Value wdump()
        {
            Json::Value j;
            return jdump();
        }

    };
    struct __sessions: public Refcountable,Broadcaster, WebDumpable
    {

        bool getAddrOnConnected(const SOCKET_id& id, msockaddr_in &out);

        __sessions(IInstance* ifa):
            Broadcaster(ifa) {}

        std::string wname()
        {
            return "sessions";
        }
        Json::Value wdump()
        {
            Json::Value j;
            return jdump();
        }


        // all
        struct _all
        {
            std::set<route_t> m_subscribers;
            std::map<SOCKET_id, REF_getter<Session> > m_socketId2session;

            void clear()
            {
                m_subscribers.clear();
                m_socketId2session.clear();
            }
        };
        _all all;
        REF_getter<Session>  getSessionOrNull(const SOCKET_id& sid)
        {
            auto i=all.m_socketId2session.find(sid);

            if(i!=all.m_socketId2session.end())
                return i->second;

            return NULL;
        }
        std::map<SOCKET_id, REF_getter<Session> > getSessionContainer()
        {
            return all.m_socketId2session;
        }

        // connector
        struct _connector
        {
            std::map<msockaddr_in,SOCKET_id > m_sa2socketId;
            std::map<SOCKET_id,msockaddr_in> m_socketId2sa;
            void clear()
            {
                m_sa2socketId.clear();
                m_socketId2sa.clear();
            }
        };
        _connector connector;

        Json::Value jdump();

        void clear()
        {
            connector.clear();
            all.clear();
        }
    public:
    };

    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster,
        public IRPC
    {


        SERVICE_id myOscar;
        const real m_iterateTimeout;

        struct _shared_Addr: public Mutexable
        {
            _shared_Addr():m_networkInitialized(false) {}
            std::set<msockaddr_in> m_internalAddr;
            bool m_networkInitialized;
            std::set<msockaddr_in>m_bindAddr_main;
            std::set<msockaddr_in>m_bindAddr_reserve;
        };
        _shared_Addr sharedAddr;
        const real m_connectionActivityTimeout;
        struct _upnp
        {
            tr_upnp* upnp;
            Mutex upnpMX;
            _upnp():upnp(NULL) {}

        };
        _upnp upnp;

    protected:

    public:
        unsigned short getExternalListenPortMain(); // network byte order
        std::set<msockaddr_in> getInternalListenAddrs(); // network byte order

        REF_getter<__sessions> sessions;
    public:


        bool on_Connected(const oscarEvent::Connected*);
        bool on_Disconnected(const oscarEvent::Disconnected*e);
        bool on_Disaccepted(const oscarEvent::Disaccepted*e);
        bool on_PacketOnConnector(const oscarEvent::PacketOnConnector*);
        bool on_PacketOnAcceptor(const oscarEvent::PacketOnAcceptor*);
        bool on_NotifyBindAddress(const oscarEvent::NotifyBindAddress*);
        bool on_NotifyOutBufferEmpty(const oscarEvent::NotifyOutBufferEmpty*);
        bool on_Accepted(const oscarEvent::Accepted*);
        bool on_ConnectFailed(const oscarEvent::ConnectFailed*e);


        bool on_startService(const systemEvent::startService*);

        bool on_PassPacket(const rpcEvent::PassPacket*);
        bool on_SendPacket(const rpcEvent::SendPacket*);

        bool on_SubscribeNotifications(const rpcEvent::SubscribeNotifications* E);
        bool on_UnsubscribeNotifications(const rpcEvent::UnsubscribeNotifications* E);

        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming* e);

        bool on_TickAlarm(const timerEvent::TickAlarm*);

#if !defined(WITHOUT_UPNP)
        bool on_UpnpPortMap(const rpcEvent::UpnpPortMap*);
        void upnp_enablePortmapping();
        void upnp_disablePortmapping();
#endif


        bool handleEvent(const REF_getter<Event::Base>& e);

        void doSend(const REF_getter<Session> & S);
        void doSendAll();
        void addSendPacket(const int& channel, const REF_getter<Session>&S, const REF_getter<oscarEvent::SendPacket>&P);
        void cleanSocket(const SOCKET_id& sid);

        struct _mx: public Mutexable
        {
            int64_t totalSendBufferSize;
            _mx():totalSendBufferSize(0) {}
        };
        _mx mx;
        IInstance* iInstance;
        bool m_isTerminating;


        void deinit()
        {
            ListenerBuffered1Thread::denit();
        }

        Service(const SERVICE_id &svs, const std::string&  nm,  IInstance *ifa);
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa);
        ~Service();

        Json::Value jdump();
    };
}


#endif

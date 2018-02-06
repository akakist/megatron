#ifndef ___________RPC__SERVER__H
#define ___________RPC__SERVER__H
#include "listenerBuffered.h"
#include "broadcaster.h"
#include "REF.h"
#include "SOCKET_id.h"


#include "ioBuffer.h"

#include "listenerBuffered1Thread.h"

#include "Events/System/Run/startService.h"


#include "IRPC.h"
#include "listenerSimple.h"
#include "tools_mt.h"
#include "logging.h"
#include "upnp_tr.h"
#include "socketsContainer.h"
#include "Events/System/Net/rpc/Disconnect.h"
#include "Events/System/Net/rpc/Disaccept.h"
#include "Events/System/Net/rpc/PassPacket.h"
#include "Events/System/Net/rpc/SendPacket.h"
#include "Events/System/Net/rpc/SubscribeNotifications.h"
#include "Events/System/Net/rpc/UnsubscribeNotifications.h"
#include "Events/System/Net/rpc/UpnpPortMap.h"
#include "Events/System/Net/rpc/UpnpResult.h"
#include "Events/System/Net/rpc/Connected.h"
#include "Events/System/Net/oscar/SendPacket.h"
#include "Events/System/Net/oscar/Connected.h"
#include "Events/System/Net/oscar/PacketOnAcceptor.h"
#include "Events/System/Net/oscar/PacketOnConnector.h"
#include "Events/System/Net/oscar/NotifyBindAddress.h"
#include "Events/System/Net/oscar/NotifyOutBufferEmpty.h"
#include "Events/System/Net/oscar/Accepted.h"
#include "Events/System/Net/oscar/SocketClosed.h"
#include "Events/System/Net/oscar/ConnectFailed.h"
#include "Events/System/timer/TickAlarm.h"
#include "Events/System/timer/TickTimer.h"

namespace RPC
{

    namespace timers
    {
        enum
        {
            CONNECTION_ACTIVITY
        };
    }

    struct Session: public Refcountable, public Mutexable
    {

        std::deque<std::string> m_cache;
        std::map<int,std::deque<REF_getter<oscarEvent::SendPacket> > > m_OutEventCache;
        time_t last_time_hit;
        enum currentState {EMPTY,FULL};
        currentState cstate;
        int64_t bufferSize;
        bool m_connectionEstablished;
        void update_last_time_hit()
        {
            last_time_hit=time(NULL);
        }

        Session():last_time_hit(time(NULL)), cstate(EMPTY), bufferSize(0),m_connectionEstablished(false) {}

        Json::Value jdump()
        {
            Json::Value v;

            v["cache_size"]=iUtils->toString((int64_t)m_cache.size());
            v["last_time_hit_dt"]=iUtils->toString(uint64_t(time(NULL)-last_time_hit));
            v["m_connectionEstablished"]=m_connectionEstablished;
            return v;
        }
    };
    struct __sessions: public SocketsContainerBase,Broadcaster
    {
        Mutex m_lock;

        //IInstance* iInstance;
        msockaddr_in getAddrOnConnected(const SOCKET_id& id);

        __sessions(IInstance* ifa):SocketsContainerBase("rpc::sessions"),Broadcaster(ifa) {}
        void on_mod_write(const REF_getter<epoll_socket_info>&) {}
        void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason);

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
            M_LOCK(m_lock);
            std::map<SOCKET_id, REF_getter<Session> > ::iterator i=all.m_socketId2session.find(sid);

            if(i!=all.m_socketId2session.end())
                return i->second;

            return NULL;
        }
        std::map<SOCKET_id, REF_getter<Session> > getSessionContainer()
        {
            M_LOCK(m_lock);
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
        private UnknownBase,
        private ListenerBuffered1Thread,
        private Broadcaster,
        public IRPC,
        public Logging
    {

        std::set<msockaddr_in>m_bindAddr_main;
        std::set<msockaddr_in>m_bindAddr_reserve;

        SERVICE_id myOscar;
        const real m_iterateTimeout;

        struct _mintAddr: public Mutexable
        {
            std::set<msockaddr_in> m_internalAddr;
        };
        _mintAddr mintAddr;
        bool m_networkInitialized;
        const real m_connectionActivityTimeout;
//#ifndef __MACH__
        struct _upnp
        {
            tr_upnp* upnp;
            Mutex upnpMX;
            _upnp():upnp(NULL) {}

        };
        _upnp upnp;
//#endif
//#endif

    protected:

    public:
        unsigned short getExternalListenPortMain(); // network byte order
        //unsigned short getExternalListenPortReserve(); // network byte order
        std::set<msockaddr_in> getInternalListenAddrs(); // network byte order

        REF_getter<__sessions> sessions;
    public:
        int64_t getTotalSendBufferSize();

        bool on_Disconnect(const rpcEvent::Disconnect*);
        bool on_Disaccept(const rpcEvent::Disaccept*);

        bool on_Connected(const oscarEvent::Connected*);
        bool on_PacketOnConnector(const oscarEvent::PacketOnConnector*);
        bool on_PacketOnAcceptor(const oscarEvent::PacketOnAcceptor*);
        bool on_NotifyBindAddress(const oscarEvent::NotifyBindAddress*);
        bool on_NotifyOutBufferEmpty(const oscarEvent::NotifyOutBufferEmpty*);
        bool on_Accepted(const oscarEvent::Accepted*);
        bool on_SocketClosed(const oscarEvent::SocketClosed*);
        bool on_ConnectFailed(const oscarEvent::ConnectFailed*e);


        bool on_startService(const systemEvent::startService*);

        bool on_PassPacket(const rpcEvent::PassPacket*);
        bool on_SendPacket(const rpcEvent::SendPacket*);

        bool on_SubscribeNotifications(const rpcEvent::SubscribeNotifications* E);
        bool on_UnsubscribeNotifications(const rpcEvent::UnsubscribeNotifications* E);

        bool on_TickAlarm(const timerEvent::TickAlarm*);

        bool on_UpnpPortMap(const rpcEvent::UpnpPortMap*);


#if !defined(WITHOUT_UPNP)
        void upnp_enablePortmapping();
        void upnp_disablePortmapping();
#endif


        bool handleEvent(const REF_getter<Event::Base>& e);

        void directHandleEvent(const REF_getter<Event::Base>&);
        void doSend(const REF_getter<Session> & S);
        void doSendAll();
        void addSendPacket(const int& channel, const REF_getter<Session>&S, const REF_getter<oscarEvent::SendPacket>&P);

        struct _mx: public Mutexable
        {
            int64_t totalSendBufferSize;
            _mx():totalSendBufferSize(0) {}
        };
        _mx mx;
        IInstance* iInstance;
        bool m_isTerminating;

        bool init(IConfigObj*);
        Service(const SERVICE_id &svs, const std::string&  nm,  IInstance *ifa);
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa);
        ~Service();
    };
}


#endif

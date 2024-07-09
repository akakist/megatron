#ifndef ___________RPC__SERVER__H
#define ___________RPC__SERVER__H
#include <REF.h>
#include <webDumpable.h>
#include <SOCKET_id.h>

//#include "event.h"
#include <epoll_socket_info.h>
#include <json/value.h>
#include <broadcaster.h>
#include <msockaddr_in.h>
#include <listenerSimple.h>
#include <IRPC.h>

#include "Events/System/Net/rpcEvent.h"
#include "Events/System/Net/oscarEvent.h"
#include "Events/Tools/webHandlerEvent.h"

#include <Events/System/timerEvent.h>
#include <Events/System/Run/startServiceEvent.h>

namespace RPC
{

    namespace timers
    {
        enum
        {
            CONNECTION_ACTIVITY
        };
    }

    struct outCache: public Refcountable
    {
        std::deque<REF_getter<refbuffer> >  container;
        RWLock lk;

    };
    struct Session: public Refcountable, WebDumpable
    {

        SOCKET_id socketId;
        REF_getter<epoll_socket_info> esi;
        REF_getter<outCache> outCache_;

        Session(SOCKET_id sockId,const REF_getter<epoll_socket_info>& _esi):socketId(sockId),
            esi(_esi),outCache_(nullptr) {}

        Json::Value jdump()
        {
            Json::Value v;
            v["socketId"]=std::to_string(CONTAINER(socketId));
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
    struct __sessions
    {


        __sessions()
            {}

        struct subscr {
        RWLock lock_;
        std::set<route_t> container_;
        };

        subscr subscribers_;

        struct sock2sess
        {

            RWLock lock_;
            std::map<SOCKET_id, REF_getter<Session> > container_;
        };
        sock2sess sock2sess_;

        struct sa2sess
        {
            RWLock lock_;
            std::map<msockaddr_in,REF_getter<Session> > container_;

        };
        sa2sess sa2sess_;

        // all
        struct passCache{
            RWLock lock_;
            std::map<SOCKET_id,std::deque<REF_getter<refbuffer>>>passCache;
        };
        passCache passCache_;

       void clear()
        {
            {
                WLocker l(sa2sess_.lock_);
                sa2sess_.container_.clear();
            }
            {
                WLocker l(subscribers_.lock_);
                subscribers_.container_.clear();
            }
            {
                WLocker l(sock2sess_.lock_);
                sock2sess_.container_.clear();
            }
        }
    public:
    };

    class Service:
        public UnknownBase,
        public ListenerSimple,
        public Broadcaster,
        public IRPC
    {


        SERVICE_id myOscar;
        const real iterateTimeout_;

        ListenerBase* myOscarListener;

        std::set<msockaddr_in>m_bindAddr_main;
        std::set<msockaddr_in>m_bindAddr_reserve;

        struct _shared_Addr
        {
            RWLock lk;
            _shared_Addr()
            {}
            std::set<msockaddr_in> m_internalAddr;
            std::set<msockaddr_in>m_bindAddr_mainSH;
            std::set<msockaddr_in>m_bindAddr_reserveSH;
        };
        _shared_Addr sharedAddr;
        const real m_connectionActivityTimeout;

    protected:

    public:
        unsigned short getExternalListenPortMain(); // network byte order
        std::set<msockaddr_in> getInternalListenAddrs(); // network byte order

        __sessions sessions;
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


        bool on_TickAlarm(const timerEvent::TickAlarm*);



        bool handleEvent(const REF_getter<Event::Base>& e);

        void flushAll();
        void addSendPacket(const REF_getter<Session>&S, const REF_getter<refbuffer> &P);
        void flushOutCache(const REF_getter<Session> & S);


        IInstance* iInstance;
        bool m_isTerminating;


        void deinit()
        {
            ListenerSimple::deinit();
        }

        Service(const SERVICE_id &svs, const std::string&  nm,  IInstance *ifa);
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa);
        ~Service();

    };
}


#endif

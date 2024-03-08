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

#include "Events/System/Net/rpc/PassPacket.h"
#include "Events/System/Net/rpc/SendPacket.h"
#include "Events/System/Net/rpc/SubscribeNotifications.h"
#include "Events/System/Net/rpc/UnsubscribeNotifications.h"
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

    struct Session: public Refcountable, WebDumpable
    {

        SOCKET_id socketId;
        std::deque<REF_getter<refbuffer> >  m_OutEventCache;
        REF_getter<epoll_socket_info> esi;

        Session(SOCKET_id sockId,const REF_getter<epoll_socket_info>& _esi):socketId(sockId),
            esi(_esi) {}

        Json::Value jdump()
        {
            Json::Value v;
            v["socketId"]=std::to_string(CONTAINER(socketId));
            v["OutEventCache size"]=std::to_string(m_OutEventCache.size());
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
        RWLock m_lock;

        // all
        std::set<route_t> mx_subscribers;
        std::map<SOCKET_id, REF_getter<Session> > mx_socketId2session;
        std::map<msockaddr_in,REF_getter<Session> > mx_sa2Session;

        // connector

        Json::Value jdump();

        void clear()
        {
            WLocker lk(m_lock);
            mx_sa2Session.clear();
            mx_subscribers.clear();
            mx_socketId2session.clear();
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
        const real m_iterateTimeout;

        ListenerBase* myOscarListener;

        std::set<msockaddr_in>m_bindAddr_main;
        std::set<msockaddr_in>m_bindAddr_reserve;

        struct _shared_Addr: public Mutexable
        {
            _shared_Addr()
//            :m_networkInitialized(false)
            {}
            std::set<msockaddr_in> m_internalAddr;
//        bool m_networkInitialized;
            std::set<msockaddr_in>m_bindAddr_mainSH;
            std::set<msockaddr_in>m_bindAddr_reserveSH;
        };
        _shared_Addr sharedAddr;
        const real m_connectionActivityTimeout;

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



        bool handleEvent(const REF_getter<Event::Base>& e);

        void doSend(const REF_getter<Session> & S);
        void doSendAll();
        void addSendPacket(const REF_getter<Session>&S, const REF_getter<refbuffer> &P);

        IInstance* iInstance;
        bool m_isTerminating;


        void deinit()
        {
            ListenerSimple::deinit();
        }

        Service(const SERVICE_id &svs, const std::string&  nm,  IInstance *ifa);
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa);
        ~Service();

        Json::Value jdump();
    };
}


#endif

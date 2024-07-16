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

#include <Events/System/timerEvent.h>
#include <Events/System/Run/startServiceEvent.h>
#include <Events/Tools/webHandlerEvent.h>

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

        Json::Value jdump()
        {
            Json::Value j;
            j["container.size()"]=(int)container.size();
            return j;
        }

    };
    struct Session: public Refcountable
#ifdef WEBDUMP
            , public WebDumpable
#endif
    {

        SOCKET_id socketId;
        REF_getter<epoll_socket_info> esi;
        outCache outCache_;

        Session(SOCKET_id sockId,const REF_getter<epoll_socket_info>& _esi):socketId(sockId),
            esi(_esi) {}

        Json::Value jdump()
        {
            Json::Value v;
            v["socketId"]=std::to_string(CONTAINER(socketId));
#ifdef WEBDUMP
            v["esi"]=esi->getWebDumpableLink("esi");
#endif
            v["outCache_"]=outCache_.jdump();
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
            Json::Value jdump()
            {
                Json::Value j;
                R_LOCK(lock_);
                for(auto &z: container_)
                {
                    j.append(z.dump());
                }
                return j;
            }
        };

        subscr subscribers_;

        struct sock2sess
        {

            RWLock lock_;
            std::map<SOCKET_id, REF_getter<Session> > container_;
            Json::Value jdump()
            {
                Json::Value j;
                R_LOCK(lock_);
                for(auto &z: container_)
                {
#ifdef WEBDUMP
                    j[std::to_string(z.first)]=z.second->getWebDumpableLink("Session");
#endif
                }
                return j;
            }

        };
        sock2sess sock2sess_;

        struct sa2sess
        {
            RWLock lock_;
            std::map<msockaddr_in,REF_getter<Session> > container_;
            Json::Value jdump()
            {
                Json::Value j;
                R_LOCK(lock_);
                for(auto &z: container_)
                {
#ifdef WEBDUMP
                    j[z.first.dump()]=z.second->getWebDumpableLink("Session");
#endif
                }
                return j;
            }


        };
        sa2sess sa2sess_;

        // all
        struct passCache{
            RWLock lock_;
            std::map<SOCKET_id,std::deque<REF_getter<refbuffer>>>passCache;
            Json::Value jdump()
            {
                Json::Value j;
                R_LOCK(lock_);
                for(auto &z: passCache)
                {
                    j[std::to_string(z.first)]=(int)z.second.size();
                }
                return j;
            }
        };
        passCache passCache_;

        Json::Value jdump()
        {
            Json::Value j;
            j["passCache_"]=passCache_.jdump();
            j["sa2sess_"]=sa2sess_.jdump();
            j["sock2sess_"]=sock2sess_.jdump();
            j["subscribers_"]=subscribers_.jdump();
            return j;
        }

        void clear()
        {
            {
                W_LOCK(sa2sess_.lock_);
                sa2sess_.container_.clear();
            }
            {
                W_LOCK(subscribers_.lock_);
                subscribers_.container_.clear();
            }
            {
                W_LOCK(sock2sess_.lock_);
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


        std::string myOscar;
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
        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);



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

        struct _evcount
        {
            RWLock lk;
            std::map<EVENT_id,int> ev_handled;
            void inc(EVENT_id id)
            {
                W_LOCK(lk);
                ev_handled[id]++;
            }
            Json::Value jdump()
            {
                Json::Value j;
                std::map<EVENT_id,int> m;
                {
                    R_LOCK(lk);
                    m=ev_handled;
                }
                for(auto &z: m)
                {
                    j[iUtils->genum_name(z.first)]=z.second;
                }
                return j;
            }
        };
        _evcount evcount;


    };
}


#endif


#ifndef ________TCP_LISTENER_____H
#define ________TCP_LISTENER_____H


#include <REF.h>
#include <SOCKET_id.h>
#include <epoll_socket_info.h>
#include <json/value.h>
#include <unknown.h>
#include <listenerSimple.h>
#include <broadcaster.h>
//#include "event.h"
#include <Events/System/Net/socket/AddToListenTCP.h>
#include <Events/System/Net/socket/AddToConnectTCP.h>
#include <Events/System/Run/startService.h>
#include <Events/System/timer/TickTimer.h>
#include <Events/Tools/webHandler/RequestIncoming.h>
#include "Events/System/Net/socket/Write.h"

namespace SocketIO
{
    struct SocketsContainerForSocketIO
        :public Refcountable
    {

    private:
        std::map<SOCKET_id,REF_getter<epoll_socket_info> > m_container;
    public:
        REF_getter<NetworkMultiplexor> multiplexor;
        RWLock lk;
        SocketsContainerForSocketIO(): multiplexor(new NetworkMultiplexor)
        {}
        ~SocketsContainerForSocketIO();
        void remove(const SOCKET_id& sid)
        {
            WLocker lock(lk);
            m_container.erase(sid);

        }
        std::map<SOCKET_id,REF_getter<epoll_socket_info> > getContainer()
        {
            RLocker lock(lk);
            return m_container;
        }
        size_t count(const SOCKET_id& sid)
        {
            RLocker lock(lk);
            return m_container.count(sid);
        }
        REF_getter<epoll_socket_info> getOrNull(const SOCKET_id& sid)
        {
            RLocker lock(lk);
            auto z=m_container.find(sid);
            if(z==m_container.end())
                return NULL;
            return z->second;
        }
        Json::Value jdump();

        void add(const REF_getter<epoll_socket_info>& esi)
        {
            WLocker lock(lk);
            m_container.insert(std::make_pair(esi->m_id,esi));
        }

        void clear()
        {
            multiplexor=NULL;
        }

    private:
    };

    class Service:
        public UnknownBase,
        public Broadcaster,
        public ListenerSimple

    {
    private:
        Json::Value jdump()
        {
            Json::Value v;
            v["listen_backlog"]=(int)m_listen_backlog;
            v["epoll_timeout_millisec"]=std::to_string(epoll_timeout_millisec);

            return v;
        }

        struct PTHSOCKS
        {
            RWLock lock;
            std::vector<REF_getter<SocketsContainerForSocketIO> > m_socks_pollers;
            std::deque<REF_getter<SocketsContainerForSocketIO> > for_threads;
            REF_getter<SocketsContainerForSocketIO> getPoller(SocketIO::Service* s)
            {
                if(m_socks_pollers.size()!=s->n_workers)
                    throw CommonError("if(m_socks_pollers.size()!=s->n_workers)");
                {
                    return m_socks_pollers[rand()%m_socks_pollers.size()];
                }
            }
        };
        PTHSOCKS m_io_socks_pollers;
        std::vector<pthread_t> m_pthread_id_worker;
        int64_t m_listen_backlog;
        int n_workers;
        /// конфигурационный параметр
        int epoll_timeout_millisec;


//#endif



        void closeSocket(const REF_getter<epoll_socket_info>&esi, const std::string& reason, int errNo, const REF_getter<SocketsContainerForSocketIO> &MS);
        bool on_AddToListenTCP(const socketEvent::AddToListenTCP*, const REF_getter<SocketsContainerForSocketIO> &MS);
        bool on_AddToConnectTCP(const socketEvent::AddToConnectTCP*, const REF_getter<SocketsContainerForSocketIO> &MS);
        bool on_startService(const systemEvent::startService*);
        bool on_TickTimer(const timerEvent::TickTimer*);
        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);
        void onEPOLLIN(const REF_getter<epoll_socket_info>& esi, const REF_getter<SocketsContainerForSocketIO> &MS);
        void onEPOLLIN_STREAMTYPE_LISTENING(const REF_getter<epoll_socket_info>&esi, const REF_getter<SocketsContainerForSocketIO> &MS);
        void onEPOLLIN_STREAMTYPE_CONNECTED_or_STREAMTYPE_ACCEPTED(const REF_getter<epoll_socket_info>&esi, const REF_getter<SocketsContainerForSocketIO> &MS);



        void onEPOLLOUT(const REF_getter<epoll_socket_info>& esi, const REF_getter<SocketsContainerForSocketIO> &MS);
        void onEPOLLERR(const REF_getter<epoll_socket_info>& esi, const REF_getter<SocketsContainerForSocketIO> &MS);
        bool  handleEvent(const REF_getter<Event::Base>&);
        void handle_accepted1(const SOCKET_fd &neu_fd, const REF_getter<epoll_socket_info> esi, const REF_getter<SocketsContainerForSocketIO>& MS);


        IInstance* iInstance;
        bool m_isTerminating;

    public:
        static UnknownBase *construct(const SERVICE_id& id, const std::string&  nm, IInstance* ifa);
        Service(const SERVICE_id& id, const std::string& nm,  IInstance *ifa);
        ~Service();
        void deinit()
        {
            ListenerSimple::deinit();
        }

    protected:
        static void *worker__(void*);
        void worker();



    };


};

#endif

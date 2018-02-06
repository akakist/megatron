#ifndef ________TCP_LISTENER_____H
#define ________TCP_LISTENER_____H

#include "unknown.h"
#include "broadcaster.h"
#include "listenerBuffered.h"
#include "SOCKET_fd.h"
#include "SOCKET_id.h"
#include "epoll_socket_info.h"


#include "Events/System/Run/startService.h"


#include "logging.h"
#include "mutexInspector.h"
#include "socketStats.h"
#include "socketsContainer.h"
#include "socketsContainerForSocketIO.h"
#include "Events/Tools/webHandler/RequestIncoming.h"
#include "Events/System/Net/socket/AddToConnectTCP.h"
#include "Events/System/Net/socket/AddToListenTCP.h"
#include "Events/System/Net/socket/Write.h"
#include "Events/System/timer/TickTimer.h"
#include "Events/System/timer/SetTimer.h"

#if defined(HAVE_EPOLL) || defined(HAVE_SELECT) || defined(HAVE_KQUEUE)
#else
#error you must specify HAVE_EPOLL or HAVE_SELECT
#endif
namespace SocketIO
{
    struct SocketsContainerForSocketIO:public SocketsContainerBase
    {

#ifdef HAVE_EPOLL
        e_poll m_epoll;
#endif
#ifdef HAVE_KQUEUE
        //k_queue m_kqueue;
        int m_kqueue;
        std::vector<struct kevent> evSet;
#endif

        SocketsContainerForSocketIO(): SocketsContainerBase("SocketsContainerForSocketIO") {}
        ~SocketsContainerForSocketIO();

        void addRead(const REF_getter<epoll_socket_info>&esi);
        void addRW(const REF_getter<epoll_socket_info>&esi);
        Json::Value jdump();
        void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason);
        void on_mod_write(const REF_getter<epoll_socket_info>&esi);

        void clear()
        {
            SocketsContainerBase::clear();
#ifdef HAVE_KQUEUE
            evSet.clear();
#endif
        }

    private:
    };


    class Service:
        private UnknownBase,
        private Broadcaster,
        private ListenerBuffered,
        public Logging

    {
    private:
        Json::Value jdump()
        {
            Json::Value v;
            v["total_recv"]=iUtils->toString(m_total_recv);
            v["total_send"]=iUtils->toString(m_total_send);
            v["total_accepted"]=iUtils->toString(m_total_accepted);
            v["listen_backlog"]=(int)m_listen_backlog;
            v["sockets"]=m_socks->jdump();
            return v;
        }
        int64_t m_total_recv, m_total_send,m_total_accepted;

        REF_getter<SocketsContainerForSocketIO> m_socks;
#ifdef WITH_TCP_STATS
        __stats m_stats;
#endif
        pthread_t m_pthread_id_worker;
        pthread_t m_pthread_id_acceptor;
        int64_t m_listen_backlog;
        const int64_t maxOutBufferSize;
#ifdef HAVE_EPOLL
        /// конфигурационный параметр
        int epoll_size;
        /// конфигурационный параметр
        int epoll_timeout_millisec;
#endif
#ifdef HAVE_KQUEUE
        /// конфигурационный параметр
        int kqueue_size;
        /// конфигурационный параметр
        int kqueue_timeout_millisec;
#endif



        void closeSocket(const REF_getter<epoll_socket_info>&esi, const std::string& reason);
        bool on_AddToListenTCP(const socketEvent::AddToListenTCP*);
        bool on_AddToConnectTCP(const socketEvent::AddToConnectTCP*);
        bool on_startService(const systemEvent::startService*);
        bool on_TickTimer(const timerEvent::TickTimer*);
        bool on_Write(const socketEvent::Write*);
        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);
        void onEPOLLIN(const REF_getter<epoll_socket_info>& esi);
        void onEPOLLOUT(const REF_getter<epoll_socket_info>& esi);
        void onEPOLLERR(const REF_getter<epoll_socket_info>& esi);
        void closeSocket(const REF_getter<epoll_socket_info>& esi);
        bool  handleEvent(const REF_getter<Event::Base>&);

        IInstance* iInstance;
        bool m_isTerminating;

    public:
        static UnknownBase *construct(const SERVICE_id& id, const std::string&  nm, IInstance* ifa);
        Service(const SERVICE_id& id, const std::string& nm,  IInstance *ifa);
        ~Service();
        bool init(IConfigObj*);
        bool deinit();
    protected:
        static void *worker(void*);


    };


};

#endif

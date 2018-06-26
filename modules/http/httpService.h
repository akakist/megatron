#ifndef _____HTTP___SERVICE______H
#define _____HTTP___SERVICE______H
#include "unknown.h"

#include "broadcaster.h"
#include "listenerBuffered1Thread.h"
#include "SOCKET_id.h"
#include "httpConnection.h"

#include "unknown.h"
#include "mutexInspector.h"



#include "Events/System/Run/startService.h"
#include "Events/System/Net/socket/Accepted.h"
#include "Events/System/Net/socket/StreamRead.h"
#include "Events/System/Net/socket/Connected.h"
#include "Events/System/Net/socket/NotifyBindAddress.h"
#include "Events/System/Net/socket/NotifyOutBufferEmpty.h"
#include "Events/System/Net/http/DoListen.h"
#include "Events/System/Net/http/RegisterProtocol.h"
#include "Events/System/Net/http/GetBindPorts.h"
#include "Events/System/Net/http/RequestIncoming.h"
#include "logging.h"
#include "socketsContainer.h"
namespace HTTP
{

    class __http_stuff: public SocketsContainerBase
    {
        Mutex m_lock;
        std::map<SOCKET_id,REF_getter<HTTP::Request> > container;
    public:
        __http_stuff():SocketsContainerBase("__http_stuff") {}
        REF_getter<HTTP::Request> getRequestOrNull(const SOCKET_id& id);
        void insert(const SOCKET_id& id,const REF_getter<HTTP::Request> &C);
        virtual ~__http_stuff() {}
        void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason);
        void on_mod_write(const REF_getter<epoll_socket_info>&) {}
        void clear()
        {
            SocketsContainerBase::clear();
            {
                M_LOCK(m_lock);
                container.clear();
            }
        }

    };

    class Service:
        private UnknownBase,
        private Broadcaster,
        private ListenerBuffered1Thread,
        public Logging
    {

        // config
        size_t m_maxPost;
        struct _mx: public Mutexable
        {
            std::set<std::string> docUrls;
            std::string documentRoot;
            std::map<std::string,std::string>mime_types;
            std::map<std::string,HTTP::IoProtocol> protocols;
            std::set<msockaddr_in> bind_addrs;
        };
        _mx mx;
        //!config
        bool on_StreamRead(const socketEvent::StreamRead* evt);
        bool on_Accepted(const socketEvent::Accepted* evt);
        bool on_Connected(const socketEvent::Connected*);
        bool on_NotifyBindAddress(const socketEvent::NotifyBindAddress*);

        bool on_DoListen(const httpEvent::DoListen*);
        bool on_RegisterProtocol(const httpEvent::RegisterProtocol*e)
        {

            M_LOCK(mx);
            mx.protocols.insert(std::make_pair(e->url,e->protocol));

            return true;
        }


        bool on_startService(const systemEvent::startService*);
        bool on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty* );
        bool on_GetBindPortsREQ(const httpEvent::GetBindPortsREQ*);



    public:
        static UnknownBase*construct(const SERVICE_id&id, const std::string&nm, IInstance *_if);
        Service(const SERVICE_id& id, const std::string& nm, IInstance *_if);
        ~Service() {
            _stuff->clear();
        }


        REF_getter<__http_stuff> _stuff;
        std::string get_mime_type(const std::string& mime) const;

    protected:
        bool handleEvent(const REF_getter<Event::Base>& evt);
        /** -1 error*/
        int send_other_from_disk_ext(const REF_getter<epoll_socket_info>&esi, const REF_getter<HTTP::Request>&req,const std::string & fn,const std::string& exten);

    private:
        struct _lastModified: public Mutexable
        {
            std::map<std::string,time_t> container;
        };
        _lastModified lastModified;


        struct _senderIo: public Mutexable
        {
            std::map<int64_t,std::pair<REF_getter<HTTP::Request>,std::vector<std::string> > > container;
        };
        _senderIo senderIo;
    };
};
#endif

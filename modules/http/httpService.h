#ifndef _____HTTP___SERVICE______H
#define _____HTTP___SERVICE______H

#include "unknown.h"

#include "broadcaster.h"
#include "listenerSimple.h"
#include "httpConnection.h"

#include "unknown.h"
#include <Events/System/Net/socketEvent.h>
#include <Events/System/Net/httpEvent.h>
#include <Events/System/Run/startServiceEvent.h>



namespace HTTP
{


    class Service:
        public UnknownBase,
        public Broadcaster,
        public ListenerSimple
    {

        // config
        size_t m_maxPost;
        struct _mx
        {
            RWLock lk;
            std::set<std::string> docUrls;
            std::string documentRoot;
            std::map<std::string,std::string>mime_types;
            std::map<std::string,HTTP::IoProtocol> protocols;
            std::set<msockaddr_in> bind_addrs;
        };
        _mx mx;
        ListenerBase* socketListener;
        //!config
        bool on_StreamRead(const socketEvent::StreamRead* evt);
        bool on_Accepted(const socketEvent::Accepted* evt);
        bool on_Connected(const socketEvent::Connected*);
        bool on_NotifyBindAddress(const socketEvent::NotifyBindAddress*);
        bool on_Disaccepted(const socketEvent::Disaccepted*);
        bool on_Disconnected(const socketEvent::Disconnected*);

        bool on_DoListen(const httpEvent::DoListen*);
        bool on_RegisterProtocol(const httpEvent::RegisterProtocol*e)
        {

            WLocker aaa(mx.lk);
            mx.protocols.insert(std::make_pair(e->url,e->protocol));

            return true;
        }


        bool on_startService(const systemEvent::startService*);
        bool on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty* );
        bool on_GetBindPortsREQ(const httpEvent::GetBindPortsREQ*);

        REF_getter<HTTP::Request> getData(epoll_socket_info* esi);
        void setData(epoll_socket_info* esi, const REF_getter<HTTP::Request> & p);
        void clearData(epoll_socket_info* esi);


    public:
        void deinit()
        {
            ListenerSimple::deinit();
        }
        static UnknownBase*construct(const SERVICE_id&id, const std::string&nm, IInstance *_if);
        Service(const SERVICE_id& id, const std::string& nm, IInstance *_if);
        ~Service() {
        }


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
        IInstance *iInstance;
    };
};
#endif

#ifndef ___________PP__SERVER__H
#define ___________PP__SERVER__H
#include "listenerBuffered.h"
#include "listenerPolled.h"
#include "listenerBuffered1Thread.h"
#include "broadcaster.h"
#include "REF.h"
#include "SOCKET_id.h"


#include "ioBuffer.h"
#include "Events/System/Run/startService.h"
#include "version_mega.h"
#include "IObjectProxy.h"
#include "logging.h"
class ObjectHandler;

namespace ObjectProxy
{
    class Base
    {
    public:

    };
    class Polled:
        public UnknownBase,
        public Broadcaster,
        public IObjectProxyPolled,
        public ListenerPolled,
        public Base,
        public Logging
    {
    public:

        Mutex __m_lock;
        std::set<std::string> __mx_handlers;

        void sendObjectRequest(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e);
        void sendObjectRequest(const SERVICE_id & dstService, const REF_getter<Event::Base>& e);
        bool handleEvent(const REF_getter<Event::Base>& e);

        void addObjectHandler(ObjectHandlerPolled* h);
        void removeObjectHandler(ObjectHandlerPolled* h);


        bool on_startService(const systemEvent::startService*)
        {
            return true;
        }

        Polled(const SERVICE_id &svs, const std::string& nm,IInstance* ifa):
            UnknownBase(nm),
            Broadcaster(ifa),
            IObjectProxyPolled(this),
            ListenerPolled(this,nm,ifa->getConfig(),svs),
            Logging(this,ERROR_log,ifa)
        {
        }

        ~Polled()
        {
        }
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
        {
            XTRY;
            return new Polled(id,nm,ifa);
            XPASS;
        }

        void poll()
        {
            ListenerPolled::poll();
        }
    };
    class Threaded:
        public UnknownBase,
        public Broadcaster,
        public IObjectProxyThreaded,
        public ListenerBuffered1Thread,
        public Base,
        public Logging
    {
    public:
        Mutex __m_lock;
        std::set<std::string> __mx_handlers;

        void addObjectHandler(ObjectHandlerThreaded* h);
        void removeObjectHandler(ObjectHandlerThreaded* h);

        void sendObjectRequest(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e);
        void sendObjectRequest(const SERVICE_id & dstService, const REF_getter<Event::Base>& e);
        bool handleEvent(const REF_getter<Event::Base>& e);
        bool on_startService(const systemEvent::startService*) {
            return true;
        }



    public:

        Threaded(const SERVICE_id &svs, const std::string& nm,IInstance* ifa):UnknownBase(nm),
            Broadcaster(ifa),
            IObjectProxyThreaded(this),
            ListenerBuffered1Thread(this,nm,ifa->getConfig(),svs,ifa),
            Logging(this,ERROR_log,ifa)
        {
        }

        ~Threaded()
        {
        }
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
        {
            XTRY;
            return new Threaded(id,nm,ifa);
            XPASS;
        }
    };

}
#endif

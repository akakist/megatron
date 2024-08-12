#pragma once
#include "unknown.h"
#include "SERVICE_id.h"
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#include "IInstance.h"
#include "listenerBuffered1Thread.h"
#include "broadcaster.h"
#include <ostream>


#include "Events/System/Run/startServiceEvent.h"
#include "Events/System/Net/rpcEvent.h"
#include "Events/Tools/errorDispatcherEvent.h"
namespace ErrorDispatcher
{

    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster

    {
        bool on_errorDispatcherSendMessage(const errorDispatcherEvent::SendMessage*);
        bool on_errorDispatcherSubscribe(const errorDispatcherEvent::Subscribe*);
        bool on_errorDispatcherUnsubscribeAll(const errorDispatcherEvent::Unsubscribe*);
        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);
        bool on_IncomingOnAcceptor(const rpcEvent::IncomingOnAcceptor*);


        Service(const SERVICE_id&, const std::string&  nm, IInstance *ifa);
        ~Service();


        std::set<route_t> m_subscribers;
        //std::map<std::string,std::string> m_cache;

    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
        {
            XTRY;
            return new Service(id,nm,ifa);
            XPASS;
        }

    private:

    };
};
